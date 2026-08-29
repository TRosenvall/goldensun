struct A { unsigned char pad00[0x55]; unsigned char f55; };

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_20099e8(void)
{
    __Func_8010704(0x39, 0x2a, 1, 1, 0x28, 0x2a);
    __Func_8010704(0x39, 0x2a, 1, 1, 0x29, 0x2a);
    __Func_8010704(0x3a, 0x2a, 1, 1, 0x2a, 0x2a);
    __Func_8010704(0x3e, 0x25, 3, 1, 0x25, 0x2a);
    __MapActor_GetActor(8)->f55 = 1;
}
