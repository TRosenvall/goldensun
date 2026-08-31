extern int iwram_3001e70;
extern int iwram_3001af4;
extern void *galloc_ewram(int a, int b);
extern int Func_8000888(int a, int b);
extern void StopTask(void *f);
int Func_80935d4(void);

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

int Func_80935d4(void)
{
    char *p;
    char *t;
    short *e;
    unsigned short *c;
    int *a;
    int *b;
    int av;
    int st;
    int d;
    int m;
    int r;

    p = (char *)iwram_3001e70;
    t = *(char **)((char *)galloc_ewram(0x1b, 0xccc) + (0xf0 << 1));
    st = t[0x5b];
    if (st != 0)
        return;
    e = (short *)(p + (0xd6 << 2));
    if (*(short *)((char *)e + (unsigned int)0) == 0)
        return;
    a = (int *)(p + (0xd4 << 2));
    b = (int *)(p + (0xd5 << 2));
    d = *b - *a;
    c = (unsigned short *)(p + 0x35a);
    *c = *c + 1;
    m = (short)*c;
    r = (d * m) / *(short *)((char *)e + (unsigned int)0);
    av = *a;
    *(int *)(p + (0xd3 << 2)) = call_via_r4(*(int *)(p + (0xd2 << 2)), av + r);
    iwram_3001af4 = *(unsigned short *)(p + (0x8c << 1)) + 1;
    if (*(short *)((char *)c + (unsigned int)0)
     == *(short *)((char *)e + (unsigned int)0)) {
        *(unsigned short *)e = st;
        StopTask(Func_80935d4);
    }
}
