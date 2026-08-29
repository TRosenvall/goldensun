typedef unsigned char u8;
typedef unsigned short u16;

struct A {
    u8 pad00[8];
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    u8 pad20[8];
    int f28;
    u8 pad2c[4];
    int f30;
    u8 pad34[0x21];
    u8 f55;
    u8 pad56[8];
    u16 f5e;
    u8 pad60[0xc];
    int f6c;
};

extern u8 Data_9f0b0[];
extern void _Actor_SetScript(struct A *a, u8 *s);
extern struct A *CreateParticleActor(int kind, int x, int y, int z);
extern int Random(void);
extern void Func_8096bec(struct A *a, int v, int r);

void Func_8099920(struct A *e)
{
    struct A *a;
    int i;

    if (e->fc > e->f14)
        return;

    e->f5e = 2;
    _Actor_SetScript(e, Data_9f0b0);
    e->f6c = 0;
    i = 0;
    while (i < 3) {
        a = CreateParticleActor(0xf0, e->f8, e->fc, e->f10);
        if (a == 0)
            break;
        a->f1c = 0x8000;
        a->f18 = 0x8000;
        a->f55 = 2;
        a->f28 = 0x10000;
        a->f30 = Random() + 0x13333;
        Func_8096bec(a, 0x200000, Random());
        a->f5e = 6;
        _Actor_SetScript(a, Data_9f0b0);
        i++;
    }
}
