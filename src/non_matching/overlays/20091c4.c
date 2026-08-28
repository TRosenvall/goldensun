/* OvlFunc_953_20091c4 -- 0x020091c4,
 * asm/overlays/rom_7d95dc/ovl_30_c_c_c_a_a_c_a_a.s
 *
 * 75 of 75 lines, 10 differing, all in one block.  Candidate at
 * scratch/L91c4.c.
 *
 * SOLVED: the guarded interleaves (the 0xc0<<6 argument and the four-argument
 * __Func_80933f8 with its mov+neg), storing the GetFlag result as the byte at
 * actor+0x55 (the ROM stores r5, which is provably zero on that path), and BOTH
 * halves of the offset chain -- `off += 0x40` making 0x200 the stored value and
 * then `off = 0x20` reusing the offset variable as the second stored value,
 * which is the lever recorded for OvlFunc_904_2008054.
 *
 * BLOCKER: base and offset occupy each other's registers.
 *      rom   ldr r1,[r3] / mov r3,#0xe0 / lsl r3,#1 / add r2,r1,r3
 *      ours  mov r2,#0xe0 / ldr r3,[r3] / lsl r2,#1 / add r1,r3,r2
 * The ROM loads the base first and builds the offset second; ours does the
 * reverse, and that swaps every register in the ten-instruction block.
 *
 * TRIED: three declaration orders (offset before base, offset first overall,
 * pointer first); assigning the offset before the base in statement order; a
 * separate pointer variable for each of the two stores; a second base variable
 * copied before the second store.  All 10.
 *
 * The mechanism is visible and still not reachable.  The ROM derefs the global
 * into a NEW register (`ldr r3, =sym / ldr r1, [r3]`), which frees r3 for the
 * offset; ours reuses the address register for the result (`ldr r3, [r3]`) and
 * the offset is pushed to r2.  Nothing in the source appears to decide whether
 * gcc reuses the address register or takes a fresh one.
 *
 * Identical signature to src/non_matching/overlays/2008054b.c, which is also
 * 21 of 56 on exactly this -- base in r1 and offset in r3 in the ROM, swapped
 * in ours.  Two instances now; if a lever for it is ever found it closes both.
 */
