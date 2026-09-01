#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001f1c;
extern unsigned int Func_8005b24(void);
extern int Func_80058ac(int sector);

int Func_8005a78(int a, void *dst)
{
    unsigned char *base;
    vu32 *dma;
    unsigned int n;

    base = iwram_3001f1c;
    n = Func_8005b24();
    if (n > 0xf)
        return 1;
    Func_80058ac(n);
    DMA3_COPY(base + 0x50, dst, 0xff0);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    return 0;
}
