#include "gba/types.h"
#include "gba/io.h"

extern unsigned char *iwram_3001e70;

void Func_8090584(void)
{
    unsigned int vc;
    unsigned char *g;
    unsigned short *p;
    int n9 = 9;
    int n3 = 3;
    int n1 = 1;

    vc = REG_VCOUNT;
    g = iwram_3001e70;
top:
    p = (unsigned short *)(g + 0x108);
    switch (*p) {
    case 3:
        if (vc < *(unsigned short *)(g + 0x104))
            return;
        REG_DISPCNT = (REG_DISPCNT & 0xfff8) | 2;
        *p = n9;
        return;
    case 2:
        if (vc < *(unsigned short *)(g + 0x106))
            return;
        REG_DISPCNT = REG_DISPCNT & 0xfff8;
        *p = n9;
        return;
    case 1:
        if (vc >= *(unsigned short *)(g + 0x104)) {
            REG_DISPCNT = (REG_DISPCNT & 0xfff8) | 2;
            *p = *p + 1;
            goto top;
        }
        if (vc < *(unsigned short *)(g + 0x106))
            return;
        REG_DISPCNT = REG_DISPCNT & 0xfff8;
        *p = n3;
        goto top;
    case 0:
        if (vc > 0x9e)
            return;
        *p = n1;
        goto top;
    }
}
