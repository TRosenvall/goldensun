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

int Func_80b84c0(int unit, struct P *p)
{
    char *a;
    struct M *m;
    int v;
    Fn f;

    a = (char *)GetBattleActor()->f0;
    m = Func_80b7f70(a, 0);
    a += 8;
    Func_80b7ed8();
    v = PhysMove(a, p);
    f = Func_8000888;
    v = f(v, m->f18);
    if (Func_80c23c0(*(_GetUnit(unit) + (0x94 << 1))))
        v = f(v, 0x18);
    else
        v = f(v, 0x30);
    p->f4 -= v;
    return 0;
}
