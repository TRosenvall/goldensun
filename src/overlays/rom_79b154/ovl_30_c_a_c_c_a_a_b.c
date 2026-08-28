extern unsigned int gState;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8092adc(int a, int b, int c);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void OvlFunc_907_20089cc(void);

void OvlFunc_907_20088f0(void)
{
    unsigned char *a;
    unsigned int g;
    unsigned int off;
    int v;
    int w1, w2, x1, y1;

    w1 = 0xc0 << 6;
    w2 = 0xa0 << 7;
    x1 = 0xf8 << 16;
    y1 = 0xd8 << 16;
    if (__GetFlag(0x845) != 0) {
        __MapActor_SetPos(9, 0, 0);
        __Func_8092adc(0xe, w1, 0);
        __Func_8092adc(0xf, w2, 0);
    } else {
        a = __MapActor_GetActor(9);
        __Actor_SetSpriteFlags(a, 0);
        __MapActor_SetPos(0x15, 0, 0);
    }
    a = __MapActor_GetActor(8);
    *(int *)(a + 0x1c) = 0xc0 << 9;
    g = (unsigned int)&gState;
    off = 0xe1;
    off <<= 1;
    g += off;
    v = *(short *)g;
    if (v == 0xa) {
        __MapActor_SetPos(8, 0, 0);
    } else if (v == 9) {
        __ClearFlag(0x12f);
    }
    if (__GetFlag(0x109) == 0) {
        g = (unsigned int)&gState;
        off = 0xe1;
        off <<= 1;
        g += off;
        if (*(short *)g == 0xb)
            __MapActor_SetPos(0x14, x1, y1);
    }
    OvlFunc_907_20089cc();
    if (__GetFlag(0x84a) != 0 && __GetFlag(0x84b) == 0)
        __SetFlag(0xc1 << 2);
}
