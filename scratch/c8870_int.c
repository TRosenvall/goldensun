extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern int __MapActor_SetPos(int a, int x, int y);

void OvlFunc_930_2008870(void)
{
    int e;
    int f;
    int x;
    int y;

    e = 0x15;
    f = 9;
    __Func_8010704(0x55, 9, 1, 1, e, f);
    __Func_808edac(0x64, 0, 0);
    x = 0xac;
    y = 0x98;
    __MapActor_SetPos(0xe, x << 17, y << 16);
}
