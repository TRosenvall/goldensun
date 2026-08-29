extern unsigned char *iwram_3001ebc;

extern void OvlFunc_915_20088c0(int a);
extern int __GetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern int OvlFunc_915_2008244(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetSpriteFlags(unsigned char *a, int b);
extern void OvlFunc_915_2008c8c(int a);

int OvlFunc_915_2008bf8(void)
{
    unsigned char *base;
    int e;
    int f;
    int g;
    int h;

    base = iwram_3001ebc;
    *(int *)(base + 0x1c0) = 0x204;
    OvlFunc_915_20088c0(0xa);
    if (__GetFlag(0x201) != 0) {
        *(__MapActor_GetActor(0xa) + 0x23) = 2;
        h = 0;
        e = 0xb;
        f = 0x10;
        __Func_8010704(0x20, 0x14, 2, 4, e, f);
        g = 4;
        OvlFunc_915_2008244(2, 0xc, 0x10, 1, g, h);
        __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
    }
    OvlFunc_915_20088c0(8);
    OvlFunc_915_20088c0(9);
    if (__GetFlag(0x845) == 0)
        OvlFunc_915_2008c8c(6);
    return 0;
}
