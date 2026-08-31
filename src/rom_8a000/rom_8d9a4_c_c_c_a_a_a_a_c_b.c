#include "dma.h"

extern int iwram_3001ecc;

void Func_80907b0(int v)
{
    int *p;
    int pat;
    int i;
    int *q;

    p = (int *)iwram_3001ecc;
    DMA3_FILL((void *)0x6002000, 0xf000f000, 0x500);
    if (v != -1) {
        pat = 0;
        i = 7;
        do {
            pat <<= 4;
            i--;
            pat |= v;
        } while (i >= 0);
        q = (int *)((char *)p + 0xa1 * 8);
        i = 7;
        do {
            i--;
            *q++ = pat;
        } while (i >= 0);
        DMA3_COPY((const void *)((char *)p + 0xa1 * 8), (void *)0x6000000, 32);
    }
}
