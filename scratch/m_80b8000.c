struct A {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned char pad08[0x28 - 8];
    int f28;
    unsigned char pad2c[4];
    int f30;
    int f34;
    unsigned char pad38[0x44 - 0x38];
    int f44;
    unsigned char pad48_[0];
    int f48;
    unsigned char pad4c[0x58 - 0x4c];
    unsigned char f58;
    unsigned char pad59[1];
    unsigned char f5a;
};

struct C {
    struct A *f0;
    unsigned char pad04[8];
    int fc;
    int f10;
};

extern struct C *GetBattleActor(void);
extern void _Actor_Stop(struct A *a);
extern void _Actor_TravelTo(struct A *a, int x, int y, int z);
extern int atan2(int y, int x);

void Func_80b8000(void)
{
    struct C *c;
    struct A *a;
    unsigned char *p5a;
    unsigned char *p58;

    c = GetBattleActor();
    a = c->f0;
    a->f34 = 0x80 << 10;
    a->f30 = 0x80 << 12;
    a->f48 = 0xab85;
    p5a = &a->f5a;
    p58 = &a->f58;
    a->f28 = 0;
    a->f44 = 0;
    *p5a = 0;
    *p58 = 1;
    _Actor_Stop(a);
    _Actor_TravelTo(a, c->fc, 0, c->f10);
    a->f6 = atan2(c->f10 / 8, c->fc) + (0x80 << 8);
}
