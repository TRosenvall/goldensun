extern int Func_8019000(int a, int b, int c, int d, int e);

int Func_80218dc(int a, int b, int c, int d)
{
    int base;

    base = 0xf315 + d * 2;
    Func_8019000(a, (0x80 << 3) | base, b, c, 0);
    Func_8019000(a, 0xf314 + d * 2, b + 1, c, 0);
    return Func_8019000(a, base, b + 2, c, 0);
}
