/* OvlFunc_931_2008d58 -- 0x02008d58,
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c.s
 *
 * 70 vs 69 lines, 27 differing.  Candidate at scratch/L8d58.c.
 * Straight-line, 23 calls, no branches.
 *
 * BLOCKER: two `-1` arguments at different calls (__Func_80933f8's second and
 * __Func_8091ff0's only) get commoned into r5 with a push, where the ROM builds
 * `mov r1,#1 / neg r1,r1` and `mov r0,#1 / neg r0,r0` separately.
 *
 * Same no-branch case as 200bc48.c, with a mov+neg build instead of a mov+lsl.
 * Two separately named locals do nothing -- there is no dominating block to
 * rematerialise them from -- and CSE_CFLAGS does not fix it either.
 *
 * Worth noting the contrast with OvlFunc_891_200a244, elevated this batch: the
 * SAME `-1` naming works there because the function has a guard, and the lever
 * needs one.  The two functions differ in nothing else that matters.
 */
