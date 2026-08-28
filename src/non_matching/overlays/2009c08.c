/* OvlFunc_881_2009c08 -- 0x02009c08, asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a.s
 *
 * 49 ROM lines against 52 of ours, 34 differing -- one decision, and the three
 * extra instructions locate it exactly.  Candidate: scratch/L9c08.c.
 *
 * The commoned-constant tell in its textbook form: ours emits
 * `push {r5, r6, r14}` where the ROM pushes only lr, hoists the two pooled flag
 * ids 0x16f and 0x171 into r6 and r5, and then passes `mov r0, r6` / `mov r0, r5`
 * at the four flag calls.  The ROM reloads `ldr r0, =0x16f` at each use.
 *
 * THIS IS THE COUNTEREXAMPLE TO THE TWO-REMEDY RULE.  docs/elevation.md records
 * that this tell is fixed either by CSE_CFLAGS or by separate named locals, and
 * says to try both before concluding anything.  Here BOTH fail, and so does
 * every other flag group in the tree:
 *
 *   CSE_CFLAGS                34   named locals (four, one per use)   34
 *   GCSE                      34   named locals + CSE                 34
 *   ALIAS                     34   named locals + GCSE                34
 *   STRENGTH                  34   SCHED2                             35
 *   FIXEDR7                   34   O1                                 35
 *
 * The docs offered a guess that the flag-group cases are flag ids crossing a
 * BRANCH while the named-local cases are constants reused within one block.
 * This function is the second kind -- br == 0, all four flag calls in a single
 * straight-line block -- and the named-local remedy still does not take, so
 * that guess does not survive as stated.  Recorded rather than patched over.
 */
