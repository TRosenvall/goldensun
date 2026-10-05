/* Func_80a9d84 @ 0x080a9d84 -- MATCHING, 0 differing encodings of 30.
 *
 * ParkElementSprites.  Moves the five state+0xC8 element-icon sprites to
 * (0xF8, 0xA8) with sort order 0xF0 and rewinds each -- the Func_80a9cbc of the
 * element icons.
 *
 * FIGURE (re-derived this batch, batch 328 brief F, never inherited):
 *   OK Func_80a9d84 -- 64 bytes, 30 encodings and 2 relocations identical
 *   SIZE 64/64.  No objcmp SIZE, INSTRUCTION COUNT or RELOCATION line.
 *   --whole: "OK whole file -- 64 bytes, 30 encodings and 2 relocations identical".
 *   PIN COUNT 0.  No inline asm, no device, no fictitious symbol.
 *
 * Verify with -- NAMES THE INSTALLED PATH, ONE LINE:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/rom_a1000/rom_a8604_c_c_a_c_a_c_a_c_c.c asm/rom_a1000/rom_a8604_c_c_a_c_a_c_a_c_c.s --whole
 *
 * SPLIT SHAPE: NONE NEEDED.  asm/rom_a1000/rom_a8604_c_c_a_c_a_c_a_c_c.s holds
 * exactly ONE function (`grep -c thumb_func_start` = 1) and
 * `python3 tools/datacheck.py asm/rom_a1000/rom_a8604_c_c_a_c_a_c_a_c_c.s` is
 * silent (exit 0 -- no data in the .s).  No `.global`, no exports.  It CONVERTS
 * WHOLE to src/rom_a1000/rom_a8604_c_c_a_c_a_c_a_c_c.c.
 *
 * ================================================================
 * THE PARK'S FIGURE WAS RIGHT AND ITS DIAGNOSIS WAS WRONG
 * ================================================================
 * Re-derived the park at 33 of 30: INSTRUCTION COUNT ref 27 / ours 31, SIZE
 * 64 / 72.  We were FOUR ENCODINGS LONG, so the 33 measured misalignment on top
 * of a real +4, exactly as the park's own header warned.
 *
 * DECOMPOSED INTO RUNS, and there were only four, with ONE cause between them:
 *   prologue  +1  ROM `mov r7,r8 / push {r7}`           -- ONE high reg saved
 *                 ours `mov r7,sl / mov r6,r8 / push {r6,r7}` -- TWO (r8 + r10)
 *   setup     +2  ours adds `mov r8,r3` (the second high move) and
 *                 `movs r7,#0xf0` (the third constant, hoisted out of the loop)
 *   loop body  0  SIX ITEMS ON BOTH SIDES.  The loop was never the problem.
 *   epilogue  +1  ours `pop {r3,r5} / mov r8,r3 / mov sl,r5`
 *                 ROM  `pop {r3} / mov r8,r3`
 * So it is ONE cause with three consequences, not three or four causes.
 *
 * THE PARK SAID "gcc hoists ALL THREE" CONSTANTS.  IT DOES NOT, AND THE PARK'S
 * OWN BODY IS THE PROOF: it writes `a = 0xf8;` and `b = 0xa8;` OUTSIDE the loop,
 * in the source.  Only `c = 0xf0;` was ever inside it.  There was one hoist.
 * Everything the park then concluded from "all three" -- that the ROM's compiler
 * "ran out of cheap callee-saved registers", that this is the same shape as
 * ovl_7bf5a8/2008704.c and rom_b5000/80bf54c.c, that "what registers get used is
 * a consequence of pressure in the original, not of how the C is written" --
 * rests on that miscount.
 *
 * ================================================================
 * THE MECHANISM, READ IN THE COMPILER
 * ================================================================
 * `move_movables` gates every hoist on ONE inequality, loop.c:1803:
 *
 *     if (already_moved[regno]
 *         || flag_move_all_movables
 *         || (threshold * savings * m->lifetime) >=
 *            (moved_once[regno] ? insn_count * 2 : insn_count)
 *         || ...)
 *
 * with `threshold = (loop_info->has_call ? 1 : 2) * (1 + n_non_fixed_regs)` at
 * loop.c:651.  Three of those four terms are fixed for us: this loop HAS a call,
 * `n_non_fixed_regs` is a target constant, `insn_count` is the ROM's own loop
 * length and `savings` is 1 for a constant load.  So the only reachable quantity
 * is whether the set is a MOVABLE AT ALL -- and a pseudo set more than once
 * inside the loop is not loop-invariant and never becomes one.
 *
 * THE FIX IS NOT TO SPELL THE CONSTANT DIFFERENTLY.  IT IS NOT TO GIVE IT A
 * NAME OF ITS OWN.  Reuse `t`, the local that already carries the 0xf8 copy:
 *
 *     t = a;                              -> mov  r3, r8
 *     *(unsigned short *)(x + 6) = t;     -> strh r3, [r0, #6]
 *     t = 0xf0;                           -> movs r3, #0xf0     <-- same r3
 *     *(unsigned short *)(x + 8) = b;     -> strh r7, [r0, #8]
 *     *(x + 0xf) = t;                     -> strb r3, [r0, #0xf]
 *
 * `t` now has TWO sets inside the loop, so it is not a movable, so 0xf0 stays
 * where the source puts it -- and r3 is reused exactly as the ROM reuses it.
 * Only 0xf8 and 0xa8 then need to survive the loop, which fits r7 plus a single
 * r8, which is the ROM's prologue.  The ROM's `mov r3,r8 / movs r3,#0xf0` pair
 * was always telling us the two values SHARE A REGISTER; the park read it as two
 * independent constants and gave them two independent names.
 *
 * GENERAL FORM, for the next reader: when a residue is an EXTRA CALLEE-SAVED
 * REGISTER rather than an extra computation, count how many of the loop's
 * invariants the SOURCE declared outside the loop, and ask which of the
 * remaining ones the ROM reuses a register for.  A reused register in the
 * reference is a reused LOCAL in the source.
 *
 * ================================================================
 * WHAT THE LANDED MODULE-MATES CONTRIBUTED
 * ================================================================
 * `src/rom_a1000/rom_a8604_c_c_a_c_a_b.c` (Func_80a9cbc) is this function's
 * structural twin -- the same walk over the same kind of slot table, matching --
 * and every lever in this body except the `t` reuse comes from it verbatim: the
 * DERIVED pointer initialiser `q = (unsigned char **)(p + 0xc8)` in a single
 * statement (a split `q = p; q += 0xc8;` coalesces q with p and drops the ROM's
 * `mov r5,r3`), the constant assigned BEFORE the pointer to match statement
 * order, `t = a;` before the first store to force `mov r3,r8`, and the
 * `i = N; do { ... i--; } while (i >= 0);` counter shape.  The twin's header
 * says "THE ONE THING THAT MATTERS HERE is that the walking pointer is
 * initialised from a DERIVED expression" and that is still true here.
 *
 * The twin differs from this function in exactly one way -- it has no third
 * constant -- so the twin could not have taught the `t` reuse.  The twin told me
 * which 26 of the 30 encodings were already solved, which is what made the
 * remaining four legible as a single cause.
 *
 * MEASURED THIS BATCH:
 *   park as installed (separate local `c = 0xf0`) .... 33 of 30, 31 insns, SIZE 72
 *   `t` reused for the third constant ................ 0  *** MATCH ***
 *   Inherited from the park and NOT re-run (all recorded there as byte-identical
 *   to each other, and all varying only the SPELLING of the constant): the
 *   literal written directly at the store `*(x + 0xf) = 0xf0;`;
 *   -fno-rerun-cse-after-loop; -fno-strength-reduce; -fno-thread-jumps.
 */
extern unsigned char *iwram_3001f2c;
extern void Func_80a17c4(void *x);

void Func_80a9d84(void)
{
    unsigned char *p;
    unsigned char **q;
    unsigned char *x;
    int a;
    int b;
    int i;
    int t;

    p = iwram_3001f2c;
    a = 0xf8;
    q = (unsigned char **)(p + 0xc8);
    b = 0xa8;
    i = 4;
    do {
        x = *q++;
        if (x != 0) {
            t = a;
            *(unsigned short *)(x + 6) = t;
            t = 0xf0;
            *(unsigned short *)(x + 8) = b;
            *(x + 0xf) = t;
            Func_80a17c4(x);
        }
        i--;
    } while (i >= 0);
}
