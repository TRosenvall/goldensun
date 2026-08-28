/* OvlFunc_931_2008d08 -- 0x02008d08,
 * asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_c.s
 *
 * 34 of 34 lines, SEVEN differing.  Candidate at scratch/L8d08.c.
 *
 * SOLVED: the guarded interleave at __CreateActor
 * (`mov r1,#0x80 / mov r3,#0xc8 / mov r0,#0xde / lsl r1,#15 / mov r2,#0 /
 * lsl r3,#17`) reproduces from two named locals in the dominating block, and
 * the `and r6, r3` with the value as destination from `v &= 3;`.
 *
 * BLOCKER: the pointer and the stored constant occupy each other's registers.
 *      rom   mov r3, r5 / mov r2, #0x14 / add r3, #0x64 / strh r2, [r3]
 *      ours  mov r2, r5 / mov r3, #0x14 / add r2, #0x64 / strh r3, [r2]
 * and that decides the rest of the block, including whether the +0x68 word
 * store is scheduled before or after the second halfword store.
 *
 * TRIED: declaring the constant before the pointer; assigning it before the
 * pointer; writing the two halfword stores with explicit offsets instead of a
 * mutated pointer; casting the actor to `short *` and indexing by 0x32.
 * All 7.
 *
 * Same wall as src/non_matching/overlays/200807c.c and 20094ac.c -- naming the
 * stored constant moves the allocation but does not choose it.
 */
