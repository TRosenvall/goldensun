#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char L650[] __asm__(".L650");
extern int _FILE_1c;

extern void *__Func_8004970(int size);
extern short __AllocSpriteSlot(void);
extern void *__GetFile(int id);
extern void __DecompressLZ(void *src, void *dst);
extern void __UploadSpriteGFX(int slot, int size, void *src);
extern void __free(void *p);

void OvlFunc_879_20081c0(void)
{
    unsigned char *buf;
    vu32 *dma;
    unsigned char *p;

    buf = __Func_8004970(0xa4 << 3);
    if (*(short *)L650 == -1)
        *(short *)L650 = __AllocSpriteSlot();
    __DecompressLZ(__GetFile((int)&_FILE_1c), buf);
    DMA3_COPY(buf, (void *)0x50003e0, 0x20);
    p = buf + 0x20;
    __UploadSpriteGFX(*(short *)L650, 0xa0 << 3, p);
    dma = (vu32 *)&REG_DMA3SAD;
    while (dma[2] & 0x80000000)
        ;
    __free(buf);
}
