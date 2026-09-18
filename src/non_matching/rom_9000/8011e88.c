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

/* ==================== BATCH 271 -- 4 DIFFERING TO 2, AND THE TIE THEORY IS ALSO DEAD ====================
 *
 * The four register-swapped lines this park is named for are now EXACT
 * (`lsl r2, r3, #0x13 / mov r3, r1 / sub r2, r4 / sub r3, #0x8`). Best candidate
 * scratch_elev/b271/regalloc/c7.c, 39 lines against 39, 2 encodings differing,
 * NO PINS.
 *
 * THE CONTEST WAS NEVER BETWEEN TWO 2-REFERENCE QUANTITIES. The batch-267 reading
 * above has `t - 8` and `c - b` tying on priority so that qty NUMBER decides. Read
 * out of .17.lreg / .18.greg, that is not what happens. local-alloc sorts by
 *
 *     QTY_CMP_PRI = floor_log2(n_refs) * n_refs * size / (death - birth)
 *
 * and `*thumb_ashlsi3` lets combine_regs tie the shift result into the dying
 * `ldrsb` temp, after which the `minus` ties into THAT -- making one
 * SIX-REFERENCE quantity (temp + c + c-b) with priority ~1.5. No 2-reference
 * quantity in a five-insn block can beat it. It takes r3 first and `t - 8` gets
 * what is left.
 *
 * Which is why all five spellings above measured 4: they moved the INSNS and never
 * touched a REFERENCE COUNT. That is the general lesson -- when the priority
 * formula is the thing deciding, statement order is the wrong knob.
 *
 * THE CURE IS TO STOP THE TIE by making the shift's destination a GLOBAL pseudo
 * (combine_regs returns 0 when reg_qty[sreg] == -1): reuse the existing global `a`
 * for the third sample, and write the subtraction as the accumulator `a -= b` so
 * the destructive two-operand `sub r2, r4` survives. Block 4 then holds only two
 * tiny locals, which land on r3 in sequence exactly as the ROM does.
 *
 * WHAT BLOCKS THE LAST TWO, and it is a hard stop rather than an unswept space:
 *
 *     rom   mov r0, r3 / mul r0, r2
 *     ours  mov r0, r2 / mul r0, r3
 *
 * The mult's RTL operand 1 is always the SECOND source operand, so `(t-8) * a`
 * gives op1 = `a`, and the only spelling giving op1 = `t-8` is `a * (t-8)`. Both
 * were dumped: the two .17.lreg streams are IDENTICAL except for that operand
 * order. But global.c's set_preference prefers on XEXP(src,0) for
 * `(set prod (mult X Y))`, so with op1 = `a` the mult ties prod to `a` and the
 * globals land ROM-correct (a->r2, b->r4, t->r1); with op1 = the local `t-8` that
 * tie disappears, `a` (priority 0.82) picks before `t` (0.57) and steals r1, `t`
 * needs r4, and a `mov r4, r1` appears. Flipping it would need `a`'s reference
 * count down to 4 or `t`'s priority above 0.82, neither reachable without changing
 * the emitted instructions.
 *
 * MEASURED THIS ROUND (39 lines unless noted): the banked form, 2; the same with
 * `u = t-8; u * a`, 2; `a * (t - 8)`, 36 at 40 lines; `a * u`, 36 at 40 lines;
 * `u` computed before `a -= b`, 35 at 40; `u` before the shift, 26; reuse `a` with
 * `(t-8)*(a-b)`, 13 at 40; a shared multiplier across both arms, 39 at 42 lines.
 *
 * PINS ARE STRICTLY WORSE HERE -- `register int a __asm__("r2")` is 28 at 40
 * lines, and pinning both a->r2 and u->r3 is also 28. Do not spend a fakematch row.
 */
