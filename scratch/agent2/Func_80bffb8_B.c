#include "gba/types.h"
#include "gba/io.h"

extern void Func_8003b70(int a);
extern int Random(void);
extern void WaitFrames(int n);

int Func_80bffb8(void)
{
    vu16 s0;
    vu16 s1;
    vu16 s2;
    vu16 s3;
    vu16 *p;
    int i;

    p = &REG_BG0CNT;
    s3 = *p;
    *p |= 0x40;
    p++;
    s2 = *p;
    *p |= 0x40;
    p++;
    s1 = *p;
    *p |= 0x40;
    p++;
    s0 = *p;
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
    *p = s3;
    p++;
    *p = s2;
    p++;
    *p = s1;
    p++;
    *p = s0;
    return 0;
}
