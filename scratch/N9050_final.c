extern int __GetFlag(int id);
extern void OvlFunc_922_2009004(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_922_2009050(void)
{
    int n;
    int p1, q1, p2, q2, p3, q3, p4, q4, p5, q5;

    n = 8;
    __Func_8010704(8, 0x2a, 0xf, 5, n, 0x1d);
    if (__GetFlag(0x301)) {
        OvlFunc_922_2009004(8, 0x16, 0x1f);
        __Func_8010704(9, 0x1e, 1, 3, n, 0x1e);
    } else {
        OvlFunc_922_2009004(8, 8, 0x1f);
        p1 = 0x16;
        q1 = 0x1e;
        __Func_8010704(9, 0x1e, 1, 3, p1, q1);
    }
    if (__GetFlag(0x302)) {
        OvlFunc_922_2009004(9, 0xc, 0x1d);
        p2 = 0xb;
        q2 = 0x21;
        __Func_8010704(0xe, 0x21, 3, 1, p2, q2);
    } else {
        OvlFunc_922_2009004(9, 0xc, 0x21);
        p3 = 0xb;
        q3 = 0x1d;
        __Func_8010704(0xe, 0x1d, 3, 1, p3, q3);
    }
    if (__GetFlag(0x303)) {
        OvlFunc_922_2009004(0xa, 0x12, 0x1d);
        p4 = 0x11;
        q4 = 0x21;
        __Func_8010704(0xe, 0x21, 3, 1, p4, q4);
    } else {
        OvlFunc_922_2009004(0xa, 0x12, 0x21);
        p5 = 0x11;
        q5 = 0x1d;
        __Func_8010704(0xe, 0x1d, 3, 1, p5, q5);
    }
}
