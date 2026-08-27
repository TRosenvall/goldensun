extern int L2f80 __asm__(".L2f80");
extern int __GetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void OvlFunc_948_200a0c4(int a, int b);

void OvlFunc_948_2009fd8(void)
{
    int x0, y0, x1, y1, x2, y2, x3, y3;

    L2f80++;
    if (L2f80 > 0x10)
        L2f80 = 0;
    switch (L2f80) {
    case 12:
        x0 = 0xe8 << 16;
        y0 = 0xda << 18;
        if (__GetFlag(0xee7) == 0)
            __MapActor_SetPos(0x8, x0, y0);
        x1 = 0x94 << 17;
        y1 = 0xce << 18;
        if (__GetFlag(0xee8) == 0)
            __MapActor_SetPos(0x9, x1, y1);
        x2 = 0xa4 << 17;
        y2 = 0xbe << 18;
        if (__GetFlag(0xee9) == 0)
            __MapActor_SetPos(0xa, x2, y2);
        x3 = 0xb4 << 17;
        y3 = 0xda << 18;
        if (__GetFlag(0xeea) == 0)
            __MapActor_SetPos(0xb, x3, y3);
        break;
    case 10: OvlFunc_948_200a0c4(8, 0); break;
    case 8:  OvlFunc_948_200a0c4(9, 0); break;
    case 6:  OvlFunc_948_200a0c4(0xa, 0); break;
    case 4:  OvlFunc_948_200a0c4(0xb, 0); break;
    case 2:  OvlFunc_948_200a0c4(0xc, 1); break;
    }
}
