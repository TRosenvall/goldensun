extern unsigned char gState[];
extern unsigned int gKeyHeld;
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_955_2008310(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_2008400(void)
{
    unsigned char *gs;
    unsigned char *a;
    int v, n, d, m;

    gs = gState;
    a = __MapActor_GetActor(*(int *)(gs + 0x1f4));
    v = *(int *)(a + 8) >> 20;
    d = 0;
    n = 0x20;
    if ((*(int *)(a + 0x10) >> 20) > 0xc)
        n = 0x21;
    a = __MapActor_GetActor(n);
    if ((*(int *)(a + 8) >> 20) != v)
        return;
    if (v > 0x33) {
        if (gKeyHeld & 0x20)
            d = -0x40;
    } else {
        if (gKeyHeld & 0x10)
            d = 0x40;
    }
    if (d == 0)
        return;
    OvlFunc_955_2008310(n, d, 0);
    m = 0xa;
    __Func_8010704(0x78, 0xa, 5, 6, 0x30, m);
    a = __MapActor_GetActor(0x20);
    v = *(int *)(a + 8) >> 20;
    __Func_8010704(0x34, 0x1c, 1, 3, v, m);
    a = __MapActor_GetActor(0x21);
    v = *(int *)(a + 8) >> 20;
    __Func_8010704(0x34, 0x1c, 1, 3, v, 0xd);
}
