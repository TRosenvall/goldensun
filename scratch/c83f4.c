struct A {
    unsigned char pad00[0x23];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetSpriteFlags(struct A *a, int n);
extern void __SetFlag(int id);

void OvlFunc_939_20083f4(void)
{
    struct A *a;
    int f;

    a = __MapActor_GetActor(8);
    __MapActor_GetActor(0);
    f = 4;
    __Func_8010704(0x11, 4, 1, 1, 0xe, f);
    __Func_8010704(0xf, 3, 1, 1, 0xf, f);
    __Func_8010704(0xf, 3, 1, 1, 0xd, f);
    if (a != 0) {
        __Actor_SetSpriteFlags(a, 0);
        a->f55 = 2;
        a->f23 = 1;
    }
    __SetFlag(0x80 << 2);
}
