/* OvlFunc_950_20080c0 -- asm/overlays/rom_7d5838/ovl_30_c_c_a_c_a_a_c_a.s
 *
 * BLOCKER: LOAD ORDER INSIDE A TABLE READ
 *
 * 5 of 53 differing.
 *
 * All five are inside the .L1dcc table read.  The ROM keeps the table base in r0
 * and loads the word LAST:
 *   rom  ldrh r1,[r0,r3] / add r3,r0 / ldrh r2,[r3,#2] / ldr r0,[r0,r5]
 * we materialise the address first and load the word early.
 * * MEASURED: all six orders of the three field reads (best z,x,d and z,d,x at 5);
 * four declarations of __Func_8010560 (int(), void(), implicit, full prototype --
 * no effect); three byte-offset pointer-arithmetic spellings (6 and 30, worse);
 * -fno-schedule-insns2 and -O1 both 15.
 */
