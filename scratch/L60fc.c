#include "dma.h"

extern char *iwram_3001e8c;

void Func_80160fc(void)
{
    char *b;
    char *s;
    char *d;
    int off;
    int m;
    int one;
    int step;

    b = iwram_3001e8c;
    off = 0xea6;
    if (*(unsigned char *)(b + off) != 0)
        return;
    off -= 3;
    m = *(unsigned char *)(b + off);
    if (m == 0)
        return;
    d = (char *)0x6002000;
    if ((m & 1) != 0)
        m = 0x3f;
    m = (m & 0x3f) >> 1;
    one = 1;
    step = 0x80 << 1;
    s = b;
loop:
    if ((m & one) != 0)
        DMA3_COPY(s, d, 0x100);
    m >>= 1;
    s += step;
    d += step;
    if (m != 0)
        goto loop;
    *(unsigned char *)(b + 0xea3) = m;
}
