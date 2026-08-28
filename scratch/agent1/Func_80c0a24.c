extern int Func_8000888(int, int);
extern int Func_80008ac(int, int);
extern int _GetFlag(int);
extern int iwram_3001f00;
extern short iwram_3001ad0[];

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
            : "memory", "r3", "r12");                           \
        _a;                                                     \
    })

void Func_80c0a24(int a1, int a2, int a3, int a4, int a5)
{
    int *g;
    int *A;
    int *B;
    int *C;
    char *w;
    int local0, sh, v, d, x, y, r, i, zero, u;
    unsigned int n;
    unsigned short *dst;
    int (*f2)(int, int);

    g = &iwram_3001f00;
    A = (int *)g[-34];
    B = (int *)g[0];
    C = (int *)g[-32];
    local0 = 0;
    sh = 0x800;
    if (a5 >= 0x10000) {
        local0 = 0x2000;
        sh = 0x6800 + -(int)*(short *)((char *)C + 0x36) * 3;
    }
    if (A == 0)
        return;
    if (B[2] == 1 || B[3] == 1) {
        if (B[4] == 0)
            iwram_3001ad0[2] = sh >> 8;
    }
    if (B[2] != 2)
        return;
    dst = (unsigned short *)((char *)A + (A[0] ^ 1) * 0x140);
    f2 = Func_80008ac;
    r = f2(a5, 0x10000);
    w = (char *)A + 0x10;
    v = r >> 8;
    zero = 0;
    *(short *)w = v;
    *(short *)(w + 2) = zero;
    *(short *)(w + 4) = zero;
    *(short *)(w + 6) = v;
    d = a5;
    d -= 0x10000;
    dst += 0x10;
    {
        register int (*h)(int, int) __asm__("r4") = Func_8000888;

        x = CALL_VIA(h, a1, d);
        x = CALL_VIA(h, r, x);
        y = CALL_VIA(h, a2, d);
        y = CALL_VIA(h, r, y);
    }
    x += 0x7fff;
    x >>= 8;
    x += a3;
    x += sh;
    *(int *)(w + 8) = x;
    y += 0x7fff;
    y >>= 8;
    y += a4;
    y -= 0x1000;
    *(int *)(w + 0xc) = y;
    n = (f2((short)v, 0x4000 - y) >> 16) + 1;
    i = 0;
    if (_GetFlag(0x16b) == 0) {
        do {
            i++;
            *dst++ = 0x3f8e;
        } while ((unsigned)i <= 0xf);
    }
    if (n > 0x88)
        n = 0x88;
    if ((unsigned)i < n) {
        u = (unsigned short)local0 | 0x478a;
        do {
            i++;
            *dst++ = u;
        } while ((unsigned)i < n);
    }
    if ((unsigned)i <= 0x87) {
        u = (unsigned short)local0 | 0x478e;
        do {
            i++;
            *dst++ = u;
        } while ((unsigned)i <= 0x87);
    }
    if ((unsigned)i <= 0x9f) {
        do {
            i++;
            *dst++ = 0x3f8e;
        } while ((unsigned)i <= 0x9f);
    }
    A[0] = A[0] ^ 1;
}
