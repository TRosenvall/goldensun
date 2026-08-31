/* Func_8011b54 -- 0x08011b54 -- asm/rom_9000/rom_11568_c_c_c_c_a_a_c.s
 *
 * SCREENED TWICE, NOT PURSUED. Best 50 of 55 at 56 lines. Filed at the depth
 * actually reached so the next attempt does not repeat it.
 *
 * Builds a DMA3 queue entry: guard the slot count against 3, fill five fields
 * of a 0x2c-byte slot, kick a DMA3 transfer from palette RAM into the slot,
 * and bump the count. The body reads cleanly --
 *
 *     src   = 0x5000000 + (((u16)a << 4) + (u16)b) * 2
 *     slot  = base + count * 0x2c
 *     slot[4] = slot[6] = 0;  slot[0xa] = d;  *(int *)slot = src;  slot[8] = c
 *     DMA3_SET(src, slot + 0xc, (u16)d | 0x80000000)
 *
 * -- and the arithmetic above is believed right; it is the PARAMETER HANDLING
 * that is unresolved, and both attempts diverge at instruction 1 or 2.
 *
 * The ROM sign-extends all four arguments at entry, in the order c, d, b, a:
 *
 *     lsl r2, #16 / lsl r3, #16 / asr r7, r2, #16 / asr r2, r3, #16
 *     ... lsl r1, #16 ... asr r6, r1, #16 ... lsl r0, #16 / asr r0, #16
 *
 * MEASURED:
 *   `short` parameters (gcc extends at entry)      56 lines, 50 differ
 *   `int` parameters with explicit (short) casts   50 lines, 54 differ
 *
 * The second is five lines SHORT, so the casts collapse rather than producing
 * the entry extensions -- `short` parameters are closer to right. What neither
 * reproduces is the ORDER: the ROM extends the third and fourth arguments
 * before the second and first, which is not the order either spelling gives.
 *
 * NEXT STEP if this is retried: the extension order is the whole problem, and
 * it most likely follows from which argument is USED first in the original
 * rather than from the declaration. Reading the body for the first use of each
 * parameter, and reordering the statements to match, is the thing to try --
 * not more casts.
 */
#include "dma.h"

extern int iwram_3001ec0;

int Func_8011b54(short a, short b, short c, short d)
{
    char *base;
    unsigned short *cnt;
    int n;
    char *slot;
    int src;

    base = (char *)iwram_3001ec0;
    cnt = (unsigned short *)(base + 0xb0);
    n = *cnt;
    if (n > 3)
        return -1;
    slot = base + n * 0x2c;
    src = (((unsigned short)a << 4) + (unsigned short)b) * 2 + 0xa0 * 0x80000;
    *(unsigned short *)(slot + 4) = 0;
    *(unsigned short *)(slot + 6) = 0;
    *(unsigned short *)(slot + 0xa) = d;
    *(int *)slot = src;
    *(unsigned short *)(slot + 8) = c;
    DMA3_SET((void *)src, slot + 0xc, (unsigned short)d | 0x80000000);
    *cnt = n + 1;
    return 0;
}
