struct Ent {
    unsigned char pad0[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50];
    short f64;
    short f66;
};

extern int L51b4[] __asm__(".L51b4");
extern int Func_8000888(int a, int b);
extern void __PlaySound(int id);
extern void __vec3_translate(int a, int b, int *v);
extern int __Func_8011f54(int a, int b, int c);
extern void __Actor_SetAnim(struct Ent *e, int anim);

static inline int call_via_r3(int a, int b)
{
    register int (*_f)(int, int) __asm__("r3") = Func_8000888;
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\tr3"
        : "=r" (_a)
        : "r" (_f), "0" (_a), "r" (_b)
        : "memory", "r12"
    );
    return _a;
}

void OvlFunc_932_200b738(struct Ent *e)
{
    int m, q, a, h1, h2;
    short *ct;
    int v[3];
    int *p;

    m = e->f6 & 0xc000;
    q = e->fc / 0x10000;
    a = L51b4[e->f64 - q + 16];
    ct = &e->f66;
    if (*ct != 0) {
        *ct = *ct - 1;
        if (*ct == 0x14)
            __PlaySound(0xb8);
        if (*ct == 0)
            __PlaySound(0xe9);
    }
    p = v;
    p[0] = e->f8;
    p[1] = e->fc;
    p[2] = e->f10;
    __vec3_translate(call_via_r3(a, 0xc0 << 8), m, p);
    e->f8 = p[0];
    e->f10 = p[2];
    h1 = __Func_8011f54(2, e->f8, e->f10);
    __vec3_translate(-call_via_r3(a, 0xc0 << 9), m, p);
    h2 = __Func_8011f54(2, p[0], p[2]);
    if (*ct <= 0x14) {
        if (h1 == h2)
            __Actor_SetAnim(e, 2);
        else if (h1 > h2)
            __Actor_SetAnim(e, 3);
        else
            __Actor_SetAnim(e, 4);
    }
}
