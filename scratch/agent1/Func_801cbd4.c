extern int Func_8000888(int, int);

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

unsigned short Func_801cbd4(int rec, int a, int b, int c)
{
    int x, y, z;

    x = call_via_r4(Func_8000888, *(unsigned short *)(rec + 0x576) << 16, a) >> 16;
    y = call_via_r4(Func_8000888, *(unsigned short *)(rec + 0x578) << 16, b) >> 16;
    z = call_via_r4(Func_8000888, *(unsigned short *)(rec + 0x57a) << 16, c) >> 16;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (z < 0)
        z = 0;
    if (x > 31)
        x = 31;
    if (y > 31)
        y = 31;
    if (z > 31)
        z = 31;
    return x + (y << 5) + (z << 10);
}
