struct Actor {
    unsigned char pad00[8];
    int x;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_948_2009da0(void);

void OvlFunc_948_2009df8(void)
{
    int s0, s1;
    OvlFunc_948_2009da0();
    s0 = __MapActor_GetActor(0xb)->x / 0x100000;
    s1 = 0x37;
    __Func_8010704(0x35, s1, 1, 1, s0, s1);
    s0 = __MapActor_GetActor(0xc)->x / 0x100000;
    __Func_8010704(0x35, s1, 1, 1, s0, s1);
}
