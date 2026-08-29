#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001ecc;

void Func_808f498(void)
{
    unsigned char *base;
    unsigned short *p;
    int m;

    base = iwram_3001ecc;
    m = base[0x539];
    p = (unsigned short *)(base + m * 644);
    (void) UnknownDMAPrefix();
    REG_DISPCNT |= 0x6000;
    REG_WININ = *p;
    p++;
    REG_WINOUT = *p;
    p++;
    REG_WIN0H = *p;
    p++;
    REG_WIN1H = *p;
    p++;
    REG_WIN0V = 0xa0;
    REG_WIN1V = 0xa0;
    DMA0_SET(p, (void *)&REG_WIN0H, 0xa6600001);
}
