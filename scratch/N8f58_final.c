extern int __GetFlag(int id);
extern void OvlFunc_918_2008918(void);
extern void __WaitFrames(int n);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_918_2008f58(int a)
{
    int n6;
    int n5;
    int p;
    int q;

    if (a != 0 && __GetFlag(0x109) == 0)
        OvlFunc_918_2008918();
    __WaitFrames(1);
    if (__GetFlag(0x844)) {
        n6 = 0xa;
        __Func_80105d4(0x79, 0x22, 3, 1, 0x5d, n6);
        n5 = 0x1e;
        __Func_80105d4(0x2e, 0x26, 1, 1, n5, 0x2b);
        __Func_8010704(0, 0, 1, 2, n5, 9);
        __Func_8010704(0x1a, 3, 1, 2, n6, 8);
        __Func_80105d4(0x1a, 0x23, 1, 4, n6, 0x28);
    } else {
        p = 0xa;
        q = 8;
        __Func_8010704(0xb, 8, 1, 2, p, q);
    }
}
