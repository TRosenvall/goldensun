/* Func_80a22f4 (SyncObjPaletteToBg) -- 0x080a22f4,
 * asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_c_c.s
 *
 * 12 vs 13 lines, 13 differing.  Candidate at scratch/La22f4.c.
 * Two DMA3_SET calls from include/dma.h, which is the right construct -- both
 * `stmia r3!, {r0, r1, r2} / sub r3, #0xc` pairs come out exactly.
 *
 * BLOCKER, and it is the INVERSE of the usual constant problem.  The ROM
 * derives ONE of the second transfer's three operands and pool-loads the other
 * two:
 *
 *      rom   add r1, #0x1c / ldr r0, =0x50001e8 / ldr r2, =0x80000001
 *      ours  sub r0, #0x18 / add r1, #0x1c      / sub r2, #0xf
 *
 * Writing the destination as a mutated variable gets `add r1, #0x1c` correctly
 * -- the batch-123 derived-constant rule working as documented, since the first
 * DMA3_SET pins the value into r1.  But the same rule then fires on the source
 * and the count, because the first call pinned those into r0 and r2 as well,
 * and CSE relates 0x5000200 to 0x50001e8 and 0x80000010 to 0x80000001.
 *
 * There is no way to ask for the derivation on one register-pinned operand and
 * not the others: they are all live in registers for the same reason, and no
 * flag separates them (--no-rerun-cse, -O1, --no-sched2 all leave 13).
 *
 * That is a sharper statement of the derived-constant class than the earlier
 * parks could make: the rule has no per-operand granularity.
 */
