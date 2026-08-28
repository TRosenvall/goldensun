/* OvlFunc_945_200bd10 -- asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s
 *
 * BLOCKER: POOL-LOADS-FIRST, single-basic-block variant
 *
 * 13 of 77, same length, and 64 lines are exact.  Every difference is the same
 * decision at four call sites whose arguments include a pooled or shifted
 * constant:
 *
 *     rom  mov r0, #0x8 / ldr r1, =0xcccc / ldr r2, =0x6666
 *     ours ldr r1, =0xcccc / ldr r2, =0x6666 / mov r0, #0x8
 *
 * gcc emits the expensive arguments first and the cheap `mov r0` last; the ROM
 * emits r0 first.  The function is one basic block -- `push {r5, lr}`, no
 * labels anywhere -- so the basic-block lever has nothing to bite on.
 *
 * MEASURED (all 77 lines, all 13 differing at position 11 unless noted):
 *   plain literals                                             13
 *   __MapActor_SetSpeed and __Func_80921c4 declared `int`      13
 *     (the return-type lever, which DID close the identical shape on
 *      OvlFunc_881_200b130's __StartTask one round earlier)
 *   both prototypes removed entirely                           13
 *   every shifted/pooled argument hoisted to an `int` local at
 *     the top of the function                        96 of 77, ours 97 lines
 *
 * That last one is the informative negative.  Round-5 agent1 recorded hoisting
 * the argument constants to top-of-function locals as the fourth answer to
 * this shape, and it is -- but only where the locals live in a block that
 * DOMINATES the calls without being their own block.  Here there is only one
 * block, so hoisting nine constants makes them all simultaneously live, gcc
 * spills, and the function grows by twenty instructions.
 *
 * So the rule wants a precondition attached: the hoist lever needs a real
 * dominating block, exactly like the basic-block lever it is a variant of.
 * In a straight-line function it is actively harmful.
 *
 * Best C: scratch/ubd10.c.
 */
