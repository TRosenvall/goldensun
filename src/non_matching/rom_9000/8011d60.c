/* HeightTile_4 -- asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b.s
 *
 * BLOCKER: one redundant parameter shuffle. 22 of 28, one line long.
 *
 * The simplest of the HeightTile family: two samples, a max, and a three-way
 * selection on the sign of the delta -- zero returns the max, negative the
 * first sample, positive the second. Both loads, the max, the `+0xf` biasing
 * and all three branches reproduce.
 *
 * THE RESIDUE is one extra move:
 *
 *     rom    mov r5, r2 / mov r2, r4        (save `b`, then the max into r2;
 *                                            `a` never moves)
 *     ours   mov r5, r1 / mov r1, r2 / mov r2, r4   (both `a` AND `b` moved)
 *
 * The ROM needs r2 for the max result, so it relocates `b` and leaves `a` in
 * its parameter register. gcc relocates both, which costs the extra line and
 * shifts everything after it.
 *
 * MEASURED, and both are NEGATIVES worth recording because the reasoning
 * looked sound:
 *   delta computed before the max block, so both operands are still in
 *     their parameter registers                   25 lines, 23 differ
 *   delta computed first of all, before the loads 25 lines, 26 differ
 *
 * Both go THREE LINES SHORT rather than one long -- computing the delta early
 * lets gcc fold the two moves away entirely and it stops matching the ROM's
 * shape at instruction 0 instead of 7. Moving a computation earlier to change
 * which register a value occupies makes the register pressure LOWER, not
 * different, so the shuffle disappears instead of being redirected.
 *
 * THE FAMILY IS NOW THREE-FOR-THREE ON REGISTER ASSIGNMENT: HeightTile_A at 4
 * of 39, HeightTile_B at 34 of 37, this at 22 of 28. Every one of them has the
 * interpolation, the signed divisions and the branch structure exact, and every
 * one differs only in which register holds which local. The family's real
 * levers -- int parameters with the guard cast, and the first multiply
 * operand-flipped -- are in src/non_matching/rom_9000/8011e88.c and they all
 * transferred; what does not transfer is the allocation.

 * UPDATE, batch 181 -- RE-MEASURED ALIGNED: THREE of 28, not 22.
 *
 * The "22 of 28" above is a POSITIONAL count on streams of different lengths,
 * which reports every instruction after the first insertion as differing.
 * tryc.py --align gives 3, and the residue is exactly two adjacent moves plus
 * the operand order they force on the subtraction:
 *
 *     rom    mov r5, r2                    ... sub r3, r5, r1
 *     ours   mov r5, r1 / mov r1, r2       ... sub r3, r1, r5
 *
 * WHY THE ROM ONLY MOVES ONE. The max block writes `m` into r2, so whichever
 * parameter sits in r2 -- `b` -- must be relocated, and `a` in r1 is untouched
 * by that block and can stay. Both versions put `m` in r2. The ROM relocates
 * only `b`; gcc relocates `a` as well, into the callee-saved register it is
 * already pushing for `b`, which costs the extra move and flips the
 * subtraction's operands.
 *
 * `t = b; t -= a;` is the best spelling found and is what is kept below.
 * `u = (b - a) + 0xf;` as one expression is byte-identical to it. Splitting the
 * bias into its own statement instead (`u = t; u += 0xf;`) is WORSE -- 25 lines
 * and 8 aligned, three short, the same collapse the two negatives recorded
 * above produce.
 *
 * So the family's verdict is unchanged but its distance is not: this is three
 * instructions from a match, not twenty-two, and the whole of it is one
 * allocator decision about a parameter that never needed to move.
 */
int HeightTile_4(signed char *p, int a, int b)
{
    int A;
    int B;
    int m;
    int t;
    int u;

    A = p[0] << 19;
    B = p[1] << 19;
    m = A;
    if (B > A)
        m = B;
    t = b;
    t -= a;
    u = t + 0xf;
    if (u == 0xf)
        return m;
    if ((unsigned int)u <= 0xe)
        return A;
    return B;
}
