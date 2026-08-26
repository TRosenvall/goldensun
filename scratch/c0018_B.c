struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct A {
    unsigned char pad00[0x50];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
};

extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Func_80929d8(struct A *a, int n);
extern void __Func_800c548(struct A *a, int n);

struct A *OvlFunc_common0_18(int a, int b, int c, int d)
{
    struct A *n;
    int mask;
    unsigned char *p;

    n = __CreateActor(d, a, b, c);
    if (n != 0) {
        mask = ~0xc;
        n->f50->f9 = mask & n->f50->f9;
        p = &n->f55;
        *p = 0;
        p += 4;
        *p = 8;
        __Actor_SetSpriteFlags(n, 0);
        __Func_80929d8(n, 0xe);
        __Func_800c548(n, 1);
        return n;
    }
    return 0;
}
