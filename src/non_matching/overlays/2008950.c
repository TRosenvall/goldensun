/* OvlFunc_943_2008950 -- 0x02008950, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_a.s
 *
 * 65 vs 66 lines, 35 differing.  Candidate at scratch/L8950.c.
 * A flag dispatcher returning one of five script pointers.
 *
 * BLOCKER: gcc DERIVES a second table offset where the ROM builds both fresh.
 *      rom   mov r1,#0xa7 / lsl r1,#1 ... mov r3,#0xd7 / lsl r3,#1
 *      ours  mov r2,#0xa7 / lsl r2,#1 ... add r2, #0x60
 * 0xd7<<1 minus 0xa7<<1 is 0x60, and with the first value live in a register
 * gcc reaches the second with an `add`.
 *
 * This is the exact inverse of the case the symbol-base lever solves
 * (src/non_matching/overlays/20085a4.c, where the ROM derives and gcc will not).
 * Here gcc derives and the ROM does not, and there is no lever pointing that
 * way: naming the three offsets as separate locals in the dominating block
 * changes nothing, and neither does CSE_CFLAGS -- so the derivation is the main
 * -O2 CSE, not the rerun pass.
 *
 * Consistent with the Func_80a22f4 finding that the derived-constant machinery
 * has no per-operand granularity: it fires on whatever is in a register, and
 * the source cannot ask for it selectively in either direction.
 *
 * Worth keeping from the attempt: `v = __GetFlag(0x928); if (v) return ...;`
 * and then storing `v` later is right -- the ROM stores r5, the result register
 * of that call, as a byte it knows is zero on that path.
 */
