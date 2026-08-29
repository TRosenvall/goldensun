extern unsigned int gState;
extern int _AREA_36;
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void OvlFunc_common0_18(int a, int b, int c, int d);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __Func_8092708(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_924_200b6ac(void)
{
    unsigned char *a;
    unsigned int q;
    unsigned int off;
    int v;
    int a1, a2, b1, c0, c2, d1, d2;
    int e, f;

    a1 = 0xec << 1;
    a2 = 0x96 << 2;
    b1 = 0x80 << 7;
    c0 = 0xe8 << 17;
    c2 = 0xa4 << 18;
    d1 = -1;
    d2 = -1;
    __CutsceneStart();
    q = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    q += off;
    v = *(short *)q;
    if (v == (int)(&_AREA_36)) {
        __Func_80921c4(0, a1, a2);
        __Func_8092adc(0, b1, 0xa);
        __Func_80933f8(c0, d1, c2, 1);
        a = __MapActor_GetActor(0);
        __Actor_SetSpriteFlags(a, 0);
        a = __MapActor_GetActor(0);
        OvlFunc_common0_18(*(int *)(a + 8), 0, 0x2be0000, 0xdf);
        e = 3;
        f = 2;
        __CopyMapTiles(0x5c, 0x2e, 0x5c, 0x28, e, f);
        a = __MapActor_GetActor(0);
        *(int *)(a + 0x48) = 0x80 << 8;
        __Func_8092b08(0, 2);
        __Func_8092708(0, 6, d1);
        q = (unsigned int)iwram_3001ebc;
        off = 0xe0;
        off <<= 1;
        q += off;
        off += 0x43;
        *(int *)q = off;
        __CutsceneWait(0x3c);
        __Func_8091e9c(8);
    } else {
        __Func_8092708(0, 6, d2);
    }
    __CutsceneEnd();
}
