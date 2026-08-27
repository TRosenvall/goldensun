#include "gba/types.h"
#include "gba/io.h"

extern char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __Func_808fe38(int n);
extern void OvlFunc_957_2008a54(void);

void OvlFunc_957_2008b30(void)
{
    char *p;
    char *q;
    int five;

    p = iwram_3001ebc;
    *(int *)(p + (0xe0 << 1)) = 0x100;
    *(int *)(p + 0x1c8) = 0x18;
    __WaitFrames(1);
    __Func_808fe38(0x4d);
    five = 5;
    q = ((char **)&iwram_3001ebc)[4];
    *(short *)(q + 0x52a) = five;
    if (__GetFlag(0x201)) {
        *(short *)(q + 0x534) = 0x1d1d;
        *(short *)(q + 0x536) = 0x3f;
        OvlFunc_957_2008a54();
    } else {
        *(short *)(q + 0x534) = 0x3f3f;
        *(short *)(q + 0x536) = 0x1f;
        REG_BLDCNT = 0x3f42;
        REG_BLDALPHA = 0xc04;
    }
}
