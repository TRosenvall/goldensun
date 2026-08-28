/* OvlFunc_965_2008eac -- 0x02008eac, asm/overlays/rom_7ef4f4/ovl_30_a_a_c_c_a_c.s
 *
 * 63 ROM lines against 61 of ours.  Body fully decoded and, past the first
 * call, correct -- the 60 differing lines are a two-line shift, not 60 faults.
 *
 * Same constant_reuse blocker as 2009090.c, on a different constant.  The
 * second call takes -1 in three of its four argument registers.  The ROM builds
 * -1 three separate times:
 *
 *     mov r0,#1 / mov r1,#1 / mov r2,#1 / neg r2,r2 / neg r1,r1 / neg r0,r0
 *
 * gcc builds it once and copies:  mov r2,#1 / neg r2,r2 / mov r0,r2 / mov r1,r2.
 * Two instructions shorter, which is the entire length difference.
 *
 * TRIED, all inert: the no-prototype lever on the callee, GCSE, CSE, SCHED2,
 * ALIAS.  As with 2009090 the three values are the same folded constant, so
 * there is no source spelling that separates them.
 *
 * The rest of the function is worth keeping -- the stores through repeated
 * __MapActor_GetActor(0) results, the two-instruction constants 0x82 << 16 and
 * 0x80 << 7, the pooled 0xfffb0000, and OvlFunc_965_2008cd0 taking the actor
 * pointer as its argument all reproduce.  Candidate: scratch/L8eac.c.
 */
