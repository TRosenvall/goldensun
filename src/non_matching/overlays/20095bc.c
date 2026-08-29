/* OvlFunc_939_20095bc -- 0x020095bc,
 * asm/overlays/rom_7c460c/ovl_314_c_a_c_c_c.s
 *
 * 63 vs 62 lines, 19 differing.  Candidate at scratch/L95bc.c.
 * A pure straight-line script: 23 calls, no memory operations, no branches.
 *
 * BLOCKER: gcc commons the shifted constant `0x90 << 1` used at two of the
 * calls into a callee-saved register and adds a push the ROM does not have.
 * The ROM builds it twice, `mov r2,#0x90 / lsl r2,#1` each time.
 *
 * This is the commoned-constant tell and NEITHER remedy applies:
 *   - `-fno-rerun-cse-after-loop` changes nothing, so the commoning is the main
 *     -O2 CSE pass and not the rerun.
 *   - Separate named locals need a DOMINATING BLOCK to be rematerialised from,
 *     and this function has no conditional branch at all.  Writing them as two
 *     named locals leaves the count unchanged.
 *   - The no-prototype lever on the callee changes nothing either.
 *
 * So this is the straight-line case of the commoned-constant class, and it is
 * the same shape as the interleave bound: both levers need a branch, and a
 * function with none is out of reach for both.  Worth stating as one fact --
 * **in a function with no conditional branch, neither the naming lever nor the
 * interleave lever can do anything**, and the only remaining tool is the flag
 * group.
 */
