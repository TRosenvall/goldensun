struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x38 - 0x14];
    int f38;
    unsigned char pad3c[4];
    int f40;
};

struct C { struct A *f0; };

extern struct C *GetBattleActor(int n);
extern void Func_80b83b0(int *v, int n);

void Func_80b83b4(int a, int b)
{
    struct A *p;
    struct A *q;
    int x1;
    int y1;
    int x2;
    int y2;
    int t[3];

    p = GetBattleActor(a)->f0;
    q = GetBattleActor(b)->f0;
    x1 = p->f38;
    if (x1 == 0x80000000)
        x1 = p->f8;
    y1 = p->f40;
    if (y1 == 0x80000000)
        y1 = p->f10;
    x2 = q->f38;
    if (x2 == 0x80000000)
        x2 = q->f8;
    y2 = q->f40;
    if (y2 == 0x80000000)
        y2 = q->f10;
    t[0] = (x1 + x2) >> 1;
    t[1] = 0;
    t[2] = (y1 + y2) >> 1;
    Func_80b83b0(t, 0x80 << 5);
}
