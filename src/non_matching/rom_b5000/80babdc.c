/* Func_80babdc -- asm/rom_b5000/rom_b9b30_c_a.s
 *
 * BLOCKER: FRAME-ADDRESS MATERIALISATION ORDER -- 2 of 57, same length
 *
 * 55 of 57 exact, including the two-iteration do/while, the high-register
 * counter (`mov r3,#1 / neg r3,r3 / add r8,r3` -- Thumb has no `sub` immediate
 * for r8), the carried actor pointer across Func_80b6cd0, and the tail.
 *
 *     rom  mov r3, #0x1 / mov r6, sp  / mov r8, r3
 *     ours mov r3, #0x1 / mov r8, r3  / mov r6, sp
 *
 * `mov r6, sp` is gcc materialising the address of the 4-byte stack buffer,
 * hoisted out of the loop; `mov r8, r3` is `i = 1`.  The ROM materialises the
 * frame address FIRST.
 *
 * MEASURED (all 57 lines, all 2 differing at position 12 unless noted):
 *   short buf[2], address taken implicitly at each use          2
 *   `short *p = buf;` assigned before `i = 1`      23 of 57, ours 59 lines
 *   -fno-schedule-insns                                         2
 *   -fno-schedule-insns2                          13, first diff at 3 (worse)
 *
 * The named-pointer result is the informative one.  Round-6 agent2 recorded
 * that a LICM hoist lands at the END of the preheader, so writing the address
 * as its own statement before the counter SHOULD put it first -- and it does
 * not; it creates a second pseudo and costs two instructions instead.  The
 * frame-address materialisation is not an ordinary loop-invariant expression
 * and does not respond to that lever.
 *
 * -fno-schedule-insns leaving it unchanged says the pre-reload scheduler is not
 * what orders these two.
 *
 * Best C: scratch/Ababdc.c.
 */
