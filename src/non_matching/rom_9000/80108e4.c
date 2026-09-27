/* Func_80108e4 -- NON-MATCHING, 113 encodings of 120.  ref 260 bytes / ours 252,
 * ref 120 instructions / ours 116 -- FOUR SHORT, so the 113 measures the shift too.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/80108e4.c \
 *     asm/rom_9000/rom_108e4_a.s --func Func_80108e4
 *
 * Landing needs the same text/data split as its file-mate Func_8010e14 -- see that park's
 * header for the shape, which is already established in this bank.
 *
 * Body structure is right: galloc_iwram / DecompressLZ1 / the 16x DMA3_COPY stride
 * 0x40->0x80 loop / the 16x16 metatile expansion / gfree.
 *
 * TWO NAMED BLOCKERS:
 *
 * 1. gcc CSEs `layer*2 + (qz & 1)` INTO ONE REGISTER; THE ROM RECOMPUTES
 *    `mov r3,r9 / add r3,r8` AT ALL THREE USE SITES.  The ROM therefore holds THREE live
 *    high-register values (l2->r9, lz->r8, lx->r10) where this holds two, which shifts the
 *    fifth argument from [sp,#0x1c] to [sp,#0x18] and cascades.  Pinning the three to
 *    r9/r8/r10 recovers the frame offset and moves the first difference from index 2 to
 *    index 7 (116 insns, 112 differing) but does not close it.  THE CURE IS TO STOP THE
 *    CSE, not to place the registers.
 *
 * 2. `check_dbra_loop` REVERSES BOTH COUNTERS -- `mov r6,#0xf / sub r6,#1 / bge` where the
 *    ROM has `mov r6,#0 / add r6,#1 / cmp #0xf / bls`.  PASS: inert to `unsigned int`
 *    counters; inert to rewriting both loops as `do { } while (++i <= 0xf)`; and WORSE
 *    (138 insns) when the addresses are written as i-indexed givs instead of walking
 *    pointers.
 */
#include "dma.h"

extern unsigned char *iwram_3001e70;
extern unsigned char ewram_2020000[];
extern unsigned char gBuffer[];
extern unsigned char ewram_2010002[];
extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void DecompressLZ1(void *src, void *dst);

int Func_80108e4(int layer, int qx, int qz, int tileset, int force)
{
    unsigned char *arc;
    unsigned char *buf;
    unsigned char *src;
    unsigned char *dst;
    unsigned short *slot;
    register int lx __asm__("r10");
    register int lz __asm__("r8");
    register int l2 __asm__("r9");
    unsigned int i;
    unsigned int j;

    arc = *(unsigned char **)(iwram_3001e70 + (0x88 << 1));
    lx = qx & 1;
    l2 = layer << 1;
    lz = qz & 1;
    slot = (unsigned short *)(iwram_3001e70 + (((l2 + lz) * 2 + lx) * 2) + (0xce << 2));
    if (force == 0 && tileset == *slot)
        return 0;
    *slot = tileset;
    buf = galloc_iwram(0xe, 0x80 << 3);
    DecompressLZ1(arc + ((unsigned int *)arc)[tileset], buf);
    src = buf;
    dst = ewram_2020000 + ((((l2 + lz) << 5) + lx) << 6);
    for (i = 0; i <= 0xf; i++) {
        DMA3_COPY(src, dst, 0x40);
        src += 0x40;
        dst += 0x80;
    }
    if (force != 0) {
        unsigned short *d;
        d = (unsigned short *)((unsigned char *)0x6004000 + ((((l2 + lz) << 6) + lx) << 5));
        src = buf;
        for (i = 0; i <= 0xf; i++) {
            for (j = 0; j <= 0xf; j++) {
                unsigned int t = *(unsigned short *)src;
                d[0] = *(unsigned short *)(gBuffer + t * 4);
                d[0x20] = *(unsigned short *)(ewram_2010002 + t * 4);
                d++;
                src += 4;
            }
            d += 0x30;
        }
    }
    gfree(0xe);
    return 1;
}
