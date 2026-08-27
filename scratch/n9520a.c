struct Actor {
    unsigned char pad00[8];
    int x;
    int fc;
    int z;
    int f14;
    unsigned char pad18[0x3d];
    unsigned char f55;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void OvlFunc_927_2008244(int a, int b, int c, int d, int e, int f);

void OvlFunc_927_2009520(void)
{
    struct Actor *a;
    int x;
    int z;
    int one;
    int zero;
    int kind;

    __CutsceneStart();
    x = __MapActor_GetActor(0xb)->x;
    z = __MapActor_GetActor(0xb)->z >> 20;
    x = x >> 20;
    kind = 2;
    one = 1;
    OvlFunc_927_2008244(kind, x, z, 1, one, 0xff);
    zero = 0;
    OvlFunc_927_2008244(kind, x + 1, z, 1, one, zero);
    OvlFunc_927_2008244(kind, x - 1, z, 1, one, zero);
    OvlFunc_927_2008244(kind, x, z + 1, 1, one, zero);
    OvlFunc_927_2008244(kind, x, z - 1, 1, one, zero);
    if (x == 0x24 && z == 0x18) {
        a = __MapActor_GetActor(0xb);
        a->f55 = zero;
        a->f14 = -0x20000;
        a->fc = -0x20000;
    }
    __CutsceneEnd();
}
