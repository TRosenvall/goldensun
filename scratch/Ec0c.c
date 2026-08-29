#include "dma.h"

extern void *iwram_3001ed0;
extern unsigned char L17b0[] __asm__(".L17b0");
extern unsigned char L10b0[] __asm__(".L10b0");
extern void __Func_8091200(int a, int b);
extern void OvlFunc_914_2008bcc(void);

void OvlFunc_914_2008c0c(int flag)
{
    void *d;

    d = iwram_3001ed0;
    if (flag != 0)
        DMA3_COPY(L17b0, d, 0x380);
    else
        DMA3_COPY(L10b0, d, 0x380);
    __Func_8091200(0x80 << 9, 0);
    OvlFunc_914_2008bcc();
}
