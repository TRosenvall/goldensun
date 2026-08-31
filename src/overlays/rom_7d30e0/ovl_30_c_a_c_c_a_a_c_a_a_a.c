extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_2008f40(int arg)
{
    int s1;
    int s2;
    int s3;
    int m1;
    int m2;
    int e;
    int t;
    int a;
    int b;

    s1 = 0x80 << 10;
    s2 = 0x80 << 10;
    s3 = 0x80 << 9;
    m1 = -1;
    m2 = -1;
    e = 0xe666;
    t = (arg & 0xff) << 2;
    a = t + 0x4d;
    b = t + 0xd;
    if (arg & (0x80 << 1)) {
        __PlaySound(0x9d);
        __Func_8012330(s1, s2, s3);
        __Func_8012330(m1, m2, e);
        __Func_80105d4(0x4f, 0x1d, 1, 3, a, 0x28);
        __WaitFrames(0x28);
    }
    __Func_80105d4(0x50, 0x1d, 1, 3, a, 0x28);
    __Func_8010704(b, 0x28, 1, 1, b, 0x29);
    __Func_8010704(b, 0x28, 1, 1, b, 0x2a);
}

void OvlFunc_948_2008fdc(int flags)
{
    int s1;
    int s2;
    int s3;
    int m1;
    int m2;
    int e;
    int p1;
    int p2;
    int q1;
    int q2;
    int t;

    s1 = 0x80 << 10;
    s2 = 0x80 << 10;
    s3 = 0x80 << 9;
    m1 = -1;
    m2 = -1;
    e = 0xe666;
    if (flags & (0x80 << 1)) {
        __PlaySound(0x9d);
        __Func_8012330(s1, s2, s3);
        __Func_8012330(m1, m2, e);
        p1 = 0x46;
        p2 = 0x31;
        __Func_80105d4(0x54, 0x1d, 1, 3, p1, p2);
        __WaitFrames(0x3c);
    }
    q1 = 0x46;
    q2 = 0x31;
    __Func_80105d4(0x55, 0x1d, 1, 3, q1, q2);
    t = 6;
    __Func_8010704(6, 0x31, 1, 1, t, 0x32);
    __Func_8010704(6, 0x31, 1, 1, t, 0x33);
}
