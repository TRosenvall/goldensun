struct Ent {
    int *f0;
    short f4;
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x4a];
    short f5e;
    unsigned char pad60[4];
    short f64;
    short f66;
};

extern int Func_8000888(int a, int b);
extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern int Func_800d924(struct Ent *e, int *v);
extern int TestCollision(struct Ent *e, int *v);
extern void Actor_TravelTo(struct Ent *e, int x, int y, int z);

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

int ActorCmd_Wander(struct Ent *e)
{
    int *p;
    int a, b, c, t;
    int i;
    int dist, head;
    int dx, dz, x, z;
    int v[3];
    int w[3];

    p = e->f0 + e->f4 + 1;
    a = *p++;
    b = *p++;
    c = *p / 0x10000;
    c = c * c;
    i = 0;
    while (1) {
        i++;
        if (i <= 7) {
        v[0] = e->f8;
        v[1] = e->fc;
        v[2] = e->f10;
        dist = a + call_via_r3(Random(), b);
        head = e->f6 + ((unsigned int)Random() >> 2) - ((unsigned int)Random() >> 2);
        vec3_translate(dist, head, v);
        if (Func_800d924(e, v) != 0)
            continue;
        if (TestCollision(e, v) != 0)
            continue;
        dist += 0x80 << 12;
        w[0] = e->f8;
        w[1] = e->fc;
        w[2] = e->f10;
        vec3_translate(dist, head, w);
        w[0] = e->f8;
        w[1] = e->fc;
        w[2] = e->f10;
        vec3_translate(dist, head + 0x2000, w);
        if (TestCollision(e, w) != 0)
            continue;
        w[0] = e->f8;
        w[1] = e->fc;
        w[2] = e->f10;
        vec3_translate(dist, head - 0x2000, w);
        if (TestCollision(e, w) != 0)
            continue;
        x = v[0];
        dx = x / 0x10000 - e->f64;
        z = v[2];
        dz = z / 0x10000 - e->f66;
        if (dx * dx + dz * dz <= c)
            goto ok;
        } else {
        e->f6 = e->f6 + 0x8000;
        e->f5e = 1;
        return 0;
        }
    }
ok:
    Actor_TravelTo(e, x, v[1], z);
    e->f4 = e->f4 + 4;
    return 1;
}
