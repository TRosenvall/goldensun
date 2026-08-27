struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Func_80929d8(struct A *a, int n);

struct A *OvlFunc_922_2008ed8(int a, int b, int c, int d)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q55;
    unsigned char *q23;
    unsigned char two;
    int mask;

    n = __CreateActor(d, a, b, c);
    if (n != 0) {
        s = n->f50;
        mask = ~0xc;
        q55 = &n->f55;
        s->f9 = (mask & s->f9) | 4;
        *q55 = 0;
        __Actor_SetSpriteFlags(n, 0);
        __Func_80929d8(n, 0xf);
        q23 = &n->f23;
        two = 2;
        *q23 = two | *q23;
        return n;
    }
    return 0;
}
