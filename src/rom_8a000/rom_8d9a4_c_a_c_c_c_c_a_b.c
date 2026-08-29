typedef unsigned char u8;

struct Sub {
    u8 pad0[9];
    u8 b9;
};

struct Actor {
    u8 pad00[8];
    int f8;
    int fc;
    int f10;
    u8 pad14[0x28 - 0x14];
    int f28;
    u8 pad2c[0x55 - 0x2c];
    u8 f55;
    u8 pad56[0x50 - 0x56 + 0x50];
};

struct Actor2 {
    u8 pad00[0x50];
    struct Sub *f50;
};

extern u8 gL9e87c[] __asm__(".L9e87c");
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct Actor2 *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetScript(struct Actor2 *a, u8 *s);
extern void _Actor_SetAnim(struct Actor2 *a, int n);

void Func_808eee4(struct Actor *a)
{
    int v[3];
    struct Actor2 *q;
    struct Sub *s;
    unsigned int r1;
    unsigned int r2;
    int t;

    if (a->f28 >= -0xff && a->f28 <= 0xff)
        a->f55 = 0;
    if (Random() * 0x64 >> 16 <= 9) {
        v[0] = a->f8;
        v[1] = a->fc;
        v[2] = a->f10;
        r1 = Random();
        r2 = Random();
        vec3_translate(r1 << 4, r2, v);
        q = CreateParticleActor(0x11d, v[0], v[1], v[2]);
        if (q != 0) {
            _Actor_SetScript(q, gL9e87c);
            _Actor_SetAnim(q, 0);
            s = q->f50;
            t = ~0xc;
            t &= s->b9;
            t |= 4;
            s->b9 = t;
        }
    }
}
