#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct SndState {
    /* 0x00 */ unsigned char f0;
    /* 0x01 */ unsigned char f1;
    /* 0x02 */ unsigned char f2;
    /* 0x03 */ unsigned char f3;
    /* 0x04 */ unsigned char f4[4];
    /* 0x08 */ unsigned char f8;
    /* 0x09 */ unsigned char f9[2];
    /* 0x0b */ unsigned char fb;
    /* 0x0c */ unsigned char fc[8];
    /* 0x14 */ int f14;
    /* 0x18 */ unsigned char f18[0x10];
    /* 0x28 */ unsigned char *volatile f28;
};

extern struct SndState ewram_2002240;

void Func_80060e8(const void *src)
{
    struct SndState *s = &ewram_2002240;
    unsigned char *p;
    unsigned short *q;
    int sum;
    unsigned int i;

    p = s->f28;
    p[0] = s->fb;
    sum = 0;
    p[1] = s->f2 ^ s->f3;
    *(unsigned short *)(p + 2) = sum;
    DMA3_SET(src, p + 4, 0x84000006);
    q = (unsigned short *)s->f28;
    for (i = 0; i < 14; i++) {
        sum += q[i];
    }
    *(unsigned short *)(s->f28 + 2) = ~sum;
    if (s->f0 != 0) {
        int z = 0;
        REG_TM3CNT_H = z;
    }
    s->f14 = -1;
    if (s->f0 != 0 && s->f8 != 0)
        REG_TM3CNT_H = 0xc0;
}
