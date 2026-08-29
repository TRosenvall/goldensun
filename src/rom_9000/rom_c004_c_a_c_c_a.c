/* Func_800c5b4  --  0x0800c5b4, was goldensun/asm/rom_9000/rom_c004_c_a_c_c_a.s.
 *
 * The .s held this function alone, so no split was needed and the linker
 * script's existing line for that object now picks up this file's.
 *
 * Registers two callbacks, starts something, waits a frame, then rewrites the
 * background-mode bits of REG_DISPCNT.
 *
 * FILED UNDER narrow_constant FOR FOUR ROUNDS AND IT WAS NOT THAT. The park
 * measured, carefully and repeatedly, that
 *
 *     rom    ldr r3, =0xf1ff   ...   ldr r2, =0x1000
 *     ours   ldrh r3, .L0      ...   ldrh r2, .L0+4
 *
 * and built a real theory on it: gcc narrows the whole expression to HImode
 * because the destination is a `vu16 *`, so no retyping of the mask reaches
 * it. Eight formulations were tried and measured against that theory.
 *
 * The mode analysis is correct. What is wrong is that it matters. Thumb-1 has
 * no pc-relative `ldrh` -- the only PC-form load is `LDR Rd, [PC, #imm8*4]` --
 * so gcc's HImode spelling and the ROM's word spelling assemble to the SAME
 * instruction. tryc.py compared assembly text and normalised only the `=`
 * form, so a naming difference in the printer read as a code difference for
 * four rounds. The C below is byte-for-byte the parked version.
 *
 * WHAT THE REAL DIFFERENCE WAS, once the false one was out of the way: the
 * POOL WORDS CAME OUT IN A DIFFERENT ORDER. Instruction for instruction the two
 * functions were identical and `make compare` still failed by fourteen bytes:
 *
 *     rom   0x1000  Func_800c62c  Func_800c880  0xf1ff
 *     ours  0xf1ff  0x1000        Func_800c62c  Func_800c880
 *
 * gcc orders a minipool by each entry's max_address -- fix->address plus the
 * pool_range of the referencing insn -- so the order is a direct readout of the
 * MODES. A HImode reference has a range of 32-60 bytes in arm.md and an SImode
 * one 1020, so an early narrow constant sorts before a late wide one. The ROM's
 * order says 0x1000 is HImode and 0xf1ff is SImode; ours said both were narrow.
 * The park's mode analysis was therefore right all along, and only its evidence
 * was wrong.
 *
 * The fix is the split the park had already tried and misread as "no change":
 *
 *     v = *p & 0xf1ff;      <- v stays u32 here, so the AND is SImode
 *     *p = v | 0x1000;      <- the store narrows only the OR
 *
 * That experiment was run four rounds ago. It was scored with a screen that
 * drops pool words entirely, so the one thing it changed was the one thing the
 * measurement could not see.
 *
 * Two general lessons, both now in docs/elevation.md: a pool diagnosis is about
 * ENCODINGS, so check objdump before believing one; and pool ORDER is a
 * readout of operand modes that no instruction-stream diff can show.
 */
#include "gba/types.h"

extern void Func_80042c8(void *fn);
extern void Func_800c62c(void);
extern void Func_800c880(void);
extern void _Func_8091200(int a, int b);
extern void _Func_8091254(int a);
extern void WaitFrames(int n);

void Func_800c5b4(void)
{
    vu16 *p;
    u32 v;

    Func_80042c8(Func_800c62c);
    Func_80042c8(Func_800c880);
    _Func_8091200(0x80 << 9, 1);
    _Func_8091254(1);
    WaitFrames(1);
    p = (vu16 *)(0x80 << 19);
    v = *p & 0xf1ff;
    *p = v | 0x1000;
}
