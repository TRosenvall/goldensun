#include "dma.h"

extern void *GetFile(int id);

void Func_80f61e8(int id)
{
    unsigned short buf[0x40];
    unsigned short *pal;
    void *f;
    unsigned int t;
    int i, off;
    int r, g, b, r2, g2, b2;

    pal = (unsigned short *)(0xa0 << 19);
    f = GetFile(id);
    DMA3_SET(f, buf, 0x84000020);
    i = 0;
    off = 0;
    do {
        t = *pal;
        r = 0x1f & t;
        t <<= 16;
        g = t >> 21;
        b = t >> 26;
        t = *(unsigned short *)(off + (int)buf);
        r2 = 0x1f & t;
        t <<= 16;
        g2 = t >> 21;
        b2 = t >> 26;
        g &= 0x1f;
        b &= 0x1f;
        g2 &= 0x1f;
        b2 &= 0x1f;
        if (r < r2)
            r++;
        else if (r > r2)
            r--;
        if (g < g2)
            g++;
        else if (g > g2)
            g--;
        if (b < b2)
            b++;
        else if (b > b2)
            b--;
        g <<= 5;
        *(unsigned short *)(off + (int)buf) = (b << 10) | g | r;
        i++;
        off += 2;
        pal++;
    } while (i != 0x40);
    DMA3_SET((char *)buf + 2, (void *)0x5000002, 0x8000003f);
}
