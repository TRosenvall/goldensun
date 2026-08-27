extern int Func_80008ac(int a, int b);
extern int Func_8000888(int a, int b);

static inline int call_via_r4(int (*f)(int, int), int a, int b)
{
    register int (*_f)(int, int) __asm__("r4") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\tr4"
        : "=r" (_a)
        : "r" (_f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

int Func_8097a10(int base, int v)
{
    int r;
    int (*g)(int, int);

    if (v == 0)
        return 0;
    if ((v & (0xf0 << 24)) != 0)
        v = -v;
    g = Func_80008ac;
    r = g(v, base);
    return base - call_via_r4(Func_8000888, r & 0xffff0000, v);
}
