#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern char *galloc_ewram(int tag, int size);
extern void Func_8090a5c(int a, void *b, void *c, int d);
extern void Func_80908e0(void);
extern int StartTask(void *fn, int pri);

void Func_8091174(void)
{
    char *p;

    p = galloc_ewram(0x20, 0x2a04);
    DMA3_CLEAR(p, 0x2a04);
    DMA3_COPY((void *)(0xa0 << 19), p, 0x70 * 4);
    DMA3_COPY((void *)0x5000200, p + (0xe0 << 1), 0x70 * 4);
    Func_8090a5c(0x80 << 9, p, p + (0xe0 << 4), 0);
    StartTask(Func_80908e0, 0xc8f);
}
