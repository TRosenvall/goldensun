/* OvlFunc_951_2008880 -- asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a.s
 *
 * BLOCKER: INSTRUCTION SCHEDULING (two adjacent swaps in one loop)
 *
 * 4 of 47 differing.
 *
 * Two adjacent-instruction swaps in the second loop: `mov r8, r2` vs `mov r6, #7`
 * in the preheader, and `add r5, #2` vs `sub r6, #1` in the body.  Pure
 * scheduling; contents and registers otherwise identical.
 * * MEASURED: *t++ vs *t; t++; vs t = t + 1 vs t[0]; named unsigned short / int temp
 * for the loaded value (the unsigned short temp turns the ldrh into an ldrsh);
 * for / while / do-while / while(--i >= 0); all six orderings of the three
 * preheader assignments; swapping i-- and t++; a named int d = 8 for the
 * __WaitFrames argument; -fno-rerun-cse-after-loop, -fno-schedule-insns,
 * -fno-strength-reduce, -fno-gcse.  Best is 4.
 */
