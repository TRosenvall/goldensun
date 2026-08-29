struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[0x55 - 0xc];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetSpriteFlags(struct A *a, int n);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);

void OvlFunc_932_200840c(void)
{
    struct A *a;
    int e;
    int f;
    int y;

    a = __MapActor_GetActor(0xa);
    if (a != 0) {
        e = 0x18;
        f = 0x1a;
        __Func_8010704(0x18, 0x1b, 2, 1, e, f);
        y = a->f8 >> 20;
        if (y == 0x19)
            __Func_8010704(0, 0, 1, 1, y, f);
        else
            __Func_8010704(0, 0, 1, 1, e, f);
        __Actor_SetSpriteFlags(a, 0);
        a->f55 = 0;
        __Func_800fe9c();
        __WaitFrames(1);
    }
}
