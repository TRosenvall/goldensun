/*
 * ### BATCH 267 -- THE RESIDUE IS IDENTIFIED DOWN TO THE PSEUDO, AND THE
 * BORN-ORDER THEORY IS DEAD.
 *
 * `.18.greg` reports `;; 6 regs to allocate: 32 47 56 33 35 34`, and neither of
 * the two swapped values is in that list -- both are LOCAL-ALLOC quantities.
 * `.15.regmove` names them:
 *
 *     (set (reg:SI 52) (plus (reg/v:SI 33) (const_int -8)))   t - 8   -> r2
 *     (set (reg:SI 53) (minus (reg/v:SI 36) (reg/v:SI 35)))   c - b   -> r3
 *
 * and the ROM wants exactly the opposite. Both have two refs and near-identical
 * ranges, so `qty_compare_1`'s priority ties and the tie-break is QTY NUMBER --
 * the order `alloc_qty` first meets them scanning the block. Whichever is met
 * first is allocated first and takes r3, REG_ALLOC_ORDER's head.
 *
 * SO THE THEORY WAS: get `t - 8` met first and the pair flips. IT DOES NOT
 * WORK, and five spellings say so, all 4 differing unchanged:
 *
 *     b + ((c - b) * (t - 8)) / 8                    6   (worse)
 *     d = c - b;  b + ((t - 8) * d) / 8              4
 *     d = c - b;  e = t - 8;  b + (e * d) / 8        4
 *     e = t - 8;  c = *p << 19;  b + (e * (c-b)) / 8 4
 *     e = t - 8;  c = *p << 19;  d = c-b; (e * d)    4
 *
 * The RTL confirms the reorder actually happened -- in the third and fourth the
 * `minus` insn really does precede the `plus` -- and the allocation does not
 * move. The load `c = *p << 19` is scheduled ahead of everything in the arm
 * regardless of where the statement sits, because it heads a memory dependence
 * chain, so `c`'s quantity is always met first whatever the source says.
 *
 * WHAT THE ROM'S ARM ACTUALLY LOOKS LIKE, for the next person:
 *
 *     rom    ldrsb r3 / lsl r2, r3, #19 / mov r3, r1 / sub r2, r4 / sub r3, #8
 *     ours   ldrsb r3 / mov r2, r1 / lsl r3, r3, #19 / sub r3,r3,r4 / sub r2,r2,#8
 *
 * Note the ROM shifts INTO r2 while we shift in place -- same swap seen from
 * the other side, not a separate defect.
 *
 * NEXT: this is a local-alloc qty-number tie that source order provably cannot
 * reach, because the scheduler fixes the order before local-alloc runs. Either
 * find something that changes the two quantities' PRIORITY (a third reference
 * to one of them, a different live range), or accept it. Do not spend another
 * round on statement order -- that axis is now exhausted with five measurements.
 */

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
