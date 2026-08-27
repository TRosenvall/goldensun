struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_200862c(void)
{
    int f;

    f = 0xb;
    __Func_8010704(0x64, 0xb, 0xc, 4, 0xe, f);
    __Func_8010704(0xd, 0x1c, 1, 4, __MapActor_GetActor(0xf)->f8 >> 20, f);
    __Func_8010704(0xd, 0x1c, 1, 4, __MapActor_GetActor(0x10)->f8 >> 20, f);
    __Func_8010704(0xd, 0x1c, 4, 1, 0x12,
                   __MapActor_GetActor(0x11)->f10 >> 20);
}
