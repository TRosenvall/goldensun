extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8093fa0(void);
extern void __Func_8093e28(void);
extern void __Func_8093c00(void);
extern void OvlFunc_948_2009b60(void);

void OvlFunc_948_20098e0(void)
{
    unsigned char *a;
    int v;
    int e1, f1, e2, f2;

    a = __MapActor_GetActor(0);
    v = *(unsigned short *)(a + 6);
    if (v == (0xc0 << 8)) {
        if (__GetFlag(0x206) != 0) {
            e1 = 0x2d;
            f1 = 0x2b;
            __Func_8010704(0x2e, 0x2b, 1, 1, e1, f1);
        }
        __ClearFlag(0x207);
        __Func_8093fa0();
    } else if (v == (0x80 << 7)) {
        __Func_8093e28();
    } else if (v == 0) {
        if (__GetFlag(0x206) != 0) {
            e2 = 0x2d;
            f2 = 0x2b;
            __Func_8010704(0x3a, 0x24, 1, 1, e2, f2);
        }
        __SetFlag(0x207);
        __Func_8093c00();
    } else if (v == (0x80 << 8)) {
        if (*(int *)(a + 0xc) == 0)
            OvlFunc_948_2009b60();
        else
            __Func_8093c00();
    }
}
