/* Func_80d67dc -- 0x080d67dc, asm/rom_c9000/rom_d6504_a_c_c_c.s
 * (four functions; Func_80d6750 beside it is elevated).
 *
 * BLOCKER CLASS: the frame base gcc picks for a store -- `[sp]` against a
 * register holding sp. TWO encodings of 72, LENGTH EXACT, everything else
 * instruction for instruction:
 *
 *     rom    mov r4, sp / str r5, [r4]      (twice)
 *     ours   mov r4, sp / str r5, [sp]      (twice)
 *
 * `mov r4, sp` is present in both -- the address is needed in a register for
 * the DMA asm's r0 operand either way -- and the ROM reuses it for the store
 * while gcc addresses off sp. Both are valid Thumb; only the encoding differs.
 *
 * THE INTERESTING RESULT IS THE INSTRUCTION THAT WAS MISSING, AND IT IS A
 * dma.h FINDING. The first candidate was ONE INSTRUCTION SHORT: the ROM loads
 * the DMA count `0x85001000` from the pool TWICE, once per transfer, and gcc
 * loads it once and reuses r2 across both.
 *
 *     rom    ldr r1,=0x6004000 / ldr r2,=0x85001000 / stmia r3!,{r0,r1,r2}
 *     ours   ldr r1,=0x6004000 /                      stmia r3!,{r0,r1,r2}
 *
 * DMA3_SET binds the count to r2 as an INPUT and clobbers only `"memory", "r0"`,
 * so gcc knows r2 survives the asm and commons the two loads. Adding `"r2"` to
 * that clobber list reproduces the ROM exactly: 71 instructions -> 72, and the
 * residue drops from 30 differing to 2.
 *
 * THE CLOBBER CHANGE IS TREE-WIDE GREEN, MEASURED. dma.h's DMA3_SET is used by
 * 29 files; widening its clobber to `"memory", "r0", "r2"` and rebuilding
 * everything leaves `make compare` GREEN, so no other matched function depends
 * on the narrower list.
 *
 * IT IS NOT IN THE TREE, AND DELIBERATELY. The change buys nothing on its own --
 * this function still does not match -- and declaring an INPUT register as
 * clobbered is a fiction about what the asm does, not a claim about the
 * hardware. Shipping a shared header on a fiction that completes no function is
 * not worth it. Whoever closes the last two encodings should make that change
 * at the same time; the measurement above is the evidence that it is safe.
 *
 * ALSO SETTLED, so nobody re-derives it:
 *   * The pooled halfword constants (1, 0x100e, 0x3f46, 0x7741) are the
 *     documented halfword exception -- they are stored through `strh` to the
 *     I/O registers, gcc narrows them to HImode, and HImode constants go to the
 *     pool. gcc prints `ldrh rX, .L` and GAS folds it to the ROM's `ldr`.
 *   * REG_DISPCNT / REG_BLDCNT / REG_BLDALPHA from include/gba/io.h give the
 *     ROM's `mov r1,#0x80 / lsl r1,#19` address build and the `sub r2,#2`
 *     between the two blend registers.
 *   * The zero word is CALLER-OWNED and passed to DMA3_SET (batch 267's
 *     result), not DMA3_CLEAR -- the ROM's `mov r4, sp` and the two stores
 *     through one buffer require it.
 *
 * MEASURED AND INERT, all 2 with the clobber in place: `u32 *volatile q`
 * (worse, 74 instructions), `q[0] = 0` instead of `*q = 0`, and `value`
 * declared as a one-element array with `q = value`.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern char *iwram_3001e74[];
extern u16 iwram_3001ad0[];
extern int gPhysVec[];
extern void _AnimTransitionIn(int a, int b, int c);

void Func_80d67dc(void)
{
    u32 value;
    u32 *q;
    char *buf;
    char *base;

    buf = iwram_3001e74[0x7c / 4];
    base = iwram_3001e74[0];
    REG_DISPCNT = 1;
    iwram_3001ad0[3] = 0x20;
    _AnimTransitionIn(1, *(u16 *)(base + (0xc9 << 3)), 0x18);
    q = &value;
    *q = 0;
    DMA3_SET(q, buf, 0x85001000);
    *q = 0;
    DMA3_SET(q, (void *)0x6004000, 0x85001000);
    WaitFrames(1);
    REG_BLDALPHA = 0x100e;
    REG_BLDCNT = 0x3f46;
    REG_DISPCNT = 0x7741;
    gPhysVec[0x10 / 4] = 0x78;
}
