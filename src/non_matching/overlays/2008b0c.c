/* OvlFunc_939_2008b0c -- 0x02008b0c,
 * asm/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_b_a.s
 *
 * 32 of 32 lines, TWO differing.  Candidate at scratch/L8b0c.c.
 *
 *      rom   mov r1,#0x81 / mov r2,#0x64 / mov r0,#0x0 / lsl r1,#1
 *      ours  mov r1,#0x81 / mov r2,#0x64 / lsl r1,#1   / mov r0,#0x0
 *
 * The interleave class, at a site with a split build and NO conditional branch
 * anywhere in the function.  Textbook instance of the bound recorded in
 * docs/elevation.md: the lever needs a split build AND a preceding branch.
 * Naming the shifted constant makes gcc keep it in a callee-saved register and
 * adds a push (34 lines, 30 differing) -- worse, as the rule predicts.
 */
