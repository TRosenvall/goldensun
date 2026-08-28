struct Obj {
    int f0;
    int f4;
    int f8;
    int fc;
};

struct Ent {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x18];
    int f38;
    unsigned char pad3c[0xc];
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
    unsigned char pad56[8];
    unsigned short f5e;
    unsigned char pad60[4];
    short f64;
    unsigned char pad66[6];
    void *f6c;
};

extern unsigned int iwram_3001f30;
extern unsigned int iwram_3001e40;
extern int sin(int a);
extern int Func_8000888(int a, int b);
extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct Ent *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetColorswap(struct Ent *a, int n);
extern void _Actor_SetScript(struct Ent *a, void *script);
extern void Func_80992f0(void);
extern unsigned char Data_9f0b0[];

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

void Func_80993b0(struct Ent *e)
{
    struct Obj *o;
    struct Ent *n;
    short *ph;
    int v[3];
    int *p;

    o = *(struct Obj **)&iwram_3001f30;
    ph = &e->f64;
    if (*ph != -1) {
        e->f8 = o->f4 + call_via_r3(0xc0 << 11, sin(*ph << 10));
        e->fc = o->f8 + (0x80 << 13);
        e->f10 = o->fc;
        *ph = *ph + 1;
        *ph = (*ph + 0x40) % 64;
    }
    if (iwram_3001e40 % 3 == 0) {
        p = v;
        p[0] = e->f8;
        p[1] = e->fc + (0x80 << 10);
        p[2] = e->f10;
        vec3_translate(Random() * 6, Random(), p);
        n = CreateParticleActor(0x11d, p[0], p[1], p[2]);
        if (n != 0) {
            n->f6c = Func_80992f0;
            n->f1c = 0x9999;
            n->f18 = 0x9999;
            n->f55 = 2;
            n->f48 = 0xe5 << 1;
            n->f64 = (unsigned int)Random() >> 9;
            n->f38 = n->f8;
            _Actor_SetColorswap(n, 9);
            n->f5e = 0x48;
            _Actor_SetScript(n, Data_9f0b0);
        }
    }
}
