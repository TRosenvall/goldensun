extern int sin(int);
extern int Func_8000888(int, int);

static inline int call_via_r3(int (*f)(int, int), int a, int b)
{
    register int (*_f)(int, int) __asm__("r3") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\tr3"
        : "=r" (_a)
        : "r" (_f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

void Func_80992f0(int e)
{
    short *p = (short *)(e + 0x64);
    int v;

    *(int *)(e + 8) = *(int *)(e + 0x38)
        + call_via_r3(Func_8000888, 0x40000, sin(*p << 9));
    *p = *p + 1;
    v = *p + 0x80;
    *p = v % 128;
}
