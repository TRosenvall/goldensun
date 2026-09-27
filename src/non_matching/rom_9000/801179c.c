/* Func_801179c (TileAnimationTask) -- NON-MATCHING, 32 encodings of 127 differ (same
 * length, 127 = 127; aligned screen: 31 instructions in disagreeing regions, of 130).
 * Whole of asm/rom_9000/rom_11568_a_c_c_c.s, so no split once it matches.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/801179c.c \
 *     asm/rom_9000/rom_11568_a_c_c_c.s --func Func_801179c
 *
 * Shape is right: inner interpreter loop written with goto (a ROM loop with its test at the
 * top and `b` back-edges), delay decrement as a goto target placed last (the ROM keeps it
 * at the bottom, falling into the channel increment), one DMA3_SET per branch (cross-jump
 * merges the orr/stmia tails exactly as the ROM does), `dst = p[0]` BEFORE
 * `c->delay = p[1]` (gives the ROM's ldrh order), and DMA3_SET with the count spelled
 * `0x84000000 | (count * 8)` -- DMA3_COPY's size/4 is folded late and puts count in r5.
 *
 * BLOCKER 1 (most of the residue): global alloc takes the cursor pointer p first (35 refs
 * / 13 insns) and gives it r2, so count (preferring r2 for the DMA) lands in r4.  The ROM
 * has p = r4, count = r2.  Pinning `register u16 *p asm("r4")` takes it to 21 of 127 --
 * a fakematch, not landed.  Inert: count as u16/int, declaration permutations, p[0..2]
 * without increments, *p++ x3.
 * BLOCKER 2: prologue constant order -- 0x6004000 is loaded before 0xffff in the ROM;
 * loop.c moves 0xffff (the op compare, life 2) first.  Same registers, only the ldr order.
 * BLOCKER 3: the ewram_201c000/2020000 branches: ROM ties op<<5 to r0 and the constant to
 * r4; ours ties the constant.  Operand order / array spelling / u32 casts were inert.
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned char *iwram_3001e70;
extern unsigned char ewram_201c000[];
extern unsigned char ewram_2020000[];

typedef struct {
    u16 *base;
    u16 *cursor;
    u16 delay;
    u16 paused;
} TileAnim;

void Func_801179c(void)
{
    unsigned char *st = iwram_3001e70;
    TileAnim *c = (TileAnim *)(st + 0x18);
    u32 i;

    for (i = 0; i <= 15; i++, c++) {
        u16 *p;
        u32 op, count, dst;
        if (c->base == 0 || c->paused != 0)
            continue;
    next:
        if (c->delay != 0)
            goto dec;
        p = c->cursor;
        op = *p++;
        if (op == 0xffff) {
            c->cursor = c->base;
            goto next;
        }
        if ((op & 0xff00) == 0xfe00) {
            u32 t = op & 0xff;
            if (t == 0xff)
                continue;
            c->cursor = (u16 *)((u8 *)c->base + (t << 2));
            goto next;
        }
        count = *p++;
        dst = p[0];
        c->delay = p[1];
        if (st[0x16] == 0) {
            if (op >= 0x600)
                DMA3_SET(ewram_201c000 + op * 32, (u8 *)0x6004000 + dst * 32, 0x84000000 | (count * 8));
            else
                DMA3_SET((u8 *)0x6004000 + op * 32, (u8 *)0x6004000 + dst * 32, 0x84000000 | (count * 8));
        } else {
            if (op >= 0x200)
                DMA3_SET(ewram_2020000 + op * 64, (u8 *)0x6008000 + dst * 64, 0x84000000 | (count * 16));
            else
                DMA3_SET((u8 *)0x6008000 + op * 64, (u8 *)0x6008000 + dst * 64, 0x84000000 | (count * 16));
        }
        c->cursor += 4;
        goto next;
    dec:
        c->delay--;
    }
}
