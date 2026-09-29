/* PARKED -- OvlFunc_924_20099b8, 4 of 229 encodings differ
 * NON-MATCHING, 61 encodings of 229.  A TRUE DISTANCE under the tree's CURRENT flags -- 229 = 229 encodings, no SIZE line, all 39
 * relocations identical.
 *
 * IT REACHES 4 OF 229 WITH A PROPOSED `-fno-rerun-cse-after-loop` RULE, WHICH IS AN OWNER
 * DECISION AND IS NOT IN THE MAKEFILE.  Do not read 4 as this park's number: objcmp and
 * parkcheck take the flag group from the Makefile, so they measure 61, and that is what this
 * header claims.  The agent built a scratch `objcmp_flag.py` to measure the flagged variant.
 *
 * The case for the rule: the residue is a qty_compare tie on movmem8b's two address operands at
 * one of four block moves, and it is a SIDE EFFECT of the fix for the other three -- without the
 * flag the ROM's register assignment falls out free but a guard/set CSE costs an instruction;
 * with it the CSE goes and the operand pair swaps.  Nine source spellings all measured 4.  The
 * precedent is strong: an explicit rule already exists for this same overlay and stem prefix
 * (ovl_f84_a_c_c_c_c_a_c.o, OvlFunc_924_20094cc).
 *
 * (Claim line first on purpose: parkcheck reads the FIRST `N encodings of M`.)
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ac2d8/20099b8.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_c_a_c.s --func OvlFunc_924_20099b8
 *   [asm/overlays/rom_7ac2d8/ovl_f84_c_a_c.s -- the file's ONLY function,
 *   confirmed with `grep -ci func_start` = 1.  222 instructions.  datacheck
 *   reports no data section, so NO SPLIT; the file defines and reads no `.L`
 *   data label at all.]
 *
 * VERDICT, with the proposed CSE_CFLAGS (see below):
 *   objcmp  -- 4 of 229 encodings differ, ours 229, SIZE EQUAL, RELOCATIONS
 *              IDENTICAL (39 of 39, offset, type and symbol, in order)
 *   --align -- 4 instruction(s) in disagreeing regions, of 230
 * Both numbers are a TRUE DISTANCE: count and size both match, so objcmp's 4
 * and --align's 4 are the same four encodings and nothing is positional.
 *
 * Without the flag: objcmp 61 of 229 (RELOCATIONS differ -- a 2-byte shift from
 * 0x17e on, one extra instruction), --align 5 of 230.
 *
 * Verify with (the flag has no Makefile rule yet, so objcmp must be forced):
 *   python3 scratch_elev/b294/B/objcmp_flag.py src/non_matching/ovl_7ac2d8/20099b8.c \
 *       asm/overlays/rom_7ac2d8/ovl_f84_c_a_c.s no-rerun-cse
 *   python3 tools/tryc.py src/non_matching/ovl_7ac2d8/20099b8.c \
 *       --ref asm/overlays/rom_7ac2d8/ovl_f84_c_a_c.s --align --no-rerun-cse
 *
 * THE BLOCKER: local-alloc's qty_compare TIE ON movmem8b's TWO ADDRESS
 * OPERANDS, AT ONE OF FOUR BLOCK MOVES.
 *
 * The ROM's third `OvlFunc_924_20088ec(s)` site is
 *     mov r3, sp / add r2, sp, #0x18 / ldmia r2!, {r0,r1} / stmia r3!, {r0,r1}
 * and ours is the same four with r2 and r3 exchanged.  The other THREE sites
 * agree exactly (out=r2, in=r3).  `thumb_expand_movstrqi` (arm.c:9478) creates
 * the destination address pseudo first and the source second; both are
 * block-local, both `+&l`, and local-alloc orders them by decreasing life
 * (local-alloc.c:1351).  Which of the two wins the first REG_ALLOC_ORDER slot
 * (r3) is decided by a qsort over five or six equal-priority quantities in that
 * one basic block, and the 0x307 pool pseudo's POSITION inside the block is what
 * moves it: WITHOUT the flag that pseudo is born above the block move and the
 * ROM's out=r3 falls out for free; WITH the flag it is born after the `bl` and
 * out takes r2.  So the residue is a side effect OF THE FIX for the other three
 * instructions, and the two cannot both be had -- which is why this is parked
 * rather than pinned.
 *
 * NINE source spellings were measured on top of the flag and ALL are 4:
 * a local for the shifted test value; a local for `s.f10` kept live across the
 * block move; the GetFlag guard as a goto chain; separate `if`s instead of
 * `else if` (31, much worse); a `switch` on the shifted value (16, worse);
 * naming nothing.  The out/in pair never moved.
 *
 * THE FLAG IS THIS TREE'S DOCUMENTED RULE FOR THIS SHAPE, NOT A NEW REQUEST.
 * `__GetFlag(0x307)` dominates `__SetFlag(0x307)`, and at -O2 the second CSE
 * pass hoists the id into callee-saved r5 -- `ldr r5,=0x307` once plus two
 * `mov r0,r5`, where the ROM rebuilds `ldr r0,=0x307` at both sites.  That is
 * verbatim the Makefile's CSE_CFLAGS class ("flag id X tested before the guard
 * branch and set after it -- the guard/set shape, first use dominating"), which
 * already has four rules, ONE OF THEM IN THIS OVERLAY AND THIS STEM PREFIX:
 * `asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_a_c.o` (OvlFunc_924_20094cc, flag
 * id 0x256).  That rule is EXPLICIT, not a pattern, so this stem needs its own.
 * Cross-checks run here: `-fno-gcse` does not undo it (the Makefile says so and
 * it is true), and neither does `-fno-cse-follow-jumps`, `-fno-cse-skip-blocks`
 * or `-fno-thread-jumps`.  A curiosity worth recording: `-fno-gcse` TOGETHER
 * with `-fno-rerun-cse-after-loop` brings the hoist BACK, so the two flags are
 * not independent.
 *
 * ------------------------------------------------------------------- NEW ----
 * A THUMB BLOCK MOVE FEEDING AN OUTGOING ARGUMENT AREA MEANS **PARTIAL**
 * AGGREGATE PASSING, AND MOVE_RATIO IS 2, NOT 15.
 *
 * `ldmia r3!, {r0,r1} / stmia r2!, {r0,r1}` into sp+0 is arm.md's `movmem8b`.
 * Two readings were measured and only the second is right:
 *   (a) an 8-BYTE struct as the 5th argument -- WRONG.  A two-word struct of
 *       ints gets TYPE_MODE DImode from compute_record_mode, never reaches
 *       emit_block_move at all, and comes out as two `ldr`/`str` pairs
 *       (measured: 33 aligned).  Only an 8-byte aggregate whose ALIGNMENT is
 *       below 4 stays BLKmode -- `struct { char b[8]; }` does emit the ldmia.
 *   (b) the WHOLE 24-BYTE struct by value, `partial = 4`.  r0-r3 take its first
 *       four words and `emit_push_insn` block-moves the remaining 8 from
 *       `&s + 0x10` -- which is exactly the ROM's `add r3, sp, #0x18`.  A
 *       24-byte record has no integer mode (TImode is past MAX_FIXED_MODE_SIZE)
 *       so it is BLKmode by construction.
 * And the reason move_by_pieces does not win: expr.c:195 makes **MOVE_RATIO 2**
 * whenever `HAVE_movstrqi` is defined, which the ARM port does define -- so any
 * 8-byte block move goes through movstrqi, not pieces.  The commonly quoted
 * `MOVE_RATIO 15` is the #else branch and does not apply to this target.
 * Worth 46 -> 33 aligned, and it is the difference between a readable landing
 * and an unreachable one.  The sibling
 * src/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_c_b.c (OvlFunc_927_20099b8, the
 * same function in another bank) already carries reading (b); reading it first
 * would have saved the whole detour.
 *
 * ------------------------------------------------------------------- NEW ----
 * `mov r1, #8` IN A STRUCT-BY-VALUE ARGUMENT FILL IS THE GUARD'S OWN
 * COMPARISON, NOT A SEPARATE PARAMETER.
 *
 * Three of the four call sites load r1 with a literal (8, 0xa, 0xa) where the
 * fourth loads `ldr r1, [r6, #4]`.  That reads as two different signatures and
 * it is one: inside `if (s.f04 == 8)` cse's record_jump_equiv has `s.f04`
 * equivalent to 8, so the outgoing word for that field is materialised as the
 * constant, while the site reached by the `bne` keeps the load.  Writing
 * `OvlFunc_924_20088ec(s)` at all four sites reproduces all four fills with
 * nothing spelled.  The guard-propagation tell is a CONSTANT sitting in the
 * middle of an otherwise contiguous run of field loads.
 *
 * SHIMS: NONE.  Zero `register ... __asm__` declarations and zero
 * `__asm__(".equ ...")` lines (both classes checked separately, on the code with
 * this comment stripped).
 */
struct Ctx {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    void (*fn)(void);
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern int __GetFlag(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);

extern int OvlFunc_924_2008758(struct Ctx *s);
extern void OvlFunc_924_20088ec(struct Ctx s);
extern void OvlFunc_924_200b860(void);
extern void OvlFunc_924_200b948(void);
extern void OvlFunc_924_20098f8(void);
extern void OvlFunc_924_20097a8(int n);
extern void OvlFunc_924_20096c4(int n);

void OvlFunc_924_20099b8(void)
{
    struct Ctx s;

    __CutsceneStart();
    if (OvlFunc_924_2008758(&s)) {
        if (s.f04 == 8) {
            if ((s.f08 >> 20) == 0xb) {
                OvlFunc_924_20088ec(s);
                __CutsceneWait(0x1e);
                __PlaySound(0xd3);
                OvlFunc_924_200b860();
                __CopyMapTiles(0x4c, 0x3c, 0x4a, 0x26, 3, 1);
                __CopyMapTiles(0x4d, 0x3c, 0x4c, 0x26, 2, 1);
                __CopyMapTiles(0x4b, 0x3a, 0x56, 0x29, 1, 3);
                __CopyMapTiles(0x4b, 0x3b, 0x56, 0x2b, 1, 2);
                __CopyMapTiles(0x4c, 0x3b, 0x50, 0x31, 2, 1);
                __CopyMapTiles(0x4d, 0x3b, 0x52, 0x31, 2, 1);
                __SetFlag(0x302);
            } else {
                s.fn = OvlFunc_924_200b948;
                __CopyMapTiles(0x4b, 0x39, 0x56, 0x29, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x56, 0x2a, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x56, 0x2b, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x56, 0x2c, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x50, 0x31, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x51, 0x31, 1, 1);
                __CopyMapTiles(0x47, 0x3b, 0x52, 0x31, 1, 1);
                __CopyMapTiles(0x4e, 0x3a, 0x53, 0x31, 1, 1);
                OvlFunc_924_20088ec(s);
                __ClearFlag(0x302);
            }
        } else if (s.f04 == 0xa) {
            if ((s.f10 >> 20) == 0x28) {
                OvlFunc_924_20088ec(s);
                if (__GetFlag(0x307) == 0) {
                    __Func_80933d4(0xc0 << 9, 0xc0 << 6);
                    __Func_80933f8(0x2ca0000, -1, 0x94 << 18, 1);
                    __Func_8093530();
                    __SetFlag(0x307);
                    OvlFunc_924_20097a8(5);
                    __CutsceneWait(0x32);
                } else {
                    OvlFunc_924_20097a8(5);
                }
                __SetFlag(0x306);
            } else if ((s.f10 >> 20) == 0x2a) {
                s.fn = OvlFunc_924_20098f8;
                OvlFunc_924_20088ec(s);
                OvlFunc_924_20096c4(5);
                __ClearFlag(0x306);
            }
        }
    }
    __CutsceneEnd();
}
