struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

extern struct A *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200ad0c(void)
{
    int x, y, z, w, n, d, t;

    x = __MapActor_GetActor(0x10)->f8 >> 20;
    y = __MapActor_GetActor(0x10)->f10 >> 20;
    z = __MapActor_GetActor(0x12)->f10 >> 20;
    w = __MapActor_GetActor(9)->f10 >> 20;
    if (x == 0xd) {
        if (w >= 9 && w <= 0xb)
            n = 0x10;
        else if (z >= 9 && z <= 0xb)
            n = 0x40;
        else
            n = 0x70;
    } else if (x == 0xc) {
        if (w >= 9 && w <= 0xb)
            return;
        if (z >= 9 && z <= 0xb)
            n = 0x30;
        else
            n = 0x60;
    } else if (x == 9) {
        if (z >= 9 && z <= 0xb)
            return;
        n = 0x30;
    } else if (x == 8) {
        n = 0x20;
    } else {
        if (x == 6)
            return;
        goto after;
    }
    OvlFunc_946_2009774(0x10, -n, 0);
after:
    __WaitFrames(2);
    d = __MapActor_GetActor(0x10)->f8 >> 20;
    t = y - 1;
    __Func_8010704(x, t, 1, 3, d, t);
    __Func_8010704(0, 0, 1, 3, x, t);
}
