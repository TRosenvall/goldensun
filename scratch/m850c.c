extern unsigned char gScript_910__02008bf4[];
extern int __GetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8092a1c(int a, int b, unsigned char *s);
extern void OvlFunc_910_2008974(int n);
extern void OvlFunc_910_20085dc(void);

void OvlFunc_910_200850c(void)
{
    int a1, p1, a2, p2, b1, c1, c2, d1;

    a1 = 0x9a << 17;
    p1 = 0x1070000;
    a2 = 0xad << 17;
    p2 = 0x1070000;
    b1 = 0x80 << 9;
    c1 = 0xe0 << 16;
    c2 = 0x92 << 17;
    d1 = 0x80 << 7;
    if (__GetFlag(0x109))
        __ClearFlag(0x80 << 2);
    if (__GetFlag(0xfd2) == 0)
        OvlFunc_910_2008974(0xd);
    if (__GetFlag(0x84a)) {
        __MapActor_SetPos(0xb, a1, p1);
        __MapActor_SetPos(0xc, a2, p2);
        if (__GetFlag(0x84f) == 0 && __GetFlag(0x845) == 0) {
            __MapActor_SetPos(0xb, 0, 0);
            __Func_8092a1c(0xc, b1, gScript_910__02008bf4);
        }
    }
    if (__GetFlag(0x845)) {
        __MapActor_SetPos(0xa, c1, c2);
        __Func_8092adc(0xa, d1, 0);
        __Func_8092adc(8, 0, 0);
        if (__GetFlag(0x85e) == 0)
            OvlFunc_910_20085dc();
    }
}
