struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_964_2008f10(int a, int b);

void OvlFunc_964_200a3a0(void)
{
    int e, f;

    e = 9;
    f = 0x26;
    __Func_8010704(0x49, 0x26, 5, 5, e, f);
    OvlFunc_964_2008f10(9, 8);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(8)->f8 >> 20,
                   __MapActor_GetActor(8)->f10 >> 20);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(9)->f8 >> 20,
                   __MapActor_GetActor(9)->f10 >> 20);
}

void OvlFunc_964_200a410(void)
{
    int e, f;

    e = 0x1d;
    f = 0x1e;
    __Func_8010704(0x5d, 0x1e, 6, 5, e, f);
    OvlFunc_964_2008f10(0xb, 0xa);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(0xa)->f8 >> 20,
                   __MapActor_GetActor(0xa)->f10 >> 20);
    __Func_8010704(2, 0x24, 1, 1,
                   __MapActor_GetActor(0xb)->f8 >> 20,
                   __MapActor_GetActor(0xb)->f10 >> 20);
}

void OvlFunc_964_200a480(void)
{
    int e;

    e = 0x19;
    __Func_8010704(0x59, 0x31, 3, 2, e, 0x31);
    __Func_8010704(0x59, 0x33, 8, 5, e, 0x33);
    __MapActor_GetActor(0xe)->f22 = 1;
    __Func_8010704(0x16, 0x34, 1, 1,
                   __MapActor_GetActor(0xc)->f8 >> 20,
                   __MapActor_GetActor(0xc)->f10 >> 20);
    __Func_8010704(0x16, 0x34, 1, 1,
                   __MapActor_GetActor(0xd)->f8 >> 20,
                   __MapActor_GetActor(0xd)->f10 >> 20);
    __Func_8010704(0x16, 0x34, 1, 1,
                   __MapActor_GetActor(0xe)->f8 >> 20,
                   __MapActor_GetActor(0xe)->f10 >> 20);
}

void OvlFunc_964_200a52c(void)
{
    int e, f;

    e = 0x2c;
    f = 0x13;
    __Func_8010704(0x6c, 0x13, 4, 1, e, f);
    OvlFunc_964_2008244(0,
                        __MapActor_GetActor(0x11)->f8 >> 20,
                        __MapActor_GetActor(0x11)->f10 >> 20, 1, 1, 0xff);
    OvlFunc_964_2008244(0,
                        __MapActor_GetActor(0x12)->f8 >> 20,
                        __MapActor_GetActor(0x12)->f10 >> 20, 1, 1, 0xff);
}
