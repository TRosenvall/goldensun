#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *iwram_3001f1c;
extern const unsigned char _TBL_79b8[] __asm__(".L79b8");
extern int Func_8005868(int channel);

int Func_8005b64(int channel, int pitch)
{
    unsigned char buf[0x10];
    unsigned char *base;
    vu32 *dma;

    base = iwram_3001f1c;
    DMA3_CLEAR(buf, 0x10);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    DMA3_COPY(_TBL_79b8, buf, 8);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    buf[7] = 0x10;
    *(unsigned short *)(buf + 0xa) = 0;
    DMA3_COPY(buf, base + 0x40, 0x10);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    if (Func_8005868(channel) != 0)
        return 1;
    base[channel] = 0;
    base[channel + 0x10] = 0x10;
    *(unsigned short *)(base + 0x20 + channel * 2) = 0;
    return 0;
}
