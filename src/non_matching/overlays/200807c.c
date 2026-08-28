/* OvlFunc_931_200807c -- 0x0200807c, asm/overlays/rom_7b8cb0/ovl_30_c_c_a_a_a.s
 *
 * 62 of 62 lines, SIX differing, all in one basic block.
 * Candidate at scratch/L807c.c.
 *
 * SOLVED: `_AREA_4b` and `_AREA_4c` for the two area comparisons (both already
 * in area.sym), the asm-label externs for the three tables, and -- worth 15
 * differing -> 6 -- naming the STORED VALUE as a local (`z = 0; *p = z;`)
 * rather than writing the literal at each store.  Same lever as the mask in
 * OvlFunc_964_20094ac: naming the constant operand shifts which register the
 * pointer gets.
 *
 * BLOCKER: the two pointers into the same table.
 *      rom   ldr r3, =L2 / mov r1, r3 / add r1, #0x8e / add r3, #0xa6
 *      ours  ldr r5, =L2 / mov r3, r5 / add r3, #0x8e / add r5, #0xa6
 * The ROM loads into r3 and copies to r1; ours loads into r5 (callee-saved, and
 * pushed in both) and copies to r3.  Structurally identical, six lines apart.
 *
 * TRIED: fresh variable names per block so the live ranges do not overlap the
 * second branch's use -- WORSE (10 differing), which is the reverse of what the
 * "two call results need two variables" rule would suggest.  That rule is about
 * a value being reassigned across a use; here the variables are already
 * distinct within each block and splitting them only lengthens the live ranges
 * gcc has to colour.
 */
