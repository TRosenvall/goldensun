struct Ent {
    unsigned char pad0[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x1c];
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x11];
    unsigned char f55;
    unsigned char f56;
    unsigned char pad57;
    unsigned char f58;
};

extern int Func_8000888(int a, int b);
extern int Func_8000948(int a);
extern int Func_80008ac(int a, int b);
extern int FastIntSqrtFP1616_RAM(int a);

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

void Actor_TravelTo(struct Ent *e, int tx, int ty, int tz)
{
    int (*g)(int);
    int (*h)(int, int);
    unsigned char *q;
    int i1, i2, i3;
    int dist, t, u, s, k, base;
    int ax, ay, az;
    int dx, dy, dz;

    dx = (tx - e->f8) / 0x10000;
    dy = (ty - e->fc) / 0x10000;
    dz = (tz - e->f10) / 0x10000;
    g = Func_8000948;
    dist = g(dx * dx + dy * dy + dz * dz) << 16;
    if (dist < (0x80 << 13)) {
        ax = tx - e->f8;
        ay = ty - e->fc;
        az = tz - e->f10;
        i1 = call_via(Func_8000888, ax, ax);
        i2 = call_via(Func_8000888, ay, ay);
        i3 = call_via(Func_8000888, az, az);
        dist = FastIntSqrtFP1616_RAM(i1 + i2 + i3);
    }
    if (dist < (0x80 << 9)) {
        e->f8 = tx;
        e->fc = ty;
        e->f10 = tz;
        e->f38 = 0x80 << 24;
        e->f3c = 0x80 << 24;
        e->f40 = 0x80 << 24;
    } else {
        if (e->f58 == 0) {
            t = call_via(Func_8000888, e->f30, e->f30);
            h = Func_80008ac;
            u = h(e->f34, t);
            if (dist > u)
                k = dist - u / 2;
            else
                k = dist / 2;
            h = Func_80008ac;
            s = h(dist, k);
            base = e->f8;
            tx = base + call_via(Func_8000888, tx - base, s);
            base = e->fc;
            ty = base + call_via(Func_8000888, ty - base, s);
            base = e->f10;
            tz = base + call_via(Func_8000888, tz - base, s);
        }
        e->f38 = tx;
        e->f40 = tz;
        e->f3c = ty;
        dx = tx - e->f8;
        dy = ty - e->fc;
        dz = tz - e->f10;
        q = &e->f56;
        *q = 0x10;
        ax = dx;
        if (dx < 0)
            ax = -dx;
        az = dz;
        if (dz < 0)
            az = -dz;
        if (ax < az) {
            *q = 0x12;
            dx = dz;
        }
        if (e->f55 == 0) {
            if (dx < 0)
                dx = -dx;
            ay = dy;
            if (dy < 0)
                ay = -dy;
            if (dx < ay)
                *q = 0x11;
        }
    }
}
