struct Sub {
    unsigned char pad0[9];
    unsigned char f9_b0 : 2;
    unsigned char f9_b2 : 2;
    unsigned char f9_b4 : 4;
};

struct Actor {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x50 - 0x24];
    struct Sub *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
};

extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Func_80929d8(struct Actor *a, int n);

struct Actor *OvlFunc_common0_70(int x, int y, int z, int id)
{
    struct Actor *a;

    a = __CreateActor(id, x, y, z);
    if (a != 0) {
        a->f50->f9_b2 = 1;
        a->f55 = 0;
        a->f59 = 8;
        __Actor_SetSpriteFlags(a, 0);
        __Func_80929d8(a, 0xf);
        a->f23 = (a->f23 & 0xfe) | 2;
        return a;
    }
    return 0;
}
