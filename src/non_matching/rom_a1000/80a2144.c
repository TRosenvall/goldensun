/* Func_80a2144 (LoadMenuPalette) -- NON-MATCHING.
 * Blocker class: THE MACRO'S PINNED REGISTER IS ALREADY CORRECT -- the second
 * instance of the class first recorded in src/non_matching/rom_b0000/80b0840.c.
 * 45 lines against the ROM's 47, 36 differing, all downstream of one elision.
 *
 * The function issues two DMA3 transfers from the SAME source, 0x50001e0:
 *
 *     rom    ldr r0, =0x50001e0 / ldr r2, =0x84000008 / stmia r3!, {r0,r1,r2}
 *     ours                        ldr r2, =0x84000008 / stmia r3!, {r0,r1,r2}
 *
 * DMA3_SET pins its source to r0 and its inline asm clobbers "memory" but not
 * r0, so gcc sees the pinned register already correct after the first transfer
 * and emits nothing. The ROM reloads it. Our stream is two instructions short
 * and every later difference follows from the shift.
 *
 * That confirms the precondition stated in the first park: the elision happens
 * when two transfers SHARE A SOURCE. rom_7a4370/ovl_30_c_c_c_c_c_c_b.c calls
 * DMA3_COPY twice and matches, because its sources differ. Two instances now,
 * same shape, same cause.
 *
 * ALSO FIXED HERE, though masked by the above: the three colour-channel
 * extractions need UNSIGNED shifts. `(c << 16) >> 26` on an int operand gives
 * `asr`; the ROM has `lsr`. Casting to `unsigned int` before the shift chain
 * produces the logical form. The fix is kept in the source below even though it
 * cannot be verified past the elision.
 *
 * Not tried, because the first park already measured it: anything aimed at the
 * elision itself. That note records that adding "r0" to DMA3_SET's clobber list
 * would force the reload and risks every other DMA3_SET user, and that the
 * whole corpus should be measured before touching include/dma.h. That applies
 * unchanged here, and with a second function now waiting on it the measurement
 * is worth more than it was.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80a2144(int bank)
{
    unsigned short *pal;
    unsigned short c;
    int r, g, b;
    unsigned int t;

    pal = (unsigned short *)((bank << 5) + (0xa0 << 19));
    DMA3_SET((void *)0x50001e0, pal, 0x80000010);
    DMA3_SET((void *)0x50001e0, pal, 0x84000008);
    c = pal[4];
    t = (unsigned int)c << 16;
    b = t >> 26;
    g = (t >> 21) & 0x1f;
    r = 0x1f & c;
    b += 9;
    if (b > 0x1f)
        b = 0x1f;
    g += 9;
    if (g > 0x1f)
        g = 0x1f;
    r += 9;
    if (r > 0x1f)
        r = 0x1f;
    pal[4] = (b << 10) | (g << 5) | r;
}
