#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001e74;
extern signed char Lc5c10[] __asm__(".Lc5c10");

void Func_80c1084(void)
{
    char *p;
    unsigned short *q;
    unsigned short n;
    int v;

    p = iwram_3001e74;
    if (p == 0)
        return;
    if (*(unsigned short *)(p + (0xca << 3)) == 0)
        return;
    REG_BLDCNT = 0x3f90;
    REG_BLDALPHA = 0x10;
    p += 0x64e;
    q = (unsigned short *)p;
    REG_BLDY = Lc5c10[*q];
    n = *q;
    v = (n + 1) & 0xf;
    if (n > 0xe)
        v |= 0x10;
    *q = v;
}
