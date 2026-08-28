#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern unsigned char ewram_2002240[];

void Func_80060e8(const void *src)
{
    unsigned char *p;
    unsigned short *q;
    int sum;
    int i;

    p = *(unsigned char **)(ewram_2002240 + 0x28);
    p[0] = ewram_2002240[0xb];
    sum = 0;
    p[1] = ewram_2002240[2] ^ ewram_2002240[3];
    *(unsigned short *)(p + 2) = sum;
    DMA3_SET(src, p + 4, 0x84000006);
    q = *(unsigned short **)(ewram_2002240 + 0x28);
    for (i = 0; i < 14; i++) {
        sum += *q;
        q++;
    }
    *(unsigned short *)(*(unsigned char **)(ewram_2002240 + 0x28) + 2) = ~sum;
    if (ewram_2002240[0] != 0)
        REG_TM3CNT_H = 0;
    *(int *)(ewram_2002240 + 0x14) = -1;
    if (ewram_2002240[0] != 0 && ewram_2002240[8] != 0)
        REG_TM3CNT_H = 0xc0;
}
