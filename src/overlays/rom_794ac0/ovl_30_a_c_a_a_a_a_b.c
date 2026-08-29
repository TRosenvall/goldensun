struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __Func_8093c00(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_899_2008080(void)
{
    int e1, f1, e2, f2;

    if (__MapActor_GetActor(0)->f6 >= 0xa000
        && __MapActor_GetActor(0)->f6 <= 0xe000) {
        __Func_8093c00();
        e1 = 0x2a;
        f1 = 0x55;
        __Func_8010704(0x29, 0x55, 1, 1, e1, f1);
    } else if (__MapActor_GetActor(0)->f6 >= 0x2000
               && __MapActor_GetActor(0)->f6 <= 0x6000) {
        __Func_8093c00();
        e2 = 0x2a;
        f2 = 0x55;
        __Func_8010704(0x2b, 0x55, 1, 1, e2, f2);
    }
}
