/* HeightTile_B -- asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s
 *
 * BLOCKER: one unreachable register COPY. 34 of 37, two lines short, and the
 * whole count is the cascade from a single `mov r4, r2`.
 *
 * The UNSIGNED twin of HeightTile_A (src/non_matching/rom_9000/8011e88.c):
 * identical structure, `ldrb` instead of `ldrsb`, and the interpolation index
 * arriving in the third parameter rather than the second. The sibling's source
 * ported across unchanged and produced the right shape immediately.
 *
 * THE RESIDUE:
 *
 *     rom    mov r4, r2 / lsl r2, r3, #0x13     (copy the index out, then
 *                                                build the first sample in r2)
 *     ours   lsl r4, r3, #0x13                  (build the sample straight
 *                                                into r4, index stays in r2)
 *
 * Ours is one instruction cheaper and both are correct; the ROM spends a `mov`
 * to free r2 that gcc has no reason to spend.
 *
 * MEASURED: an explicit local copy of the index, `i = t;`, used throughout --
 * BYTE-IDENTICAL at 34. That is the documented boundary again (a copy of an
 * UNCHANGING value is coalesced, so no `mov` is emitted), now on its third
 * function after Func_80a8b10 and Func_80e38b8.
 *
 * Recorded because this is the cheapest possible instance of it: a two-line
 * function-opening difference on a body that is otherwise exact, where the
 * obvious fix is a one-line local and it provably cannot work.
 *
 * FAMILY NOTE. Five HeightTile functions remain; HeightTile_A is parked at 4
 * of 39 and this at 34 of 37, both on register assignment rather than on
 * anything about the interpolation. The levers that matter for the family are
 * in 8011e88.c: int parameters with the guard cast so the compare is `bhi`
 * while the divisions keep their sign correction, and the first multiply
 * written operand-flipped.
 */
int HeightTile_B(unsigned char *p, int unused, int t)
{
    int a;
    int b;
    int c;

    a = *p << 19;
    p++;
    b = *p << 19;
    p++;
    if ((unsigned int)t <= 7)
        return a + ((b - a) * t) / 8;
    c = *p << 19;
    return b + ((t - 8) * (c - b)) / 8;
}
