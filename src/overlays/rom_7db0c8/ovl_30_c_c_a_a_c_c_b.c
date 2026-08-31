extern unsigned char gState[];
extern volatile int gKeyHeld;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_954_200833c(int a, int b, int c);

void OvlFunc_954_2008490(void)
{
    unsigned char *g;
    unsigned char *a;
    int dir;
    int x;
    int n;
    int nine;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    x = *(int *)(a + 8) >> 20;
    if (gKeyHeld & 0x20)
        dir = -1;
    if (gKeyHeld & 0x10)
        dir = 1;
    a = __MapActor_GetActor(0x11);
    n = *(int *)(a + 0x10) >> 20;
    if (x == 0x3f) {
        if (n == 0xb)
            return;
        n = 0xa0;
    } else if (x == 0x43) {
        if (n == 0xb && dir == -1)
            return;
        n = 0x60;
    } else {
        if (n == 0xb)
            n = 0x60;
        else
            n = 0xa0;
        n = -n;
    }
    nine = 9;
    __Func_8010704(0x48, 9, 1, 3, x, nine);
    OvlFunc_954_200833c(0x12, n, 0);
    a = __MapActor_GetActor(0x12);
    x = *(int *)(a + 8) >> 20;
    __Func_8010704(0x3f, 0x19, 1, 3, x, nine);
}
