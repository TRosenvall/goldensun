/* Anim_Attack -- 0x080e3a3c, asm/rom_c9000/rom_e3958_c_c_c_c.s
 *
 * 39 of 39 lines, FIVE differing, and all five are one register rename.
 * Candidate at scratch/Nattack_best.c.
 *
 *      rom   ldr r3, [r5] / mov r1, r3 / sub r1, #0x64 / cmp r1, #0x23 ... cmp r3, #0xc7
 *      ours  ldr r2, [r5] / mov r3, r2 / sub r3, #0x64 / cmp r3, #0x23 ... cmp r2, #0xc7
 *
 * Identical instruction stream.  The ROM holds the loaded value in r3 and its
 * decremented copy in r1; we hold them in r2 and r3.
 *
 * WORTH DISTINGUISHING FROM THE OTHER FOUR.  The coin flip parked on
 * ovl_7ced6c/2009c84.c, .../200a16c.c, .../20096a8.c and ovl_7d0e88/2008f58.c
 * is about CALLEE-SAVED registers -- which long-lived value earns r5 rather than
 * r6.  This one is entirely in the SCRATCH set: after the third galloc call
 * every one of r0-r3 is free, and gcc simply takes r2 where the ROM took r3.
 * No push differs, no value is spilled, and nothing about live ranges is in
 * play.  Whether the two have the same cause in `find_reg` is not established
 * here; recorded separately so nobody assumes it.
 *
 * SCREENED AND INERT, all still 5: the subtraction given its own named local;
 * the field read twice instead of held in a local; and the range test written
 * `(unsigned)(k - 0x64) > 0x23` with the arms swapped, which is WORSE (16) --
 * it reorders the branches as well.
 *
 * The body is otherwise settled: three tagged allocations freed in reverse
 * order, the range test spelled `(unsigned)(k - 0x64) <= 0x23` to give the
 * ROM's `sub / cmp / bhi`, and the second test signed (`ble`) so `k` is a plain
 * int.
 */
