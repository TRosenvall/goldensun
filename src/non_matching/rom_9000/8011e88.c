/* HeightTile_A -- asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s
 *
 * BLOCKER: two subexpressions swapped between r2 and r3. 4 of 39, LENGTH
 * EXACT, everything else instruction for instruction.
 *
 * Linear interpolation between height samples: two signed bytes shifted left
 * 19, interpolated by the low three bits of t; above 7 it reads a third sample
 * and interpolates on (t - 8).
 *
 * TWO LEVERS LANDED, 27 differing to 4:
 *
 *   1. UNSIGNED COMPARE, SIGNED ARITHMETIC.            27 -> 6, and the
 *      length went from 31 to 39.
 *      The guard is `bhi` so t reads as unsigned -- but declaring the
 *      PARAMETER unsigned makes the products unsigned too, and then `/ 8`
 *      becomes `lsr #3` with no sign correction. That silently deleted BOTH
 *      four-instruction `cmp/bge/add #7` sequences, which is the entire
 *      eight-line shortfall. Declaring `int t` and writing the guard as
 *      `(unsigned int)t <= 7` gives the unsigned compare AND the signed
 *      divisions.
 *
 *      Worth stating as a rule: a single `bhi` does not make the variable
 *      unsigned. Cast the COMPARISON, not the declaration -- the same shape
 *      as the `(int)p >= (int)buf` lever on Func_8029274, in the opposite
 *      direction.
 *
 *   2. MULTIPLY OPERAND ORDER, first site only.        6 -> 4.
 *      `(b - a) * t` produces the ROM's `mov r0, r1 / mul r0, r3`; `t * (b-a)`
 *      produces the reverse. Flipping the SECOND multiply the same way makes
 *      it worse (8), so the two sites want opposite spellings.
 *
 * WHAT REMAINS: in the second arm the ROM computes `c` into r2 and `t` into
 * r3; ours the other way round. Four instructions, all of them that swap.
 *
 * MEASURED AND FOLDED -- naming does not move it:
 *   `d = c - b` named before the multiply          39 lines, 4 differ
 *   `d = c - b` and `e = t - 8` both named         39 lines, 4 differ
 *   `e = t - 8` computed BEFORE `c`                39 lines, 4 differ
 *   `t -= 8` in place                              38 lines, 19 differ
 *
 * The first three are byte-identical. The last is a clean negative: modifying
 * the parameter in place costs a line and eleven differences.
 */
int HeightTile_A(signed char *p, int t)
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
