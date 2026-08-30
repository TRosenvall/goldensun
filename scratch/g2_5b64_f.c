#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct SoundChan {
    unsigned char f0[7];
    unsigned char f7;
    unsigned short f8;
    unsigned short fa;
    unsigned char fc[4];
};

extern unsigned char *iwram_3001f1c;
extern const unsigned char _TBL_79b8[] __asm__(".L79b8");
extern int Func_8005868(int channel);

int Func_8005b64(int channel, int pitch)
{
    struct SoundChan buf;
    unsigned char *base;
    vu32 *dma;
    int off;
    int ret;

    base = iwram_3001f1c;
    DMA3_CLEAR(&buf, 0x10);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    DMA3_COPY(_TBL_79b8, &buf, 8);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    buf.f7 = 0x10;
    buf.fa = 0;
    DMA3_COPY(&buf, base + 0x40, 0x10);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    ret = Func_8005868(channel);
    if (ret == 0) {
        base[channel] = ret;
        off = channel + 0x10;
        base[off] = 0x10;
        off = channel * 2;
        off += 0x20;
        *(unsigned short *)(base + off) = ret;
        return 0;
    } else {
        return 1;
    }
}
