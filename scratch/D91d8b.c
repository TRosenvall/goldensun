extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_948_20091d8(void)
{
    int s1, s2, x, y;

    s1 = 0x19;
    s2 = 0x30;
    __Func_80105d4(0x18, 0x30, 1, 2, s1, s2);
    x = 0x80 << 12;
    y = 0x80 << 12;
    __MapActor_SetPos(0xc, x, y);
}
