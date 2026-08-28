#include "gba/types.h"
#include "gba/io.h"

extern unsigned char *iwram_3001e70;

void Func_8090584(void)
{
    unsigned int vc;
    unsigned char *g;
    unsigned short *p;

    vc = REG_VCOUNT;
    g = iwram_3001e70;
    while (1) {
        p = (unsigned short *)(g + 0x108);
        switch (*p) {
        case 3:
            if (vc < *(unsigned short *)(g + 0x104))
                return;
            REG_DISPCNT = (REG_DISPCNT & 0xfff8) | 2;
            *p = 9;
            return;
        case 2:
            if (vc < *(unsigned short *)(g + 0x106))
                return;
            REG_DISPCNT = REG_DISPCNT & 0xfff8;
            *p = 9;
            return;
        case 1:
            if (vc >= *(unsigned short *)(g + 0x104)) {
                REG_DISPCNT = (REG_DISPCNT & 0xfff8) | 2;
                *p = *p + 1;
            } else {
                if (vc < *(unsigned short *)(g + 0x106))
                    return;
                REG_DISPCNT = REG_DISPCNT & 0xfff8;
                *p = 3;
            }
            break;
        case 0:
            if (vc > 0x9e)
                return;
            *p = 1;
            break;
        default:
            return;
        }
    }
}
