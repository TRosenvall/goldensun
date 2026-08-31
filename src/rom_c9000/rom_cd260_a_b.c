#include "gba/io.h"

extern int iwram_3001eec;

void Func_80cd418(void)
{
    char *p;

    p = (char *)iwram_3001eec;
    REG_WIN0H = *(unsigned short *)(p + 0x77bc);
    REG_WIN0V = *(unsigned short *)(p + 0x77be);
    REG_WIN1H = *(unsigned short *)(p + 0x77c0);
    REG_WIN1V = *(unsigned short *)(p + 0x77c2);
    REG_WININ = *(unsigned short *)(p + 0x77c4);
    REG_WINOUT = *(unsigned short *)(p + 0x77c6);
    REG_DISPCNT = *(unsigned short *)(p + 0x77c8);
    REG_BLDCNT = *(unsigned short *)(p + 0x77ca);
    REG_BLDALPHA = *(unsigned short *)(p + 0x77cc);
}
