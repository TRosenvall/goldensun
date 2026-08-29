struct Ent {
    unsigned char pad0[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x34];
    int f48;
    unsigned char pad4c[9];
    unsigned char f55;
    unsigned char pad56[4];
    unsigned char f5a;
    unsigned char pad5b[3];
    unsigned short f5e;
    unsigned char pad60[8];
    struct Ent *f68;
};

extern int atan2(int a, int b);
extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct Ent *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetAnim(struct Ent *a, int anim);
extern void _Actor_SetScript(struct Ent *a, void *script);
extern unsigned char Data_9f0b0[];

void Func_8097b70(struct Ent *e)
{
    struct Ent *t;
    struct Ent *n;
    int d8, d10, d;
    int a, b;
    int v[3];

    t = e->f68;
    if (t != 0) {
        d8 = t->f8 - e->f8;
        d10 = t->f10 - e->f10;
        if (d8 != 0 || d10 != 0) {
            d = (short)(atan2(d10, d8) - e->f6);
            if (d > 0x1000)
                d = 0x1000;
            if (d < -0x1000)
                d = -0x1000;
            e->f6 = e->f6 + d;
        }
        e->f5a = 0;
    }
    v[0] = e->f8;
    v[1] = e->fc - (Random() << 4) - 0x80000;
    v[2] = e->f10;
    a = Random() * 48;
    b = Random();
    vec3_translate(a, b, v);
    n = CreateParticleActor(0x11d, v[0], v[1], v[2]);
    if (n != 0) {
        n->f55 = 2;
        n->f48 = 0x1999;
        _Actor_SetAnim(n, 0);
        n->f5e = 0xc;
        _Actor_SetScript(n, Data_9f0b0);
    }
}
