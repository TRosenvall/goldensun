extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __WaitFrames(int n);

void OvlFunc_941_2008384(void)
{
    int a;
    int b;
    int c;
    int d;
    int e;

    a = 0x15;
    __Func_80105d4(0x29, 0x57, 2, 5, a, 0x3b);
    __WaitFrames(4);
    b = 0x18;
    c = 0x3e;
    __Func_80105d4(2, 0x5d, 1, 1, b, c);
    d = 0x37;
    __Func_80105d4(2, 0x5e, 1, 1, a, d);
    e = 0x3a;
    __Func_80105d4(0x2b, 0x57, 2, 5, a, e);
    __WaitFrames(4);
    __Func_80105d4(3, 0x5d, 1, 1, b, c);
    __Func_80105d4(1, 0x5e, 1, 1, a, d);
    __Func_80105d4(0x29, 0x57, 2, 5, a, e);
    __Func_8010704(0x15, 0xb, 2, 2, a, 0xd);
    __Func_8010704(0x13, 0x11, 1, 1, a, 0xe);
}
