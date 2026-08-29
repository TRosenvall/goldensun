struct Obj {
    int f0;
    int x;
    int y;
    int z;
};

struct PActor {
    unsigned char pad00[0x55];
    unsigned char f55;
};

extern unsigned int iwram_3001f30;
extern void Func_8098698(void);
extern void _PlaySound(int id);
extern int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern struct PActor *CreateParticleActor(int id, int x, int y, int z);
extern void _Actor_SetScript(struct PActor *a, const void *script);
extern void WaitFrames(int frames);
extern void Func_809748c(void);
extern const unsigned char L9f11c[] __asm__(".L9f11c");

void Field_Growth(void)
{
    struct Obj *o;
    struct PActor *a;
    int v[3];
    int *p;
    int i;

    o = *(struct Obj **)&iwram_3001f30;
    Func_8098698();
    _PlaySound(0x86);
    p = v;
    i = 4;
    do {
        p[0] = o->x;
        p[2] = o->z;
        vec3_translate(Random() * 6 + (0x80 << 11), Random(), p);
        p[1] = o->y;
        a = CreateParticleActor(0xd9, p[0], p[1], p[2]);
        if (a != 0) {
            _Actor_SetScript(a, L9f11c);
            a->f55 = 2;
        }
        WaitFrames(((unsigned int)(Random() << 1) >> 16) + 2);
        i--;
    } while (i >= 0);
    WaitFrames(0x1e);
    Func_809748c();
}
