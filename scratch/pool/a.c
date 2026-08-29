#include "gba/io.h"
extern void WaitFrames(int n);
void Func_80c0e38(void)
{
    int i; vu16 *dst; vu16 *cnt; 
    cnt = &REG_BLDCNT;
    
    dst = &REG_BLDALPHA;
    *cnt = 0x2044;
    i = 1;
    do { *dst = 0x1010 - i; WaitFrames(1); i += 2; } while (i <= 0x10);
}
void Func_80c0e70(void)
{
    int i; vu16 *dst; vu16 *cnt; 
    cnt = &REG_BLDCNT;
    
    dst = &REG_BLDALPHA;
    *cnt = 0x2044;
    i = 1;
    do { *dst = i + 0x1000; WaitFrames(1); i += 2; } while (i <= 0x10);
}
