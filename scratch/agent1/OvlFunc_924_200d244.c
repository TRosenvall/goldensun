struct Sub2 {
    unsigned char pad0[5];
    unsigned char f5;
};

struct Sub {
    unsigned char pad0[0x25];
    unsigned char f25;
    unsigned char pad26[2];
    struct Sub2 *f28;
};

struct Src {
    unsigned char pad0[8];
    int f8;
    unsigned char padc[4];
    int f10;
};

struct Ent {
    unsigned char pad0[8];
    int f8;
    unsigned char padc[4];
    int f10;
    unsigned char pad14[0x1c];
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0xc];
    struct Sub *f50;
    unsigned char pad54[0x14];
    struct Src *f68;
};

extern int Func_8000888(int a, int b);
extern int Func_8000948(int a);
extern int Func_80008ac(int a, int b);
extern int __FastIntSqrtFP1616_RAM(int a);
extern unsigned int iwram_3001e40;

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r12"
    );
    return _a;
}

void OvlFunc_924_200d244(struct Ent *e)
{
    struct Src *s;
    int px, pz;
    int dx, dz;
    int d, dist, lim, t, v;
    int (*g)(int);
    int (*h)(int, int);

    e->f30 = 0x80 << 10;
    e->f34 = 0x80 << 9;
    s = e->f68;
    px = s->f8;
    e->f38 = 0x80 << 24;
    e->f3c = 0x80 << 24;
    e->f40 = 0x80 << 24;
    pz = s->f10;
    dx = (px - e->f8) / 0x10000;
    dz = (pz - e->f10) / 0x10000;
    g = Func_8000948;
    d = g(dx * dx + dz * dz);
    dx = px - e->f8;
    dz = pz - e->f10;
    dist = d << 16;
    if (dist < (0x80 << 15)) {
        t = call_via(Func_8000888, dx, dx);
        v = call_via(Func_8000888, dz, dz);
        dist = __FastIntSqrtFP1616_RAM(t + v);
    }
    lim = dist / 8;
    if (lim > e->f30)
        lim = e->f30;
    if (dist < (0x80 << 7)) {
        e->f8 = px;
        e->f10 = pz;
    } else {
        if (dist > lim) {
            h = Func_80008ac;
            t = h(dist, dx);
            dx = call_via(Func_8000888, t, lim);
            t = h(dist, dz);
            dz = call_via(Func_8000888, t, lim);
        }
        e->f8 += dx;
        e->f10 += dz;
    }
    v = (iwram_3001e40 >> 1) & 1;
    e->f50->f28->f5 = v * 7;
    e->f50->f25 = 1;
}
