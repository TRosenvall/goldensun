#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char *galloc_ewram(int tag, int size);
extern int StartTask(void *fn, int pri);
extern void Func_8094544(void);
extern void Func_80944ec(void);

void Func_8094730(int a, int b, int c, int d, int e, int f, int g)
{
    unsigned char *p;
    vu32 *dma;

    p = galloc_ewram(0x22, 0xf2 << 4);
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
    StartTask(Func_8094544, 0xc8 << 4);
    StartTask(Func_80944ec, 0x90 << 3);
}
