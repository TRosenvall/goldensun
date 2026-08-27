struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
    int f18;
    int f1c;
    unsigned char pad20[0x30 - 0x20];
    int f30;
    int f34;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Actor_TravelTo(struct A *a, int x, int y, int z);
extern void __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_956_2008274(void)
{
    struct A *a;
    int v30;
    int v34;
    int e;

    a = __MapActor_GetActor(9);
    a->f18 = 0x80 << 9;
    a->f1c = 0x80 << 9;
    a = __MapActor_GetActor(0xb);
    v34 = 0x6666;
    v30 = 0xcccc;
    a->f34 = v34;
    a->f30 = v30;
    __Actor_TravelTo(a, a->f8, 0x80 << 14, a->f10);
    a = __MapActor_GetActor(0xa);
    a->f34 = v34;
    a->f30 = v30;
    __Actor_TravelTo(a, a->f8, 0x80 << 11, a->f10);
    __SetFlag(0x362);
    e = 0xc;
    __Func_8010704(0xf, 0xc, 1, 1, 0xd, e);
    __Func_8010704(0xe, 0xc, 1, 1, 9, e);
}
