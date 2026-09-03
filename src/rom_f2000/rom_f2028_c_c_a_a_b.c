/* Func_80f377c  --  0x080f377c   "StartPaletteFadeEngine"
 *
 * Cut out of goldensun/asm/rom_f2000/rom_f2028_c_c_a_a.s.
 *
 * Takes 0x3004 bytes under tag 0x20 -- the block iwram_1ed0 points at -- and
 * snapshots both palettes into the front of it: 0x200 bytes from 0x5000000 and
 * 0x200 from 0x5000200, so +0x000..+0x400 is the packed 512-colour copy.
 * Func_80f3078 then unpacks it into the working buffer at +0x1000, and
 * Func_80f2f10 is registered at sort key 0xc80.
 *
 * The block's layout, from here and Func_80f3858:
 *
 *     +0x0000  the packed snapshot, and a step index halfword at +0
 *     +0x0400  the CURRENT unpacked palette, 0x600 halfwords
 *     +0x1000  the TARGET unpacked palette
 *     +0x1c00  the per-frame step from Func_80f2ebc
 *     +0x3001  frames remaining     +0x3002  a phase flag
 *
 * THE ALLOC SIZE AND THE CLEAR SIZE ARE ONE CONSTANT. The control word
 * 0x85000c01 decodes as 0x85000000 | (0x3004 / 4) -- the same 0x3004 handed to
 * galloc_ewram -- which is what says DMA3_CLEAR rather than a separate literal.
 * The two 0x84000080 words are 0x84000000 | (0x200 / 4), the two DMA3_COPY
 * calls. Picking DMA3_CLEAR over DMA3_SET is the "where the fill value is
 * stored" rule: `mov r0, sp / mov r3, #0 / str r3, [r0]` is the
 * store-through-r0 signature, and the value is zero.
 *
 * `push {lr}` with r4 NOT saved, while r4 holds the block pointer, is not an
 * anomaly -- it is -fcall-used-r4 working as intended. r4 is live across the
 * three inline-asm blocks (which bind r0-r3) but DEAD BEFORE THE NEXT `bl`, so
 * gcc may use it at no prologue cost. This is the converse of the entry about
 * a prologue that pushes r4 and keeps a value in it ACROSS a call, which means
 * the TU was built without the flag. Check the liveness before filing a
 * missing push as a wall.
 *
 * Constant synthesis is not a source tell here: 0x5000000 comes out as
 * `mov r0, #0xa0 / lsl r0, #19` while 0x5000200 pools, and 0x200 and 0x1000
 * are built with shifts. All of that falls out of plain hex literals.
 */

#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern int StartTask(void *fn, int pri);
extern void Func_80f3078(int a, void *b, void *c, int d);
extern void Func_80f2f10(void);

void Func_80f377c(void)
{
    unsigned char *p;

    p = galloc_ewram(0x20, 0x3004);
    DMA3_CLEAR(p, 0x3004);
    DMA3_COPY((void *)0x5000000, p, 0x200);
    DMA3_COPY((void *)0x5000200, p + 0x200, 0x200);
    Func_80f3078(0x10000, p, p + 0x1000, 0);
    StartTask(Func_80f2f10, 0xc8 << 4);
}
