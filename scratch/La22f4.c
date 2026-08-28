#include "dma.h"

void Func_80a22f4(void)
{
    unsigned int d;

    d = 0x50001c0;
    DMA3_SET((const void *)0x5000200, (void *)d, 0x80000010);
    d += 0x1c;
    DMA3_SET((const void *)0x50001e8, (void *)d, 0x80000001);
}
