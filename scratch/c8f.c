struct Spr { unsigned char pad00[9]; unsigned char f9; };

struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x28 - 0x14];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0x48 - 0x38];
    int f48;
    unsigned char pad4c[4];
    struct Spr *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[0x6c - 0x56];
    void *f6c;
};

extern unsigned char gState[];
extern unsigned char L9e75c[] __asm__(".L9e75c");
extern unsigned char L9e6c0[] __asm__(".L9e6c0");
extern unsigned char L9e87c[] __asm__(".L9e87c");
extern void Func_808eee4(void);
extern struct Actor *GetFieldActor(int id);
extern void _Actor_TravelTo(struct Actor *a, int x, int y, int z);
extern void WaitFrames(int n);
extern void _Actor_SetAnim(struct Actor *a, int anim);
extern void _Actor_SetSpriteFlags(struct Actor *a, int f);
extern void _Actor_SetScript(struct Actor *a, unsigned char *s);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct Actor *CreateParticleActor(int a, int b, int c, int d);

void Func_808f0d8(struct Actor *a)
{
    struct Actor *b;
    unsigned char *g;

    if (a != 0) {
        g = gState;
        b = GetFieldActor(*(int *)(g + 0x1f4));
        a->f34 = 0x80 << 9;
        a->f30 = 0x80 << 10;
        a->f55 = 0;
        _Actor_TravelTo(a, b->f8, b->fc + (0x90 << 14), b->f10);
        WaitFrames(3);
        _Actor_SetAnim(b, 0x1c);
        _Actor_SetScript(a, L9e75c);
        b->f6 = 0x80 << 7;
    }
}

void Func_808f140(struct Actor *a, int flags)
{
    struct Actor *b;
    unsigned char *g;

    if (a != 0) {
        g = gState;
        b = GetFieldActor(*(int *)(g + 0x1f4));
        if (flags & 1) {
            _Actor_SetSpriteFlags(a, 0);
            _Actor_SetScript(a, L9e6c0);
            a->f28 = 0x80 << 10;
            a->f48 = 0x80 << 7;
            a->f6c = Func_808eee4;
        }
        if (flags == 3)
            WaitFrames(0x3c);
        if (flags & 2)
            Func_808f0d8(a);
        if (flags == 3)
            WaitFrames(0x50);
        _Actor_SetAnim(b, 1);
    }
}

void Func_808f28c(struct Actor *a)
{
    int v[3];
    struct Actor *n;
    int r;
    int mask;

    if (Random() * 0x64 >> 16 <= 9) {
        v[0] = a->f8;
        v[1] = a->fc;
        v[2] = a->f10;
        r = Random();
        vec3_translate(r << 4, Random(), v);
        n = CreateParticleActor(0x11d, v[0], v[1], v[2]);
        if (n != 0) {
            _Actor_SetScript(n, L9e87c);
            _Actor_SetAnim(n, 0);
            mask = ~0xc;
            n->f50->f9 = (mask & n->f50->f9) | 4;
        }
    }
}
