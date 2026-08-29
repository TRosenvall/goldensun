struct Actor { unsigned char pad00[8]; int f8; unsigned char pad0c[4]; int f10; };

extern struct Actor *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_2009ef4(void)
{
    int x;
    int z;

    x = __MapActor_GetActor(0xb)->f8 >> 20;
    z = __MapActor_GetActor(0xb)->f10 >> 20;
    if (x == 0x24)
        return;
    if (x == 0x1e) {
        if ((__MapActor_GetActor(0xa)->f10 >> 20) == 0x12)
            return;
        OvlFunc_946_2009774(0xb, 0x60, 0);
    } else if (x == 0x22) {
        OvlFunc_946_2009774(0xb, 0x20, 0);
    }
    __WaitFrames(2);
    z = z - 1;
    __Func_8010704(x, z, 1, 3, __MapActor_GetActor(0xb)->f8 >> 20, z);
    __Func_8010704(0, 0, 1, 3, x, z);
}
