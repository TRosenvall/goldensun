struct Spr {
    unsigned char pad00[9];
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

extern unsigned char gScript_923__0200a7b8[];
extern unsigned char gScript_924__0200de08[];
extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetScript(struct A *a, unsigned char *s);
extern void __Sprite_SetAnim(struct Spr *s, int n);

void OvlFunc_923_2009bc8(struct A *src)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q;
    unsigned char zero;

    n = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (n != 0) {
        s = n->f50;
        __Actor_SetScript(n, gScript_923__0200a7b8);
        zero = 0;
        n->f55 = zero;
        q = &n->f22;
        *q = 1;
        q += 1;
        *q = 2;
        if (s != 0) {
            __Sprite_SetAnim(s, 2);
            s->f26 = zero;
            s->f9 |= 0xc;
        }
    }
}

void OvlFunc_924_200d158(struct A *src)
{
    struct A *n;
    struct Spr *s;
    unsigned char *q;
    unsigned char zero;

    n = __CreateActor(0x18, src->f8, src->fc, src->f10);
    if (n != 0) {
        s = n->f50;
        __Actor_SetScript(n, gScript_924__0200de08);
        zero = 0;
        n->f55 = zero;
        q = &n->f22;
        *q = 1;
        q += 1;
        *q = 2;
        if (s != 0) {
            __Sprite_SetAnim(s, 2);
            s->f26 = zero;
            s->f9 |= 0xc;
        }
    }
}
