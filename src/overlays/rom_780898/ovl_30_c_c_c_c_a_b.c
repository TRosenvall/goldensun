struct S {
    unsigned char pad00[9];
    unsigned char f9_lo : 2;
    unsigned char f9_mid : 2;
    unsigned char f9_hi : 4;
};

struct A {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x34];
    int f48;
    unsigned char pad4c[4];
    struct S *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[8];
    unsigned short f5e;
};

extern int __Random(void);
extern void __vec3_translate(int a, int b, int *v);
extern struct A *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct A *a, int f);
extern void __Actor_SetAnim(struct A *a, int n);
extern void __Actor_SetScript(struct A *a, const void *s);
extern void __PlaySound(int id);
extern const unsigned char gScript_883__0200e6e0[];

void OvlFunc_883_200d75c(struct A *src)
{
    int v[3];
    int *p;
    struct A *a;
    int r;

    p = v;
    p[0] = src->f8;
    p[1] = src->fc - (__Random() << 4) + 0xfff80000;
    p[2] = src->f10;
    r = __Random();
    __vec3_translate(r * 48, __Random(), p);
    a = __CreateActor(0x11d, p[0], p[1], p[2]);
    if (a != 0) {
        a->f55 = 2;
        a->f48 = 0x1999;
        a->f5e = 0xc;
        __Actor_SetSpriteFlags(a, 0);
        __Actor_SetAnim(a, 0);
        __Actor_SetScript(a, gScript_883__0200e6e0);
        a->f50->f9_mid = 1;
    }
    __PlaySound(0x8a);
}
