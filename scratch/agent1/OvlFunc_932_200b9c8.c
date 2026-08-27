#include "dma.h"

extern unsigned char *iwram_3001ed0;
extern short L525c[] __asm__(".L525c");
extern unsigned short L5260[] __asm__(".L5260");
extern signed char s8_ARRAY_932__0200bd28[];

void OvlFunc_932_200b9c8(void)
{
    unsigned char *base;
    const void *src;
    unsigned short *p;
    int i;
    int n;
    int v;

    base = iwram_3001ed0;
    if (L525c[0] > 0)
        goto tail;
loop:
    p = L5260;
    i = p[0];
    n = i + 1;
    p[0] = n;
    v = s8_ARRAY_932__0200bd28[(short)i];
    if (v == -1) {
        p[0] = 0;
        goto loop;
    }
    p[0] = n + 1;
    L525c[0] = s8_ARRAY_932__0200bd28[(short)n];
    src = base + v * 2;
    DMA3_COPY16(src, (void *)0x5000006, 0x24);
tail:
    L525c[0]--;
}
