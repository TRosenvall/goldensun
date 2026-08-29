#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80f0254(int page)
{
    int slot;
    int value;
    void *dst;
    void *pal;

    if (page == 0) {
        dst = (void *)(0xc0 << 19);
        pal = (void *)(0xa0 << 19);
        value = 0x1010101;
    } else {
        value = 0x81818181;
        dst = (void *)0x6008000;
        pal = (void *)0x5000100;
    }
    slot = value;
    DMA3_SET(&slot, dst, 0x85001e00);
    value = 0;
    slot = value;
    DMA3_SET(&slot, pal, 0x85000040);
}
