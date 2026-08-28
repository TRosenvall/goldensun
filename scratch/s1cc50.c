extern int Func_8000888(int a, int b);

static inline int call_via_r4(int a, int b)
{
    register int (*_f)(int, int) __asm__("r4") = Func_8000888;
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

int Func_801cc50(short *p, int a, int b, int c)
{
    int r, g, bl;

    r  = call_via_r4(p[0] << 16, a) >> 16;
    g  = call_via_r4(p[1] << 16, b) >> 16;
    bl = call_via_r4(p[2] << 16, c) >> 16;
    if (r < 0)
        r = 0;
    if (r > 0x1f)
        r = 0x1f;
    if (g < 0)
        g = 0;
    if (g > 0x1f)
        g = 0x1f;
    if (bl < 0)
        bl = 0;
    if (bl > 0x1f)
        bl = 0x1f;
    return r + ((bl << 10) + (g << 5));
}
