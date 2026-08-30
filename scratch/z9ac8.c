extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_948_20099e8(void);
extern void OvlFunc_948_2009a9c(void);
extern void OvlFunc_948_2009a48(void);
extern void OvlFunc_948_2009a70(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_2009ac8(void)
{
    unsigned char *p;
    int d;
    int z;
    int n;

    p = __MapActor_GetActor(8);
    d = *(int *)(p + 8) / 0x100000;
    if (*(int *)(p + 0xc) == 0)
        __MapActor_GetActor(8)[0x23] = 2;
    OvlFunc_948_20099e8();
    z = 0;
    __MapActor_GetActor(8)[0x55] = 3;
    if (d == 0x28) {
        OvlFunc_948_2009a9c();
    } else if (d == 0x2a) {
        OvlFunc_948_2009a48();
    } else if (d == 0x29) {
        OvlFunc_948_2009a70();
    } else if (d == 0x27 || d == 0x26 || d == 0x25) {
        n = 0x2a;
        __Func_8010704(0x3d, 0x24, 1, 1, d, n);
        __MapActor_GetActor(8)[0x55] = z;
        *(int *)(__MapActor_GetActor(8) + 0xc) = 0x80 << 14;
    }
}
