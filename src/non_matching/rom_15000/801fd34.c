/* Func_801fd34 (StepOverlayAnimation) -- NON-MATCHING.
 * Blocker class: EXPRESSION SCHEDULING inside one statement.  12 of 35, same
 * length, and the loop body up to the divide is byte-exact.
 *
 * The function packs three terms derived from one value into a halfword:
 *
 *     *p = ((t + 0x14) << 10) | ((t + 0x10) << 5) | (t * 2 + 0x16);
 *
 * The ROM computes all three BASES first (`lsl r1, r3, #1` for the doubled
 * term, `mov r2, r3` for the middle one, `r3` already holding t), then the
 * three adds, then the two shifts, then the two ORs.  gcc completes each term
 * before starting the next.  Same instructions, different interleave.
 *
 * THREE THINGS WERE SOLVED and each is worth reusing:
 *
 *   1. THE SIGNED DIVIDE IS THE IDIOM, NOT A SHIFT.  `asr r3, r0, #0xe`
 *      preceded by `cmp r0, #0 / bge / add r0, #0x3fff` is gcc's expansion of
 *      `/ 0x4000` on a signed int.  Writing `>> 14` gives the shift without
 *      the bias and never matches; writing the division reproduces all four
 *      instructions.
 *   2. ASSIGNMENT ORDER SETS THE POOL-LOAD ORDER.  `i = 0;` before
 *      `p = (unsigned short *)0x50001d0;` is what puts `ldr r7, =0x3001800`
 *      ahead of `ldr r6, =0x50001d0`, matching the ROM's prologue.
 *   3. NAMING THE THREE TERMS as locals took it from 18 differing to 12 and
 *      made both ORs exact.  The accumulator gcc picks is the FIRST operand of
 *      `a | b | c`, and the ROM's accumulator is the `<< 10` term, so that
 *      term must be written first.
 *
 * Tried for the remaining 12, all worse or equal:
 *   - operands reversed, `(t*2+0x16) | ((t+0x10)<<5) | ((t+0x14)<<10)`: 14
 *   - explicit right grouping `a | (b | c)`: 20, and a line longer
 *   - a `for` loop instead of `do/while`: 12, byte-identical to the form below
 *
 * What remains needs gcc to start the second and third terms before finishing
 * the first, which is scheduling within a single expression and has no source
 * form.  The loop structure, the divide, the compare and both ORs are right.
 */
extern int iwram_3001800;
extern int sin(int a);

void Func_801fd34(void)
{
    unsigned short *p;
    int i;
    int t, a, b, c;

    i = 0;
    p = (unsigned short *)0x50001d0;
    do {
        t = sin((iwram_3001800 + i * 8) * 3 << 8) / 0x4000;
        a = t * 2 + 0x16;
        b = t + 0x10;
        c = t + 0x14;
        *p = (c << 10) | (b << 5) | a;
        p++;
        i++;
    } while (i <= 3);
}
