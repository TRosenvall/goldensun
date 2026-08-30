extern int Func_8000888(int, int);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "lr", "r12"
    );
    return _a;
}

extern int *_GetBattleActor(int rec);
extern int _Func_80b7f70(int a, int b);
extern void InitMatrixStack(void);
extern void MatrixSetLook(int a, int b);
extern int PhysMove(int a, int b);
extern int _Func_80b8530(int rec);
extern void *iwram_3001e80;

int Func_80e3994(int rec, int out)
{
    int view;
    int *p;
    int q;
    int m;
    int t;
    int u;

    view = (int)iwram_3001e80;
    p = _GetBattleActor(rec);
    q = *p;
    m = _Func_80b7f70(q, 0);
    InitMatrixStack();
    MatrixSetLook(view, view + 0xc);
    q += 8;
    t = PhysMove(q, out);
    t = call_via(Func_8000888, t, *(int *)(m + 0x18));
    u = call_via(Func_8000888, t, _Func_80b8530(rec) >> 17);
    *(int *)(out + 4) -= u;
    return 0;
}
