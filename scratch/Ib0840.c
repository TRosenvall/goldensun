#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern char *iwram_3001ebc[];
extern void _Func_8091200(int a, int b);
extern void _Func_8091254(int n);

void Func_80b0840(int index)
{
    char *base;
    char *s;

    base = iwram_3001ebc[5];
    s = base + (0xe0 << 4);
    DMA3_SET(s, iwram_3001ebc[0] + 0x236, 0x84000150);
    DMA3_SET(s, base + (0xe0 << 2), 0x840002a0);
    _Func_8091200(index, 1);
    _Func_8091254(0x10);
}
