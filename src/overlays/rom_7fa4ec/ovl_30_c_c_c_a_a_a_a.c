#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned short L181c __asm__(".L181c");
extern unsigned short L14ac[] __asm__(".L14ac");

void OvlFunc_970_20080b0(void)
{
    unsigned short i;

    i = L181c / 6;
    DMA3_SET(&L14ac[i], (void *)0x50000e8, 0x80000006);
    L181c = L181c + 1;
    if (L181c > 0x23)
        L181c = 0;
}
