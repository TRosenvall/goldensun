struct Src {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

struct Actor {
    unsigned char pad00[0x48];
    int f48;
    unsigned char pad4c[0x55 - 0x4c];
    unsigned char f55;
    unsigned char pad56[0x5e - 0x56];
    short f5e;
};

extern unsigned char *iwram_3001f30;
extern unsigned char Data_9f0b0[];

extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct Actor *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _Actor_SetScript(struct Actor *a, void *s);

void Func_8099d18(void)
{
    int v[3];
    struct Src *s;
    struct Actor *a;

    s = *(struct Src **)(iwram_3001f30 + 0x14);
    v[0] = s->f8;
    v[1] = s->fc - (Random() << 4) + (0xc0 << 13);
    v[2] = s->f10;
    vec3_translate(Random() * 48, Random(), v);
    a = CreateParticleActor(0x11d, v[0], v[1], v[2]);
    if (a != 0) {
        a->f55 = 2;
        a->f48 = 0x1999;
        _Actor_SetAnim(a, 0);
        a->f5e = 0xc;
        _Actor_SetScript(a, Data_9f0b0);
    }
}
