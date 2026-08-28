extern int Func_8000948(int);
extern int Func_8000888(int, int);
extern int Func_80008ac(int, int);
extern int __FastIntSqrtFP1616_RAM(int);
extern unsigned int iwram_3001e40;

#define CALL_VIA(f, a, b)                                       \
    ({                                                          \
        register int _a __asm__("r0") = (a);                    \
        register int _b __asm__("r1") = (b);                    \
        __asm__ volatile (                                      \
            "\t.align\t2, 0\n"                                  \
            "\tmov\tr12, pc\n"                                  \
            "\tbx\t%1"                                          \
            : "=r" (_a)                                         \
            : "r" (f), "0" (_a), "r" (_b)                       \
            : "memory", "r2", "r12");                           \
        _a;                                                     \
    })

#define CALL_VIA_R3(f, a, b)                                    \
    ({                                                          \
        register int (*_f)(int, int) __asm__("r3") = (f);       \
        register int _a __asm__("r0") = (a);                    \
        register int _b __asm__("r1") = (b);                    \
        __asm__ volatile (                                      \
            "\t.align\t2, 0\n"                                  \
            "\tmov\tr12, pc\n"                                  \
            "\tbx\tr3"                                          \
            : "=r" (_a)                                         \
            : "r" (_f), "0" (_a), "r" (_b)                      \
            : "memory", "r2", "r12");                           \
        _a;                                                     \
    })

void OvlFunc_923_2009cb4(int e)
{
    int *p;
    int B, A, dx, dy, d, lim;
    int (*g)(int);
    int (*g2)(int, int);
    int (*h)(int, int);
    int t1, t2;
    int *q;
    int v;

    *(int *)(e + 0x30) = 0x20000;
    *(int *)(e + 0x34) = 0x10000;
    p = *(int **)(e + 0x68);
    A = p[2];
    B = p[4];
    *(int *)(e + 0x38) = 0x80000000;
    *(int *)(e + 0x3c) = 0x80000000;
    *(int *)(e + 0x40) = 0x80000000;
    dx = (A - *(int *)(e + 8)) / 0x10000;
    dy = (B - *(int *)(e + 0x10)) / 0x10000;
    t2 = dx * dx + dy * dy;
    g = Func_8000948;
    t1 = g(t2);
    dx = A - *(int *)(e + 8);
    dy = B - *(int *)(e + 0x10);
    d = t1 << 16;
    if (d < 0x400000) {
        t1 = CALL_VIA(Func_8000888, dx, dx);
        t2 = CALL_VIA(Func_8000888, dy, dy);
        t1 += t2;
        d = __FastIntSqrtFP1616_RAM(t1);
    }
    lim = d / 8;
    if (lim > *(int *)(e + 0x30))
        lim = *(int *)(e + 0x30);
    if (d < 0x4000) {
        *(int *)(e + 8) = A;
        *(int *)(e + 0x10) = B;
    } else {
        if (d > lim) {
            g2 = Func_80008ac;
            dx = CALL_VIA(Func_8000888, g2(d, dx), lim);
            dy = CALL_VIA(Func_8000888, g2(d, dy), lim);
        }
        *(int *)(e + 8) += dx;
        *(int *)(e + 0x10) += dy;
    }
    v = (iwram_3001e40 >> 1) & 1;
    q = *(int **)(e + 0x50);
    *(char *)(q[10] + 5) = v * 7;
    *((char *)q + 0x25) = 1;
}
