/* OvlFunc_952_200bd40 -- 0x0200bd40,
 * asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.s
 *
 * 74 of 74 lines, TWO differing.  Candidate at scratch/Lbd40_A.c.
 * Both are the `mov r0, #0` position at the one __MapActor_SetSpeed call.
 *
 * The batch-127 argument-order lever WORKS in this function -- but only at the
 * `neg` sites.  Naming `m = -0x10;` at the top fixed all three
 * __Func_809228c/__Func_8092208 call sites, which sit after the function's
 * three-way branch.  The SetSpeed call does not move, and the reason sharpens
 * the rule:
 *
 *   THE BRANCH MUST BE BEFORE THE SITE.  This function's SetSpeed is the third
 *   instruction group in the function, before any conditional branch at all.
 *   Naming its two shifted constants at the top then makes gcc keep them in
 *   CALLEE-SAVED registers across the two intervening calls -- ours grew to
 *   `push {r5, r6, r7, r14}` against the ROM's `push {r5, r14}` -- which is the
 *   same failure the three straight-line parks show, for the same reason.
 *   Intervening CALLS are not enough; gcc will happily save and restore across
 *   them. It is the branch that makes rematerialising the cheaper choice.
 *
 * MY OWN CLASSIFICATION WAS TOO COARSE.  The batch-127 sizing counted a
 * function as "guarded" if ANY conditional branch preceded ANY site with the
 * shape.  This function has both kinds at once: guarded `neg` sites and an
 * unguarded `lsl` site.  The split is per-SITE, not per-function, and the
 * 150/98 figures should be read as an upper bound on the workable side.
 *
 * ALSO TRIED: naming only one of the two shifted constants (75 lines, worse in
 * both directions); leaving all three inline (11 differing -- the `neg` sites
 * regress, confirming the lever is doing real work there).
 */
