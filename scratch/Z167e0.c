#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern void Func_80008d8(void *p, int n, int v);

void Func_80167e0(int lines)
{
    char *src;
    char *dst;
    char *cur;
    int n3, n6;
    int stride, cnt, ctl, off;
    int i;
    void (*fp)(void *, int, int);

    n3 = lines * 3;
    dst = (char *)0x6002520;
    n6 = n3 * 2;
    stride = n3 * 8;
    src = (char *)0x6002520 + stride;
    cur = (char *)0x6002500;
    cnt = 0x18 - n6;
    ctl = 0x84000000;
    off = (0x20 - n6) * 4;
    i = 0x1d;
    do {
        DMA3_SET(src, dst, ctl | cnt);
        fp = Func_80008d8;
        fp(cur + off, stride, 0);
        i--;
        cur += 0x80;
        dst += 0x80;
        src += 0x80;
    } while (i >= 0);
}
