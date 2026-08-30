extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int a, int b);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __WaitFrames(int n);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_941_20080d4(void)
{
    unsigned char *e;
    int n;
    int m;
    int p1, q1, p2, q2;

    e = __MapActor_GetActor(0xa);
    __MapActor_SetAnim(0xa, 5);
    if (e != 0) {
        __Actor_SetSpriteFlags(e, 0);
        e[0x23] = 1;
    }
    n = 0x15;
    __Func_80105d4(0x29, 0x57, 2, 5, n, 0x3b);
    __WaitFrames(4);
    p1 = 0x18;
    q1 = 0x3e;
    __Func_80105d4(3, 0x5d, 1, 1, p1, q1);
    __Func_80105d4(1, 0x5e, 1, 1, n, 0x37);
    m = 0x3a;
    __Func_80105d4(0x2b, 0x57, 2, 5, n, m);
    __WaitFrames(4);
    __Func_80105d4(0x29, 0x57, 2, 5, n, m);
    __WaitFrames(4);
    __WaitFrames(4);
    __Func_8010704(0x15, 0xb, 2, 2, n, 0xd);
    p2 = 0x16;
    q2 = 0xf;
    __Func_8010704(0x15, 0xb, 1, 1, p2, q2);
    __Func_8010704(0x13, 0x11, 1, 1, n, 0xe);
}
