extern int _Func_8019000(int a, int b, int c, int d, int e);

void Func_80a8cc0(int a, int b, int c, int d)
{
    int base;

    base = 0xf281 + d * 2;
    _Func_8019000(a, (0x80 << 3) | base, b, c, 0);
    _Func_8019000(a, 0xf280 + d * 2, b + 1, c, 0);
    _Func_8019000(a, base, b + 2, c, 0);
}
