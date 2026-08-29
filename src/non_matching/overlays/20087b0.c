/* OvlFunc_950_20087b0 -- 0x020087b0,
 * asm/overlays/rom_7d5838/ovl_30_c_c_a_c_c_c.s
 *
 * 61 of 61 lines, 3 differing -- one of which is only the symbol spelling, so
 * really 2.  Candidate at scratch/L87b0.c.  Needs CSE_CFLAGS (33 differing
 * without it) and `_MSG_2399` in message.sym.
 *
 * SOLVED: the symbol-base derivation from
 * src/non_matching/overlays/20085a4.c works here too -- `ldr r5, =_MSG_2399`
 * with `add r0, r5, #1` and `add r0, r5, #2`, where a plain int constant gives
 * three independent pool loads.  Second instance of that lever.
 *
 * BLOCKER: argument order at `__Func_8092c40(slot, 0)`.
 *      rom   mov r1, #0x0 / mov r0, r6
 *      ours  mov r0, r6   / mov r1, #0x0
 * One argument is a register copy and the other a constant; neither is a split
 * build.  Same bound as 20082b8.c and 20085a4.c.
 *
 * TRIED: naming the zero as a local immediately before the call; copying the
 * parameter into a local at the top of the function.  Both 3.
 */
