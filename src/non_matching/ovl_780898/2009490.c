/* OvlFunc_883_2009490 -- asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_a.s
 *
 * BLOCKER: POOL-LOADS-FIRST, single-basic-block variant
 * (the same blocker as src/non_matching/ovl_7cb2c0/200bd10.c, same round)
 *
 * 6 of 70, same length, 64 lines exact.  All six are the SAME callee,
 * __MapActor_Emote, at three sites:
 *
 *     rom  mov r2, #0x14 / mov r0, #0x16 / lsl r1, #0x1
 *     ours mov r2, #0x14 / lsl r1, #0x1  / mov r0, #0x16
 *
 * The ROM emits the cheap `mov r0` BEFORE finishing the shifted or pooled
 * argument; gcc finishes the expensive argument first.  The function has no
 * labels at all, so there is no block that dominates the calls without being
 * their own block, and the basic-block lever is unreachable.
 *
 * MEASURED (all 70 lines, all 6 differing at position 14):
 *   plain literals                                     6
 *   __MapActor_Emote declared `int` (return-type lever) 6
 *
 * The return-type lever is worth noting as a negative here because it closed
 * the byte-for-byte identical shape on OvlFunc_881_200b130 in the same round
 * (2 of 70 -> exact, on __StartTask).  So the lever is real and it is not
 * general: it moved a pool-loaded SYMBOL address past a shift there, and does
 * nothing for a shifted integer argument here.  That distinction is worth
 * keeping -- it matches the round-5 measurement that the symbol tell governs
 * hoisting across a call rather than setup order within one.
 *
 * Best C: scratch/u9490.c.
 */
