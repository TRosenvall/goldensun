/* OvlFunc_945_200b51c -- 0x0200b51c, asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_a.s
 *
 * 133 ROM lines against 134 of ours, 85 differing.  Candidate: scratch/Gb51c.c.
 * The five-arm flag chain, both SetBehavior calls and every argument value are
 * right; the whole diff is in one block.
 *
 * BLOCKER: FOUR values held across calls, allocated differently.  The 0x8a0 arm
 * calls OvlFunc_945_200c890 six times with four repeated constants -- 0x1e0,
 * 0x1e8, 0xb000, 0xd000 -- and the ROM keeps them in r10, r5, r8 and r6:
 *
 *      rom   mov r3,#0xb0 / lsl r3,#8 / mov r5,#0xf4 / mov r8,r3 / ... / lsl r5,#1
 *      ours  mov r5,#0xb0 / mov r3,#0xd0 / lsl r5,#8 / lsl r3,#8 / mov r6,#0xf4
 *
 * The ROM also interleaves the partial builds into the argument setup of the
 * call between them, and passes the first call's fourth argument as the LIVE r3
 * rather than copying from r8.
 *
 * ALL SIX ASSIGNMENT ORDERS SCREENED.  The source-order lever picks registers
 * for two independent values; with FOUR it has no purchase.  Every permutation
 * of the three non-first assignments gives 85 or 86 -- the two that give 86 move
 * the first difference one line earlier, so the ordering is doing something, but
 * nothing that converges.
 *
 * That is worth recording as a boundary on the lever rather than another
 * failure: it reaches two values, it does not scale to four.
 *
 * ALSO NOTE the one-line length difference.  The ROM CROSS-JUMPS: the 0x93e arm
 * ends `mov r0, #0xe / b .L360c` and the 0x925 arm falls into the same
 * `.L360c: mov r1,#0 / mov r2,#0 / bl OvlFunc_945_200c8e8`, sharing three
 * instructions between two paths.  Writing the two calls out separately lets gcc
 * share some but not all of it.  Not attacked, because the register allocation
 * above dominates and would have to move first.
 */
