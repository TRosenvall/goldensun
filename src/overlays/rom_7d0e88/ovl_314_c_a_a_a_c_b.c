#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001e70;

void OvlFunc_947_2008fcc(int a, int b, int c, void *dst)
{
    unsigned char *base;
    unsigned char *tbl;
    vu32 *dma;
    int k;

    base = iwram_3001e70;
    if (base == 0)
        return;
    k = (a * 3 << 4) + (0x98 << 1);
    tbl = *(unsigned char **)(base + k);
    DMA3_COPY(tbl + ((b + (c << 7)) << 2), dst, 4);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
}
