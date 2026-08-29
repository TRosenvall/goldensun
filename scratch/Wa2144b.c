#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern int _CONST_1f;

void Func_80a2144(int bank)
{
    unsigned short *pal;
    unsigned short c;
    unsigned int r, g, b;
    unsigned int t;

    pal = (unsigned short *)((bank << 5) + (0xa0 << 19));
    DMA3_SET((void *)0x50001e0, pal, 0x80000010);
    DMA3_SET((void *)0x50001e0, pal, 0x84000008);
    c = pal[4];
    t = (unsigned int)c << 16;
    b = t >> 26;
    g = (t >> 21) & (int)&_CONST_1f;
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
