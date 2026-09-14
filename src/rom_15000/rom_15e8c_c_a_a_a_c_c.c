#include "dma.h"

extern unsigned char *iwram_3001e8c;
extern void Func_801e260(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

/* FillTilemapRun
 * r0 = destination, r1 = tile entry, r2 = count.  DMA3-fills `count` halfwords
 * with the same tile entry and returns the advanced destination pointer, so
 * callers can chain runs.  A count of 0 or less writes nothing and returns the
 * pointer unchanged.  The source is a stack halfword with the DMA source-fixed
 * bit set (control 0x81000000).
 */
void *Func_80170c4(void *dst, int value, int n)
{
    unsigned short buf;

    if (n > 0) {
        buf = value;
        DMA3_SET(&buf, dst, (0x81 << 24) | n);
        dst = (unsigned short *)dst + n;
    }
    return dst;
}

/* DrawWindowFrame
 * r0 = x column, r1 = y row, r2 = width, r3 = height, in tiles.
 * Paints a window's border and interior into the tilemap at
 * [iwram_1e8c] + (row*32 + column)*2, emitting each span with Func_80170c4.
 * Widths or heights of 1 or less, or over 30, take an early exit, so
 * degenerate windows draw nothing rather than corrupting the map.
 * The +0xEA4 byte selects between two tile sets for the corner and edge
 * pieces; +0xEA3 is set to 1 to mark the tilemap dirty.
 */
void Func_80170f8(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    unsigned char *base;
    unsigned short *p;
    unsigned int i;
    int n;

    base = iwram_3001e8c;
    p = (unsigned short *)(((y << 5) + x) * 2 + (unsigned int)base);
    if (w > 1 && h > 1 && w <= 0x1e && h <= 0x1e) {
        Func_801e260(x, y, w, h);
        if (base[0xea4] != 0) {
            *p = 0xf01c;
            p++;
        } else {
            *p = 0xf010;
            p++;
        }
        n = w - 2;
        p = (unsigned short *)Func_80170c4(p, 0xf011f011, n);
        if (base[0xea4] != 0) {
            *p = 0xf41c;
            p++;
        } else {
            *p = 0xf012;
            p++;
        }
        p += 0x20 - w;
        for (i = 1; i < h - 1; i++) {
            *p = 0xf016;
            p++;
            if (w != 2)
                p = (unsigned short *)Func_80170c4(p, 0xf020f020, n);
            *p = 0xf017;
            p++;
            p += 0x20 - w;
        }
        if (base[0xea4] != 0)
            *p = 0xf81c;
        else
            *p = 0xf013;
        p++;
        p = (unsigned short *)Func_80170c4(p, 0xf014f014, n);
        if (base[0xea4] != 0)
            *p = 0xfc1c;
        else
            *p = 0xf015;
        base[0xea3] = 1;
    }
}

/* FillWindowInterior
 * r0 = x column, r1 = y row, r2 = width, r3 = height, arg5 = whole-rect flag.
 * Same (row*32 + column)*2 indexing and the same early-out as DrawWindowFrame.
 * Starting one row down, it writes a rectangle of consecutive tile entries,
 * numbered COLUMN-MAJOR from 0x127 and masked into the 0xF000 palette/priority
 * bits.  With arg5 == 0 only the interior columns 1..w-2 are painted, leaving
 * the frame DrawWindowFrame drew; with arg5 != 0 the full width 0..w-1 is.
 * Sets +0xEA3 to 1 to mark the tilemap dirty.
 */
void Func_8017248(unsigned int x, unsigned int y, unsigned int w, unsigned int h, int flag)
{
    unsigned char *base;
    unsigned short *p;
    unsigned int i, j;

    base = iwram_3001e8c;
    p = (unsigned short *)(((y << 5) + x) * 2 + (unsigned int)base);
    if (w > 1 && h > 1 && w <= 0x1e && h <= 0x1e) {
        p += 0x20;
        if (flag == 0) {
            for (i = 1; i < h - 1; i++) {
                p++;
                for (j = 1; j < w - 1; j++) {
                    *p = ((0x127 + i + (j - 1) * (h - 2)) & 0xfff) | 0xf000;
                    p++;
                }
                p++;
                p += 0x20 - w;
            }
        } else {
            for (i = 1; i < h - 1; i++) {
                for (j = 0; j < w; j++) {
                    *p = ((i + 0x127 + j * (h - 2)) & 0xfff) | 0xf000;
                    p++;
                }
                p += 0x20 - w;
            }
        }
        base[0xea3] = 1;
    }
}
