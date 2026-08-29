struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    int f14;
    unsigned char pad18[0x23 - 0x18];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_957_2008cf8(void)
{
    struct A *a;
    int y;

    a = __MapActor_GetActor(0xc);
    if ((a->f8 >> 20) == 0x1e) {
        y = a->f10 >> 20;
        if (y == 0x14) {
            a->f55 = 2;
            a->f14 = 0;
            a->f23 = 2;
            __Func_8010704(0x1e, 0x14, 1, 1, 0x20, y);
            __SetFlag(0x212);
        }
    }
}
