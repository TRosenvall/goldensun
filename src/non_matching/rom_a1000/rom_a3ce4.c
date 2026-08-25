/* Func_80a3ce4 -- NOT MATCHING. 2 of 12 lines, same length.
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c.s
 *
 * Blocker class: SIGNED LOWER-BOUND CANONICALISATION. See the class note below;
 * OvlFunc_899_2008048 in src/non_matching/ovl_794ac0/2008048.c is the same
 * defect to the instruction.
 *
 *     rom    cmp r0, #0xc4 / bgt <no>   <- upper bound, MATCHES
 *            cmp r0, #0xc1 / blt <no>   <- lower bound
 *     ours   cmp r0, #0xc4 / bgt <no>
 *            cmp r0, #0xc0 / ble <no>
 *
 * IMPROVED FROM 11 LINES TO 12 AND FROM A FUSED RANGE TEST TO THIS. The park
 * previously held
 *
 *     if (x >= 0xc1 && x <= 0xc4) return 1;
 *     return 0;
 *
 * which gcc fuses into ONE unsigned comparison, eleven lines against twelve and
 * nothing like the ROM. Splitting the bounds into separate statements with a
 * goto -- the batch-53/55 lever, a branch in the SOURCE stopping a rewrite --
 * gets the length right and leaves only the canonicalisation.
 *
 * THE CANONICALISATION IS ONE-DIRECTIONAL, which is what makes it a class
 * rather than noise. gcc-2.96 leaves an UPPER bound alone: `v > 0xc4` compiles
 * to `cmp #0xc4 / bgt`, exactly as the ROM has it. It rewrites every LOWER
 * bound to `cmp #(K-1) / ble`:
 *
 *     v < 0xc1     ->  cmp #0xc0 / ble
 *     v <= 0xc0    ->  cmp #0xc0 / ble
 *     v >= 0xc1    ->  cmp #0xc0 / ble   (inverted for the else arm)
 *
 * All three spellings, plus declaring the operand `int` rather than `short`,
 * give identical output. The ROM's compiler does not do it.
 *
 * WHAT THIS PREDICTS: any function whose only remaining difference is a lower
 * bound has a 2-line floor and is not worth another round. Check for
 * `cmp #(K-1) / ble` against `cmp #K / blt` before spending one.
 */
int Func_80a3ce4(int v)
{
    if (v > 0xc4)
        goto no;
    if (v < 0xc1)
        goto no;
    return 1;
no:
    return 0;
}
