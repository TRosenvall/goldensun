struct M { unsigned char pad00[0x18]; int f18; };
struct P { unsigned char pad00[4]; int f4; };
struct C { void *f0; };

typedef int (*Fn)(int a, int b);

extern int Func_8000888(int a, int b);
extern struct C *GetBattleActor(void);
extern struct M *Func_80b7f70(void *a, int n);
extern void Func_80b7ed8(void);
extern int PhysMove(void *p, struct P *q);
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int n);

static inline int call_via_r5(Fn f, int a, int b)
{
    register Fn _f __asm__("r5") = f;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile ("\t.align\t2, 0\n\tmov\tr12, pc\n\tbx\tr5"
                      : "=r" (_a) : "r" (_f), "0" (_a), "r" (_b)
                      : "memory", "lr", "r12");
    return _a;
}

int Func_80b84c0(int unit, struct P *p)
{
    char *a;
    struct M *m;
    Fn f;
    int v;

    a = (char *)GetBattleActor()->f0;
    m = Func_80b7f70(a, 0);
    a += 8;
    Func_80b7ed8();
    v = PhysMove(a, p);
    f = Func_8000888;
    v = call_via_r5(f, v, m->f18);
    if (Func_80c23c0(*(_GetUnit(unit) + (0x94 << 1))))
        v = call_via_r5(f, v, 0x18);
    else
        v = call_via_r5(f, v, 0x30);
    p->f4 -= v;
    return 0;
}
