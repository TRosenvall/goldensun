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
