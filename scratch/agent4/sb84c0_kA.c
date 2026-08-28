extern int Func_8000888(int a, int b);
extern unsigned char **GetBattleActor(int id);
extern int Func_80b7f70(unsigned char *p, int n);
extern int Func_80b7ed8(void);
extern int PhysMove(unsigned char *p, int *q);
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int n);

#define CALL_VIA_R5(res, aa, bb) do {                                    \
    register int _a __asm__("r0") = (aa);                                \
    register int _b __asm__("r1") = (bb);                                \
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\tr5"         \
        : "=r" (_a) : "r" (f), "0" (_a), "r" (_b)                        \
        : "memory", "lr", "r12");                                        \
    (res) = _a;                                                          \
} while (0)

int Func_80b84c0(int unit, int *q)
{
    register int (*f)(int, int) __asm__("r5");
    int v;
    int k;
    unsigned char *s;
    int *m;

    s = *GetBattleActor(unit);
    m = (int *)Func_80b7f70(s, 0);
    s += 8;
    Func_80b7ed8();
    v = PhysMove(s, q);
    f = Func_8000888;
    CALL_VIA_R5(v, v, m[6]);
    if (Func_80c23c0(_GetUnit(unit)[0x94 << 1]))
        k = 0x18;
    else
        k = 0x30;
    CALL_VIA_R5(v, v, k);
    q[1] -= v;
    return 0;
}
