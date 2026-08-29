#include "dma.h"

extern void *iwram_3001ed0;
extern unsigned char L24e0[] __asm__(".L24e0");
extern unsigned char L1de0[] __asm__(".L1de0");
extern void __Func_8091200(int a, int b);
extern void OvlFunc_917_2009878(void);

void OvlFunc_917_20098b8(int flag)
{
    void *d;

    d = iwram_3001ed0;
    if (flag != 0)
        DMA3_COPY(L24e0, d, 0x380);
    else
        DMA3_COPY(L1de0, d, 0x380);
    __Func_8091200(0x80 << 9, 0);
    OvlFunc_917_2009878();
}
