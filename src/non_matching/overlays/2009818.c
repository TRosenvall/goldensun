/* OvlFunc_927_2009818 -- 0x02009818,
 * asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c.s
 *
 * 36 of 36 lines, THREE differing.  Candidate at scratch/L9818.c.
 *
 *      rom   mov r1,#0xd4 / mov r2,#0xf0 / mov r0,#0x11 / lsl r1,#17 / lsl r2,#17
 *      ours  mov r1,#0xd4 / mov r2,#0xf0 / lsl r1,#17   / lsl r2,#17 / mov r0,#0x11
 *
 * Same bound as OvlFunc_939_2008b0c: two split builds at the site, no branch in
 * the function.  Naming the four shifted constants is dramatically worse (49
 * lines against 36) -- gcc spills them to r5/r6/r8/r10 and the prologue grows
 * from `push {r14}` to a four-register save.
 *
 * That cost is worth noting on its own: in a straight-line function the naming
 * lever does not merely fail to help, it can add half a dozen instructions.
 * Check the branch count BEFORE reaching for it.
 */
