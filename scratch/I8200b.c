#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001ebc[];
extern void __Func_808fe38(int n);
extern void OvlFunc_919_20082e0(void);

int OvlFunc_919_2008200(void)
{
    char *p;
    unsigned short *q;
    int *d;
    int off;
    int v;

    p = iwram_3001ebc[0];
    off = 0xe0 << 1;
    d = (int *)(p + off);
    *d = off - 0xc0;
    __Func_808fe38(9);
    REG_BLDCNT = 0x3f42;
    REG_BLDALPHA = 0xc04;
    p = iwram_3001ebc[4];
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
    OvlFunc_919_20082e0();
    return 0;
}
