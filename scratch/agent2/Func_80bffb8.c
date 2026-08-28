#include "gba/types.h"
#include "gba/io.h"

extern void Func_8003b70(int a);
extern int Random(void);
extern void WaitFrames(int n);

int Func_80bffb8(void)
{
    u16 save[4];
    vu16 *p;
    int i;

    p = &REG_BG0CNT;
    save[3] = *p;
    *p |= 0x40;
    p++;
    save[2] = *p;
    *p |= 0x40;
    p++;
    save[1] = *p;
    *p |= 0x40;
    p++;
    save[0] = *p;
    *p |= 0x40;
    p += 0x21;
    *p = 0x3eee;
    Func_8003b70(0x10);
    for (i = 0; i <= 0xf; i++) {
        Random();
        Random();
        Random();
        Random();
        REG_MOSAIC = (i << 8) | i;
        WaitFrames(1);
    }
    REG_DISPCNT = 1;
    WaitFrames(4);
    p = &REG_BG0CNT;
    *p = save[3];
    p++;
    *p = save[2];
    p++;
    *p = save[1];
    p++;
    *p = save[0];
    return 0;
}
