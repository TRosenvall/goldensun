#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001ecc;

void Func_808f498(void)
{
    unsigned char *base;
    vu16 *p;
    vu16 *w;
    vu16 *h0;
    vu16 *h1;
    int m;
    int v;

    base = iwram_3001ecc;
    m = base[0x539];
    p = (vu16 *)(base + m * 644);
    (void) UnknownDMAPrefix();
    REG_DISPCNT |= 0x6000;
    w = &REG_WININ;
    *w = *p;
    p++;
    w++;
    *w = *p;
    p++;
    h0 = &REG_WIN0H;
    *h0 = *p;
    p++;
    h1 = &REG_WIN1H;
    *h1 = *p;
    p++;
    v = 0xa0;
    h1++;
    *h1 = v;
    h1++;
    *h1 = v;
    DMA0_SET(p, (void *)h0, 0xa6600001);
}
