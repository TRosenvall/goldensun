struct Spr {
    unsigned char pad00[5];
    unsigned char f5;
    unsigned char pad06[3];
    unsigned char f9;
    unsigned char pad0a[0x26 - 0xa];
    unsigned char f26;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char pad23[0x50 - 0x23];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
};

extern unsigned char gScript_907__02009d7c[];
extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetScript(struct A *a, unsigned char *s);
extern void __Sprite_SetAnim(struct Spr *s, int n);

void OvlFunc_907_2008f3c(struct A *src)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q;
    unsigned char *r;
    unsigned char zero;
    int mask;

    n = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (n != 0) {
        s = n->f50;
        __Actor_SetScript(n, gScript_907__02009d7c);
        r = &n->f55;
        q = &n->f22;
        zero = 0;
        *r = zero;
        *q = 1;
        q += 1;
        *q = 2;
        if (s != 0) {
            __Sprite_SetAnim(s, 2);
            s->f26 = zero;
            mask = ~0xc;
            s->f5 = (mask & s->f5) | 4;
            s->f9 |= 0xc;
        }
    }
}
