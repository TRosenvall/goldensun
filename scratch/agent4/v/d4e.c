#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *__galloc_ewram(int tag, int size);
extern void __StartTask();
extern void OvlFunc_970_2008f80(void);
extern void OvlFunc_970_2008f30(void);

void OvlFunc_970_20090d4(int a, int b, int c, int d, int e, int f, int g)
{
    unsigned char *p;
    vu32 *dma;

    p = __galloc_ewram(0x22, 0xf2 << 4);
    DMA3_CLEAR(p, 0xf2 << 4);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    p[0xf01] = a;
    *(int *)(p + 0xf08) = b;
    *(int *)(p + 0xf0c) = e;
    *(int *)(p + 0xf18) = d;
    *(int *)(p + 0xf1c) = g;
    *(int *)(p + (0xf1 << 4)) = c;
    *(int *)(p + 0xf14) = f;
    __StartTask(OvlFunc_970_2008f80, 0xc8 << 4);
    __StartTask(OvlFunc_970_2008f30, 0x90 << 3);
}
