extern unsigned char L773c[] __asm__(".L773c");
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_2008d54(int n)
{
    unsigned char *tb;
    int off;
    int a;
    int b;
    int c;

    tb = L773c;
    off = n * 8;
    a = *(int *)(tb + off);
    off += 4;
    b = *(int *)(tb + off);
    __Func_80105d4(0, 0x4d, 1, 3, a, b);
    __Func_80105d4(1, 0x4d, 1, 1, a + 1, b);
    c = b - 0x2c;
    __Func_8010704(a, b - 0x2d, 1, 1, a, c);
    if (n == 1)
        __Func_8010704(a, c, 1, 1, a, b - 0x2b);
}
