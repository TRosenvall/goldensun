/* Func_8011bf4 (PaletteCycleTask) -- NON-MATCHING, 78 encodings of 115 differ (ours 113:
 * TWO SHORT, 232 bytes vs 236, so the positional 78 is mostly shift; aligned screen: 23
 * instructions in disagreeing regions, of 120).  Whole of asm/rom_9000/rom_11568_c_c_c_c_c.s.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/8011bf4.c \
 *     asm/rom_9000/rom_11568_c_c_c_c_c.s --func Func_8011bf4
 *
 * What moved it (47 -> 23 aligned):
 *  - loop bound `i < (n & 3)` with n the raw halfword: loop.c hoists the AND into a new
 *    pseudo in the pre-header, which is the ROM's `mov r9, r2` AFTER the entry test;
 *  - `void *dst = c->dst;` declared/assigned before `src` (dst -> r8, i -> r14 as ROM);
 *  - phase/len read as s16 locals, then u16 copies (ulen, uph) -- the ROM's r6 = len<<16
 *    with a separate lsr at every use comes out of that;
 *  - the phase update through a u32 <<16 temporary (t) instead of a u16 np.
 * Remaining:
 *  - src (colours pointer) takes r1 and the per-loop `mov rX, sp` buffer bases r4; ROM is
 *    the other way round.  src (20 refs/32 insns) outranks the hoisted sp copies; nothing
 *    tried gives it an r1 conflict.  Inert: src assignment position (y1-y4), `u16 *b = buf`,
 *    byte-offset spelling.
 *  - the ROM copies phase<<16 into a new pseudo (`lsl r1, r7, #16; mov r7, r1`); ours
 *    shifts in place.
 *  - ROM keeps t in shifted form across the join (`mov r1, #0 / lsr r3, r1, #16 / strh`);
 *    ours folds t>>16 to 0 on the else path (one instruction short).
 *  - `n & 3`: ROM puts the 3 in the AND's destination (mov r2,#3; and r2,r3); `3 & n`,
 *    `(u8)(n & 3)`, `3u` inert, `(u16)` worse.
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned char *iwram_3001ec0;

typedef struct {
    void *dst;
    s16 phase;
    u16 counter;
    u16 delay;
    s16 len;
    u16 colors[16];
} PalCycle;

void Func_8011bf4(void)
{
    unsigned char *st = iwram_3001ec0;
    u8 i;
    u16 buf[16];
    u32 n = *(u16 *)(st + 0xb0);

    for (i = 0; i < (n & 3); i++) {
        PalCycle *c = (PalCycle *)(st + i * 0x2c);
        if (c->counter == 0) {
            s16 phase = c->phase;
            s16 len = c->len;
            u8 j = len - phase;
            u16 ulen = len;
            u16 uph;
            void *dst = c->dst;
            u16 *src = c->colors;
            u16 np;
            for (; j < ulen; j++)
                buf[j] = *src++;
            uph = phase;
            for (j = 0; j < ulen - uph; j++)
                buf[j] = *src++;
            DMA3_SET(buf, dst, 0x80000000 | ulen);
            {u32 t = (uph + 1) << 16;
            if ((t >> 16) >= ulen)
                t = 0;
            c->phase = t >> 16;}
            c->counter = c->delay;
        } else {
            c->counter--;
        }
    }
}
