#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001ecc;
extern void __Func_808fe38(int n);

void OvlFunc_919_200815c(void)
{
    char *p;
    unsigned short *q;
    int off;
    int v;

    __Func_808fe38(9);
    REG_BLDCNT = 0x3f42;
    REG_BLDALPHA = 0xc04;
    p = iwram_3001ecc;
    off = 0x534;
    q = (unsigned short *)(p + off);
    v = 0x3f3f;
    *q = v;
    off = 0x536;
    q = (unsigned short *)(p + off);
    v = 0x1f;
    *q = v;
    off = 0x52a;
    p += off;
    v = 0xa;
    *(unsigned short *)p = v;
}
