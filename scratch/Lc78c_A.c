#include "dma.h"

extern unsigned char L5140[] __asm__(".L5140");
extern unsigned char L5168[] __asm__(".L5168");
extern unsigned char *__galloc_ewram(int tag, int size);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_800c548(unsigned char *a, int n);
extern void __StartTask(void *fn, int pri);
extern void OvlFunc_896_200c49c(void);

void OvlFunc_896_200c78c(int slot, unsigned int n)
{
    unsigned char *buf;
    unsigned char *p;
    unsigned char *a;
    unsigned int i;
    unsigned int j;
    int z;

    buf = __galloc_ewram(0x21, 0xca << 1);
    p = buf;
    DMA3_CLEAR(buf, 0xca << 1);
    if (n > 0xa)
        n = 0xa;
    i = 0;
    if (n != 0) {
        z = 0;
        j = 0;
        do {
            a = __MapActor_GetActor(slot);
            *(unsigned char **)p = a;
            *(unsigned char *)(*(int *)(a + 0x50) + 0x26) = z;
            a += 0x55;
            *a = z;
            a = __MapActor_GetActor(slot);
            __Func_800c548(a, 1);
            *(int *)(p + 0x1c) = *(int *)((char *)L5140 + j);
            *(int *)(p + 0x20) = -*(int *)((char *)L5168 + j);
            p[0x24] = 3;
            i++;
            j += 4;
            p += 0x28;
            slot++;
        } while (i != n);
    }
    *(short *)(buf + (0xc8 << 1)) = n;
    __StartTask(OvlFunc_896_200c49c, 0xc8 << 4);
}
