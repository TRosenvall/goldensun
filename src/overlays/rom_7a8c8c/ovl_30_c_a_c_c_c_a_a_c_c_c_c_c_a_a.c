extern void __PlaySound(int id);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void OvlFunc_922_2008180(int a, int b, int c);
extern void __WaitFrames(int n);
extern void OvlFunc_922_20092cc(void);

void OvlFunc_922_2008920(void)
{
    __PlaySound(0xf1);
    if (__GetFlag(0x311) != 0) {
        OvlFunc_922_2008180(0xa, 0, 0x30);
        __ClearFlag(0x30b);
        __ClearFlag(0xc3 << 2);
        __SetFlag(0x30d);
        __ClearFlag(0x30e);
    } else if (__GetFlag(0xc4 << 2) != 0) {
        OvlFunc_922_2008180(0xa, 0, 0x20);
        __ClearFlag(0x30b);
        __SetFlag(0xc3 << 2);
        __ClearFlag(0x30d);
        __ClearFlag(0x30e);
    } else {
        OvlFunc_922_2008180(0xa, 0, 0x70);
        __ClearFlag(0x30b);
        __ClearFlag(0xc3 << 2);
        __ClearFlag(0x30d);
        __ClearFlag(0x30e);
    }
    __PlaySound(0x121);
    __WaitFrames(2);
    OvlFunc_922_20092cc();
}
