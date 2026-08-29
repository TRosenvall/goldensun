/* OvlFunc_881_20097a4 -- 0x020097a4,
 * asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a.s
 *
 * 37 vs 39 lines, 24 differing (with --no-rerun-cse; 25 without).
 * Candidate at scratch/L97a4.c.
 *
 * BLOCKER: ours pushes r6 the ROM does not.  The value 0xa0 << 12 is built
 * TWICE in the ROM -- once for the `cmp` before the branch and once for the
 * store inside it -- and gcc commons the two into a callee-saved register held
 * across the intervening __GetFlag call.
 *
 * This is the commoned-constant tell, but NEITHER remedy reaches it:
 *   - `-fno-rerun-cse-after-loop` takes 25 to 24 and leaves the push.
 *   - Two separately named locals (which is already how it is written) do not
 *     stop the commoning.
 *   - Assigning BOTH in the dominating block above the branch is worse (28).
 *
 * That is a third case for the tell and the first where both known remedies
 * fail.  What distinguishes it from the five that CSE_CFLAGS fixes: there the
 * repeated value is a flag ID consumed by two CALLS, here it is a comparison
 * operand and a stored value, so the commoning survives the pass that
 * -fno-rerun-cse-after-loop disables.
 */
