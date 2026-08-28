extern int iwram_3001e40;
extern int Func_8000888(int a, int b);
extern void __Actor_SetColorswap(void *a, int n);
extern int __sin(int n);
extern void __vec3_translate(int a, int b, void *v);

static inline int call_via_r3(int a, int b)
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
        : "memory", "r12"
    );
    return _a;
}

void OvlFunc_881_20081c4(unsigned char *a)
{
    short *p;
    int t;
    int r;

    if ((iwram_3001e40 & 2) != 0)
        __Actor_SetColorswap(a, 0xa);
    else
        __Actor_SetColorswap(a, 7);
    if (*(short *)(a + 0x66) == 0) {
        *(int *)(a + 8) = 0x15d00000;
        p = (short *)(a + 0x64);
        r = call_via_r3(__sin(*p << 3), 0x80 << 11);
        t = 0x80 << 13;
        *(int *)(a + 0xc) = r + t;
        *(int *)(a + 0x10) = 0xa6 << 19;
        __vec3_translate(t, *p, a + 8);
        *(short *)(a + 6) = *(unsigned short *)p + (0x80 << 7);
        *(unsigned short *)p = *(unsigned short *)p + (0x80 << 3);
    }
}
