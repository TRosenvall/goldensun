#include "dma.h"

extern void *iwram_3001ed0;
extern unsigned char L17e0[] __asm__(".L17e0");
extern unsigned char L10e0[] __asm__(".L10e0");
extern void __Func_8091200(int a, int b);
extern void OvlFunc_915_2008d9c(void);

void OvlFunc_915_2008ddc(int flag)
{
    void *d;

    d = iwram_3001ed0;
    if (flag != 0)
        DMA3_COPY(L17e0, d, 0x380);
    else
        DMA3_COPY(L10e0, d, 0x380);
    __Func_8091200(0x80 << 9, 0);
    OvlFunc_915_2008d9c();
}
