#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001f1c;
extern void ReadFlash(unsigned short sectorNum, unsigned int offset, void *dest, unsigned int size);
extern int Func_8005ae0(void);

int Func_80058ac(unsigned short sector)
{
    unsigned short buf[8];
    unsigned char *base;
    vu32 *dma;

    base = iwram_3001f1c;
    base += 0x40;
    ReadFlash(sector, 0, base, 0x80 << 5);
    DMA3_COPY(base, buf, 0x10);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    return (unsigned short)Func_8005ae0() - buf[4];
}
