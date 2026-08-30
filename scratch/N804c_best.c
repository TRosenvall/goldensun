extern int L441c __asm__(".L441c");

extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetAnim(int a, int b);

void OvlFunc_954_200804c(void)
{
    int a;
    int b;

    switch ((unsigned int)L441c) {
    case 6:
    case 0x42:
        b = 0x26;
        __Func_8010788(0x5c, 0x1f, 2, 2, 0x32, b);
        __Func_8010788(0x5c, 0x1f, 2, 2, 0x36, b);
        __MapActor_SetAnim(0x10, 0xa);
        break;
    case 0x3c:
        a = 0x32;
        b = 0x26;
        __Func_8010788(0x5c, 0x21, 2, 2, a, b);
        __Func_8010788(0x5c, 0x21, 2, 2, 0x36, b);
        __Func_8010704(0x32, 0x19, 6, 1, a, 0xc);
        __MapActor_SetAnim(0x10, 0xb);
        break;
    case 0:
        a = 0x32;
        b = 0x26;
        __Func_8010788(0x5c, 0x1d, 2, 2, a, b);
        __Func_8010788(0x5c, 0x1d, 2, 2, 0x36, b);
        __MapActor_SetAnim(0x10, 0xc);
        __Func_8010704(0x32, 0x18, 6, 1, a, 0xc);
        L441c = 0x78;
        break;
    }
    L441c = L441c - 1;
}
