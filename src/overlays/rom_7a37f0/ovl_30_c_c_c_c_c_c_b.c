#include "dma.h"

extern void *iwram_3001ed0;
extern unsigned char L19d0[] __asm__(".L19d0");
extern unsigned char L12d0[] __asm__(".L12d0");
extern void __Func_8091200(int a, int b);
extern void OvlFunc_916_2008f74(void);

void OvlFunc_916_2008fb4(int flag)
{
    void *d;

    d = iwram_3001ed0;
    if (flag != 0)
        DMA3_COPY(L19d0, d, 0x380);
    else
        DMA3_COPY(L12d0, d, 0x380);
    __Func_8091200(0x80 << 9, 0);
    OvlFunc_916_2008f74();
}
