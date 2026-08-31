/* HeightTile_6 -- asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b.s
 *
 * BLOCKER: REGISTER ASSIGNMENT of two live locals. 20 of 44, LENGTH EXACT ON
 * THE FIRST ATTEMPT.
 *
 * Sibling of HeightTile_A (src/non_matching/rom_9000/8011e88.c): three signed
 * bytes shifted left 19, interpolated on a delta, with an early return when
 * the delta is zero and a second arm above 14.
 *
 * BOTH OF HeightTile_A's LEVERS TRANSFERRED UNCHANGED, which is the useful
 * part -- the first attempt here was written with them already applied and
 * came out at the exact length:
 *
 *   - `int` parameters with the guard written `(unsigned int)u <= 0xe`, so the
 *     compare is `bhi` while the divisions keep their `+0xf / asr #4` sign
 *     correction. Declaring the parameter unsigned would delete both
 *     correction sequences, as it did on HeightTile_A.
 *   - the first multiply written operand-flipped, `(B - A) * u`, which
 *     produces the ROM's `mov r0, r1 / mul r0, r3`; the second left as
 *     written. The two sites want opposite spellings, same as the sibling.
 *
 * WHAT REMAINS is which registers hold the two deltas:
 *
 *     rom    `sub r0, r2, r1` into a FRESH register, then `mov r1, r0` and
 *            `add r1, #0xf` -- t in r0, u in r1, both live
 *     ours   `sub r2, r1` IN PLACE, clobbering the dead parameter, then u in r0
 *
 * Both are correct and ours is one instruction cheaper at that point; the
 * counts match because the difference is paid back elsewhere.
 *
 * MEASURED, both byte-identical to the baseline at 20:
 *   `u = b - a + 0xf` computed first, `t = u - 0xf` derived
 *   `t` computed before the third sample load rather than after
 *
 * Stopped here deliberately. The residue is the register-assignment class,
 * and this batch has five instances of spelling being unable to reach it --
 * the stopping rule in docs/elevation.md says read the ROM's register
 * assignment and stop when it is rotated, rather than spending screens
 * confirming it again.
 */
int HeightTile_6(signed char *p, int a, int b)
{
    int A;
    int B;
    int C;
    int t;
    int u;

    A = *p << 19;
    p++;
    B = *p << 19;
    C = p[1] << 19;
    t = b - a;
    u = t + 0xf;
    if (u == 0xf)
        return B;
    if ((unsigned int)u <= 0xe)
        return A + ((B - A) * u) / 16;
    return B + (t * (C - B)) / 16;
}
