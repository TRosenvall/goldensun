extern int Func_8000888(int a, int b);

static inline int via_r3(int a, int b)
{
    register int (*_f)(int, int) __asm__("r3") = Func_8000888;
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

int probe(int n, unsigned short *src, unsigned short *dst)
{
    int i, s = 0;
    for (i = 0; i < n; i++) {
        s += via_r3(src[i] << 16, s >> 4);
        s += via_r3(src[i] << 8, s >> 4);
        s += via_r3(src[i] << 4, s >> 4);
    }
    return s;
}
