extern int __GetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_948_200a0c4(int a, int b);
extern void OvlFunc_948_2008f40(int n);
extern void OvlFunc_948_2008fdc(int n);

void OvlFunc_948_200a188(void)
{
    int x, y;

    __WaitFrames(1);
    OvlFunc_948_200a0c4(0xc, 0xf3);
    OvlFunc_948_200a0c4(0xb, 0xf4);
    OvlFunc_948_200a0c4(0xa, 0xf4);
    OvlFunc_948_200a0c4(9, 0xf4);
    OvlFunc_948_200a0c4(8, 0xf4);
    x = 0xe8 << 16;
    y = 0xda << 18;
    if (__GetFlag(0xee7) == 0)
        __MapActor_SetPos(8, x, y);
    x = 0x94 << 17;
    y = 0xce << 18;
    if (__GetFlag(0xee8) == 0)
        __MapActor_SetPos(9, x, y);
    x = 0xa4 << 17;
    y = 0xbe << 18;
    if (__GetFlag(0xee9) == 0)
        __MapActor_SetPos(0xa, x, y);
    x = 0xb4 << 17;
    y = 0xda << 18;
    if (__GetFlag(0xeea) == 0)
        __MapActor_SetPos(0xb, x, y);
    if (__GetFlag(0x9c << 4))
        OvlFunc_948_2008f40(0);
    if (__GetFlag(0x9c1))
        OvlFunc_948_2008f40(1);
    if (__GetFlag(0x9c2))
        OvlFunc_948_2008f40(2);
    if (__GetFlag(0x9c3))
        OvlFunc_948_2008f40(3);
    if (__GetFlag(0x9c4))
        OvlFunc_948_2008fdc(0);
}
