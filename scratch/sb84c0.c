extern int Func_8000888(int a, int b);
extern unsigned char **GetBattleActor(int id);
extern int Func_80b7f70(unsigned char *p, int n);
extern int Func_80b7ed8(void);
extern int PhysMove(unsigned char *p, int *q);
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int n);

static inline int call_via_r5(int (*f)(int, int), int a, int b)
{
    register int (*_f)(int, int) __asm__("r5") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\tr5"
        : "=r" (_a)
        : "r" (_f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

int Func_80b84c0(int unit, int *q)
{
    int (*f)(int, int);
    unsigned char *s;
    int *m;
    int v;

    s = *GetBattleActor(unit);
    m = (int *)Func_80b7f70(s, 0);
    s += 8;
    Func_80b7ed8();
    f = Func_8000888;
    v = call_via_r5(f, PhysMove(s, q), m[6]);
    if (Func_80c23c0(_GetUnit(unit)[0x94 << 1]))
        v = call_via_r5(f, v, 0x18);
    else
        v = call_via_r5(f, v, 0x30);
    q[1] -= v;
    return 0;
}
