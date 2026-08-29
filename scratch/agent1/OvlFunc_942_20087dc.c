extern unsigned char gState[];
extern int _AREA_6b;
extern int _AREA_70;
extern unsigned char gScript_930__020096b8[];
extern unsigned char L16ce[] __asm__(".L16ce");
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8010560(void *p, int a, int b);
extern void __Func_8091e9c(int a);

void OvlFunc_942_20087dc(void)
{
    unsigned char *gs;
    int v;
    int a1;
    int a2;

    a1 = 0x98 << 1;
    a2 = 0xae << 3;
    __CutsceneStart();
    __PlaySound(0x9e);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __Func_8092b08(0, 3);
    gs = gState;
    v = *(short *)(gs + 0x1c0);
    if (v == (int)(&_AREA_6b)) {
        __Func_809218c(0, a1, a2);
        __Func_8010560(gScript_930__020096b8, 0x4e, 0x56);
    } else if (v == (int)(&_AREA_70)) {
        __Func_809218c(0, 0xf8, 0xc0);
        __Func_8010560(L16ce, 0x4a, 9);
    }
    __CutsceneWait(0x10);
    __Func_8091e9c(3);
    __CutsceneEnd();
}
