extern int L2f80 __asm__(".L2f80");
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_948_200a0c4(int a, int b);

int OvlFunc_948_2009fd8(void)
{
    L2f80++;
    if (L2f80 > 0x10)
        L2f80 = 0;
    switch (L2f80) {
    case 12:
        if (__GetFlag(0xee7) == 0)
            __MapActor_SetPos(8, 0xe8 << 16, 0xda << 18);
        if (__GetFlag(0xee8) == 0)
            __MapActor_SetPos(9, 0x94 << 17, 0xce << 18);
        if (__GetFlag(0xee9) == 0)
            __MapActor_SetPos(0xa, 0xa4 << 17, 0xbe << 18);
        if (__GetFlag(0xeea) == 0)
            __MapActor_SetPos(0xb, 0xb4 << 17, 0xda << 18);
        break;
    case 10: OvlFunc_948_200a0c4(8, 0); break;
    case 8:  OvlFunc_948_200a0c4(9, 0); break;
    case 6:  OvlFunc_948_200a0c4(0xa, 0); break;
    case 4:  OvlFunc_948_200a0c4(0xb, 0); break;
    case 2:  OvlFunc_948_200a0c4(0xc, 1); break;
    }
    return 0;
}
