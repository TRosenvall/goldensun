/* Func_80a22f4  [rom_a1000]
 * Source asm: goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a.s
 *
 * Parked: logic faithful, does NOT byte-match.
 *
 * PROGRESS 2026-08-03. The TODO below was right about the idiom, and it is
 * already in the tree: include/dma.h's DMA3_SET takes the control word raw and
 * emits exactly the `stmia r3!, {r0, r1, r2}` / `sub r3, #0xc` pair. Switching
 * from DMA3_COPY16 to two DMA3_SET calls with plain literals takes this from a
 * structural mismatch to TWELVE instructions against the ROM's twelve, with
 * three left over:
 *
 *     rom    add r1, #0x1c / ldr r0, =0x50001e8 / ldr r2, =0x80000001
 *     ours   sub r0, #0x18 / add r1, #0x1c      / sub r2, #0xf
 *
 * Both derive the destination from the previous one. gcc ALSO derives the
 * source and the control word by arithmetic -- 0x50001e8 is 0x5000200 - 0x18,
 * and 0x80000001 is 0x80000010 - 0xf -- where the ROM loads both fresh from
 * the literal pool.
 *
 * That is the same disposition recorded in
 * src/non_matching/overlays/constant_reuse.c and seen three times: gcc as
 * invoked here is more eager to reuse a value than the original build was.
 * Eleven flags affecting CSE and reuse are ruled out in that file, so it is
 * not the invocation.
 *
 * Also tried: the destination cached in a local and offset for the second call
 * (13 instructions -- it spills to r4 and costs a `mov r1, r4`), and
 * DMA3_COPY16 as below, which does not produce the block-store shape at all.
 *
 * NOTE for whoever picks this up: the first instruction is NOT a difference.
 * The ROM writes `ldr r3, =REG_DMA3SAD` and gcc writes `ldr r3, =0x40000d4`;
 * gba.inc defines that name with `.set`, so both assemble to the same word.
 * tools/tryc.py now resolves `.set` constants -- before that fix this function
 * appeared to differ at instruction zero, which sent the first attempt looking
 * in the wrong place entirely.
 *
 * ORIGINAL NOTE, kept:
 *   HAND-ONLY (idiom mismatch, not a schedule the permuter can reach).
 *   Candidate: tools/runs/run_20260606T194103Z/Func_80a22f4-iter-5.c
 *   TODO(residual): the ROM does NOT call a DMA copy helper; it writes the
 *     DMA3 registers inline as two 3-word block stores, reusing dst across
 *     both.
 */
#include "dma.h"

/* Takes no arguments. DMA3-copies OBJ palette bank 0 (0x5000200) down into BG
 * bank 14 (0x50001C0), plus one further colour. Keeps the text drawn into the
 * tilemap the same colours as the sprites drawn over it.
 */
void Func_80a22f4(void)
{
    DMA3_SET((void *)0x5000200, (void *)0x50001c0, 0x80000010);
    DMA3_SET((void *)0x50001e8, (void *)0x50001dc, 0x80000001);
}
