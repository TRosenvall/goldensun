struct Actor {
    unsigned char pad00[8];
    int x;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_948_2009da0(void);

void OvlFunc_948_2009df8(void)
{
    struct Actor *p;
    struct Actor *q;
    int s1;

    OvlFunc_948_2009da0();
    s1 = 0x37;
    p = __MapActor_GetActor(0xb);
    __Func_8010704(0x35, 0x37, 1, 1,
                   p->x / 0x100000, s1);
    q = __MapActor_GetActor(0xc);
    __Func_8010704(0x35, 0x37, 1, 1,
                   q->x / 0x100000, s1);
}
