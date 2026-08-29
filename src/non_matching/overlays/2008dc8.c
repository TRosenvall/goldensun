/* OvlFunc_960_2008dc8 -- 0x02008dc8, asm/overlays/rom_7eaf28/ovl_314_c_c_a.s
 *
 * 60 vs 61 lines, 14 differing.  Candidate at scratch/L8dc8.c.
 *
 * SOLVED:
 *   - `_AREA_a5` for the area comparison (the ROM pool-loads 0xa5 where a `cmp`
 *     immediate would do; the symbol was already in area.sym).
 *   - TWO SEPARATE POINTER VARIABLES for the two __MapActor_GetActor results.
 *     Reusing one variable made gcc preserve the returned pointer with
 *     `mov r2, r0 / add r2, #0x23` where the ROM destroys it with
 *     `add r0, #0x23`.  50 differing -> 14 on that change alone.
 *   - Named locals for both stack-argument pairs and for the shifted position
 *     constants.
 *
 * BLOCKER: the ROM spends a CALLEE-SAVED register on a short-lived zero.
 *      rom   push {r5, r14} ... mov r5, #0x0 / ... / strb r5, [r0, #0x0]
 *      ours  push {r14}     ... mov r3, #0x0 / ... / strb r3, [r0, #0x0]
 * r5 is not live across any call there, so nothing requires it to be
 * callee-saved -- gcc simply had r0-r3 committed to the upcoming SetPos
 * arguments and reached past them.  Ours orders the strb and those argument
 * builds differently, leaves r3 free, and never needs the fifth register.
 *
 * The remaining differences are that missing push plus the two stack-argument
 * pairs, where the ROM materialises both values before storing either
 * (`mov r3,#0xf / mov r2,#0x2c / str r3,[sp] / str r2,[sp,#4]`) and ours reuses
 * one register (`str r3,[sp] / mov r3,#0x2c / str r3,[sp,#4]`).  Naming the
 * pairs as locals -- which fixed exactly this on OvlFunc_924_200b6ac -- does not
 * reach it here, because there the pair was the only pressure and here the
 * interleaved strb competes for the same registers.
 *
 * TRIED: hoisting `z = 0;` into the dominating block to make gcc spill it to a
 * callee-saved register (no change, 14); --no-rerun-cse (no change).
 */
