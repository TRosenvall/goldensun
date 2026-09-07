/* OvlFunc_968_200c2bc -- 0x0200c2bc,
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.s
 *
 * 110 differing of 271, AT THE ROM'S EXACT ENCODING COUNT (271 against 271).
 *      first at index 34: ref 469a  ours 4699
 *      relocations differ -- offsets only, the shift from the encodings above
 * Best candidate: scratch_elev/b249/c2bc/e2bc_c.c. The directory holds 75
 * screened variants plus ref_*.s for all three functions and o.sh / t.sh /
 * batch.sh, which rebuild any measurement.
 *
 * THE FLOOR MOVED: the previous round left this at 265 lines against 275 and
 * 205 DIFFERING. This is 110 at exact length. The next attempt should start
 * from e2bc_c.c, NOT from scratch and NOT from the 205 candidate. The 75
 * variants also rule out the whole statement-order family: the p_a_* and p_b_*
 * sweeps are 24 orderings each of two blocks and every one lands at 205-206,
 * so ORDERING IS NOT THE REMAINING LEVER.
 *
 * WHAT IS KNOWN TO SEPARATE IT FROM ITS SOLVED SIBLINGS (the family in
 * src/overlays/rom_7f2f14/ovl_30_c_c_c_c_a.c, whose header documents the
 * levers that DID work for 200c610/200c7c0/200c968):
 *
 *   1. The inner loop is a ROTATED `while (i <= 7 && k <= 3)` whose base/z
 *      initialisers gcc must sink BELOW the guard.
 *   2. The SECOND outer loop copies the struct pointer into a SECOND high
 *      register (`mov r10, r9`) and stores fields THROUGH IT, while the first
 *      loop stores sp-relative -- the same address in two roles in one
 *      function.
 *
 * LIVE HYPOTHESIS, EXPLICITLY UNTESTED. The last screening run ended on the
 * idea that `base` and `z` ARE NOT SOURCE STATEMENTS AT ALL -- that they are a
 * GIV and a hoisted loop invariant which loop.c manufactures in the preheader,
 * below the guard, from an expression written inside the loop body. If that is
 * right, every candidate so far has been wrong in kind rather than in detail,
 * because all 75 write them as statements and then try to place them. The test
 * is cheap: write the loop with the expression INLINE at its use and let loop.c
 * hoist it, rather than naming base/z anywhere. NOBODY HAS RUN THAT YET.
 *
 * MEASURED WORSE: the split-condition `do` + `break` form is 270 lines, 224
 * differing, with the frame grown to 0x3c by a spill. Do not re-spend it.
 *
 * BLOCKER CLASS as currently understood: loop-form / induction-variable
 * placement, not allocation -- the encoding COUNT is already exact and the
 * relocation list differs only by the offsets the encoding differences produce.
 *
 * LANDING SHAPE IF CLOSED: the .s holds THREE functions -- OvlFunc_968_200c048
 * (~282 insns), this one, and OvlFunc_968_200c520 (~90 insns) -- and closing
 * all three lands src/overlays/rom_7f2f14/ovl_30_c_c_a_c_c.c WHOLE, with no
 * split and no linker edit. 200c520 is the smallest and was never attempted;
 * it is the cheapest way to start the next session on this file.
 * The overlay.ld line MUST KEEP its asm/ path.
 */
