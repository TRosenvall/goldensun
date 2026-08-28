extern unsigned char gState[];
extern int _AREA_10;
extern int _AREA_13;
extern unsigned char L1fd8[] __asm__(".L1fd8");
extern unsigned char L2050[] __asm__(".L2050");
extern unsigned char L21b8[] __asm__(".L21b8");
extern unsigned char L22a8[] __asm__(".L22a8");
extern unsigned char L1fc0[] __asm__(".L1fc0");
extern unsigned char L22e4[] __asm__(".L22e4");
extern unsigned char L232c[] __asm__(".L232c");
extern unsigned char L241c[] __asm__(".L241c");
extern unsigned char L2524[] __asm__(".L2524");
extern unsigned char L22d8[] __asm__(".L22d8");
extern void __Func_808b868(unsigned char *p);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __WaitFrames(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8092b08(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int n);
extern void __Func_809228c(int a, int b, int c);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int n);

unsigned char *OvlFunc_895_200807c(void)
{
    unsigned char *g;
    unsigned char *r;
    int e;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_10)) {
        e = *(short *)(g + (0xe1 << 1));
        switch (e) {
        case 0xe: case 0xf: case 0x10:
            return L21b8;
        case 0xb: case 0xc: case 0xd:
            return L2050;
        default:
            r = L1fd8;
            __Func_808b868(r);
            return r;
        }
    }
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_13))
        return L22a8;
    return L1fc0;
}

unsigned char *OvlFunc_895_20080ec(void)
{
    unsigned char *g;
    int e;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_13))
        return L22e4;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_10)) {
        e = *(short *)(g + (0xe1 << 1));
        switch (e) {
        case 0xb: case 0xc: case 0xd:
            return L241c;
        case 0xe: case 0xf: case 0x10:
            return L2524;
        default:
            return L232c;
        }
    }
    return L22d8;
}

void OvlFunc_895_2008154(void)
{
    int a, b;

    a = 3;
    b = 2;
    __CutsceneStart();
    __PlaySound(0xb5);
    __CopyMapTiles(0x10, 0x1c, 0x15, 3, a, b);
    __WaitFrames(0xa);
    __CopyMapTiles(0x10, 0x1e, 0x15, 3, a, b);
    __WaitFrames(0xa);
    __CopyMapTiles(0x10, 0x20, 0x15, 3, a, b);
    __WaitFrames(0xa);
    __Func_8092b08(0, 2);
    __MapActor_SetSpeed(0, 0x9999, 0x4ccc);
    __Func_80921c4(0, 0x78, 0x62);
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, 0, -8);
    __CutsceneWait(0xa);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(2);
    __CutsceneEnd();
}
