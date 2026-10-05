/* Func_80175c0 -- PARK STANDS at 2 of 44, and the park's ELEMENT-TYPE ASYMMETRY
 * NOW HAS ITS MECHANISM, read in the compiler.  Batch 327 brief C.
 *
 *   2 differing encodings of 44.  ref 44 encodings / 44 insns / 96 bytes, ours
 *   44 / 44 / 96.  Relocations identical, both pool words identical.
 *   Re-measured; identical to the batch-323 figure.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/rom_175c0.c asm/rom_15000/rom_15e8c_c_a_c_c_c.s --func Func_80175c0
 *
 * The residue is unchanged and is one fact twice: idx 13 `sub sp,#16` against
 * our `#8`, idx 38 `add sp,#16` against our `#8`.  `thumb_expand_prologue`
 * (arm.c:8926-8945) is confirmed verbatim -- `amount = get_frame_size () +
 * current_function_outgoing_args_size`, then `ROUND_UP` (arm.h:843) -- and
 * ** THERE IS NO THIRD TERM. **  outgoing_args_size is 8 (the six-argument
 * call's two stack words, which the ROM writes at [sp] and [sp,#4]), so the
 * ROM's get_frame_size() is in [5,8] and ours is 0.
 *
 * ============ THE ASYMMETRY IS `stor-layout.c`, AND IT IS NOT ELEMENT TYPE
 *
 * The park recorded "an unread 8-byte array of 1- or 2-byte elements gets a
 * frame slot, an unread 8-byte array or struct of 4-byte elements does not" as
 * "a MEASUREMENT and not yet a mechanism".  The mechanism is exact:
 *
 * ** AN UNREFERENCED LOCAL OCCUPIES FRAME SPACE ONLY IF ITS TYPE STAYS BLKmode. **
 * Otherwise `expand_decl` gives it a PSEUDO, and an unreferenced pseudo costs
 * nothing.  Type layout hands out an integer mode aggressively:
 *
 *   ARRAY_TYPE  (stor-layout.c:1432-1449)  TYPE_MODE starts BLKmode, then
 *     mode_for_size_tree is tried, and it is forced BACK to BLKmode only when
 *         STRICT_ALIGNMENT && TYPE_ALIGN <  BIGGEST_ALIGNMENT
 *                          && TYPE_ALIGN <  GET_MODE_ALIGNMENT (that mode)
 *   RECORD/UNION (stor-layout.c:1076-1093, compute_record_mode)  same shape,
 *     guard `! (TYPE_ALIGN >= BIGGEST_ALIGNMENT || TYPE_ALIGN >= ...)`.
 *
 * and arm.h fixes all three constants: BIGGEST_ALIGNMENT 32 (:679),
 * STRICT_ALIGNMENT 1 (:708), DEFAULT_STRUCTURE_SIZE_BOUNDARY ** 32 ** (:700).
 *
 * So the rule is:
 *   - an ARRAY is decided by its ELEMENT WIDTH, because that sets TYPE_ALIGN.
 *     `char[8]`/`short[4]` have TYPE_ALIGN 8/16 < 32, so DImode is refused and
 *     they stay BLKmode -> A FRAME SLOT.  `int[2]`/`int *[2]` have TYPE_ALIGN
 *     32, which is >= BIGGEST_ALIGNMENT, so the first conjunct fails, they KEEP
 *     DImode -> a pseudo -> NO FRAME AT ALL.
 *   - a STRUCT OR UNION is decided only by whether it has a BLKmode MEMBER,
 *     because STRUCTURE_SIZE_BOUNDARY 32 makes TYPE_ALIGN >= BIGGEST_ALIGNMENT
 *     true for EVERY struct, satisfying compute_record_mode's first disjunct.
 *   - a non-BLKmode SCALAR (`double`, `long long`) is always a pseudo.
 *
 * Three predictions of that rule were measured and all three held:
 * `struct { char a[8]; }` and `union { char a[8]; }` DO get the slot (BLKmode
 * member) while `struct { char a,b,c,d,e,f,g,h; }` and
 * `struct { short a,b,c,d; }` DO NOT (8 bytes, alignment 2, but
 * STRUCTURE_SIZE_BOUNDARY promotes TYPE_ALIGN to 32); and
 * `__attribute__((aligned(4)))` on `char[8]`/`short[4]` is INERT, because it
 * sets DECL_ALIGN and TYPE_MODE is a property of the TYPE.
 *
 * ** AND ONE PARK FIGURE IS CORRECTED: `int pad[2]` does not get "the same 8
 * bytes and no slot" -- it gets NO FRAME AT ALL.  It emits `sub sp,#8`, i.e.
 * get_frame_size() == 0. **
 *
 * RE-MEASURED THIS BATCH (ref 44 encodings; every row at 44 instructions):
 *   char pad[8]                              0  sub sp,#16  <- DEVICE
 *   short pad[4]                             0  sub sp,#16  <- DEVICE
 *   char pad[5] / [6] / [7]                  0  sub sp,#16  <- DEVICE
 *   char pad[2][4]                           0  sub sp,#16  <- DEVICE
 *   struct { char a[8]; }                    0  sub sp,#16  <- DEVICE
 *   union  { char a[8]; }                    0  sub sp,#16  <- DEVICE
 *   char pad[4]; char pad2[4];               0  sub sp,#16  <- DEVICE
 *   char pad[6]; short pad2;                 0  sub sp,#16  <- DEVICE
 *   short pad[4] __attribute__((aligned(4))) 0  (attribute inert)
 *   char  pad[8] __attribute__((aligned(4))) 0  (attribute inert)
 *   short pad[2]                             2  sub sp,#12
 *   char pad[9]                              2  sub sp,#20
 *   char pad[16]                             2  sub sp,#24
 *   int pad[2]                               2  sub sp,#8   NO FRAME
 *   int *pad[2]                              2  sub sp,#8   NO FRAME
 *   struct { int a, b; }                     2  sub sp,#8   NO FRAME
 *   struct { short a, b, c, d; }             2  sub sp,#8   NO FRAME
 *   struct { char a,b,c,d,e,f,g,h; }         2  sub sp,#8   NO FRAME
 *   double / long long                       2  sub sp,#8   NO FRAME
 *
 * EVERY zero above is a NEVER-READ LOCAL, which is a DEVICE by this project's
 * standard (docs/owner-decisions.md item 4).  So the device-free body ships at
 * 2 and the 0 stays a figure ABOUT the blocker.
 *
 * ** THE BOUND, NOW SHARP: the construct has to be a 5-to-8-byte local whose
 * TYPE IS BLKmode -- an array of 1- or 2-byte elements, or a struct/union with
 * such an array as a member -- AND IT MUST NEVER BE READ, because any read of
 * it emits an instruction and the other 42 encodings are already exact. **  No
 * construct meeting both halves is anything other than a device, so closing
 * this needs evidence from OUTSIDE the function: a struct type this module
 * already uses whose size is 5..8 with sub-word alignment, declared here for a
 * call this version does not make.  `tools/upstream_module.py Func_80175c0`
 * and the module's landed siblings are where to look, not another spelling.
 */

/* Func_80175c0 -- 0x080175c0  (asm/rom_15000/rom_15e8c_c_a_c_c_c.s)
 *
 * NON-MATCHING, 2 of 44 encodings.  MEASURED in batch 323, brief I.
 * Previous park figure: 20 of 44 (rom_175c0.c), and 28 of 44 (80175c0.c,
 * the DUPLICATE park -- retire it).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/rom_175c0.c \
 *     asm/rom_15000/rom_15e8c_c_a_c_c_c.s --func Func_80175c0
 *
 * SPLIT SHAPE: none.  asm/rom_15000/rom_15e8c_c_a_c_c_c.s holds exactly one
 * .thumb_func_start (Func_80175c0), so this would convert WHOLE.
 * PINS: 0.  No register pin, no inline asm, no device.
 *
 * WHAT THE RESIDUE IS -- exactly two encodings, and they are the SAME fact
 * twice:
 *
 *     idx 13   rom  sub sp, #16      ours  sub sp, #8
 *     idx 38   rom  add sp, #16      ours  add sp, #8
 *
 * Instruction count 44 against 44, size 96 against 96, relocations identical,
 * both pool words identical, and every other encoding exact.
 *
 * THE MECHANISM, read out of the compiler rather than inferred.
 * arm.c:8928 thumb_expand_prologue:
 *
 *     HOST_WIDE_INT amount = (get_frame_size ()
 *                             + current_function_outgoing_args_size);
 *     ...
 *     amount = ROUND_UP (amount);        / * arm.h:843: (((X) + 3) & ~3) * /
 *
 * The six-argument call needs two outgoing stack words, so
 * current_function_outgoing_args_size is 8 and our get_frame_size() is 0:
 * amount = 8.  The ROM's amount is 16, so the ROM's frame carried
 * get_frame_size() in [5, 8] -- and it is NOT a spill, because the ROM never
 * touches [sp,#8] or [sp,#12] anywhere in the function.  It is a stack slot
 * that was allocated and never accessed.
 *
 * MEASURED, as a DEVICE and not shipped: adding an unread 8-byte local to this
 * body gives 0 of 44 -- byte-identical.  Figures, all against this same body:
 *
 *     char pad[8];            0 of 44    sub sp, #16   <-- DEVICE, byte-identical
 *     short pad[4];           0 of 44    sub sp, #16   <-- DEVICE, byte-identical
 *     int pad[2];             2 of 44    sub sp, #8    (same 8 bytes, no slot!)
 *     struct { int a, b; };   2 of 44    sub sp, #8
 *     char pad[4];            2 of 44    sub sp, #8    (4 bytes -> amount 12)
 *     char pad[1];            2 of 44    sub sp, #8
 *     int pad[3];             2 of 44    sub sp, #8    (12 bytes -> amount 20)
 *     int w; ... *&w          2 of 44    sub sp, #8    (purge_addressof folds it)
 *
 * A never-read local is a DEVICE by this project's standard, so the 0 is
 * recorded as a figure ABOUT THE BLOCKER and the device-free body ships at 2.
 * Note the asymmetry, which is a MEASUREMENT and not yet a mechanism: an unread
 * 8-byte array of 1- or 2-byte elements gets a frame slot, an unread 8-byte
 * array or struct of 4-byte elements does not.  Whatever a real source
 * construct for those 8 bytes is, it has to survive gcc-2.96 the way
 * `char[8]`/`short[4]` do and `int[2]`/`struct{int,int}` do not.
 *
 * WHAT CLOSED THE OTHER 18.  The park's "WHAT REMAINS" item 1 said "there is
 * no source-level way to say these two zeros are different", after four
 * initialisation positions.  That diagnosis is REFUTED, and the reason is that
 * the two zeros are not two values -- THEY ARE ONE VARIABLE.  The ROM's r6
 * holds the stored zero from the prologue, is passed as the fifth argument at
 * `str r6, [sp]`, and is then OVERWRITTEN BY THE CALL'S RESULT at
 * `adds r6, r0, #0`.  So r6 is the result variable, initialised to 0 and used
 * as the zero argument, and the ROM's two `movs r0, #0` are the `return 0;`
 * paths.  Declaring one `int r = 0`, storing it, passing it, and assigning the
 * call's result back into it takes 20 -> 2 in a single edit.
 *
 * Also settled: the park's separate claim that we "come out at 42 lines, not
 * 43" because of a missing `mov r6, r0` is the same fact -- that instruction is
 * the result assignment, and it is present once `r` is one variable.
 */
extern int iwram_3001e8c;
extern int BufferString(int s, int n);
extern int Func_80165d8(int a, int b, int c, int d, int e, int f);

int Func_80175c0(int a, int b)
{
    char *base;
    unsigned short *p;
    int off;
    int idx;
    int t;
    int r;

    off = 0x12f4;
    base = (char *)iwram_3001e8c;
    r = 0;
    p = (unsigned short *)(base + off);
    *p = r;
    off += 2;
    p = (unsigned short *)(base + off);
    *p = r;
    idx = BufferString(b, 1);
    t = (idx << 1) + (0xeb << 4);
    if (*(unsigned short *)(base + t) == 0)
        return 0;
    if (a == 0)
        return 0;
    r = Func_80165d8(a, idx, 0, 0, r, 1);
    if (r == 0)
        return 0;
    return r;
}
