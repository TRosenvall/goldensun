#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80f0254(int page)
{
    int v;
    void *dst;
    void *pal;

    if (page == 0) {
        dst = (void *)(0xc0 << 19);
        pal = (void *)(0xa0 << 19);
        v = 0x1010101;
    } else {
        v = 0x81818181;
        dst = (void *)0x6008000;
        pal = (void *)0x5000100;
    }
    DMA3_SET(&v, dst, 0x85001e00);
    v = 0;
    DMA3_SET(&v, pal, 0x85000040);
}
