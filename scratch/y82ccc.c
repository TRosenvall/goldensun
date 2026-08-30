extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_808e118(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Actor_TravelTo(unsigned char *a, int x, int y, int z);
extern void __WaitFrames(int n);
extern void __MapActor_WaitMovement(int slot);
extern void OvlFunc_932_200b850(int a, int b);
extern void __Func_809202c(void);

void OvlFunc_932_20082cc(int arg)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int d;
    int k;

    a = __MapActor_GetActor(0);
    b = __MapActor_GetActor(8);
    __Func_808e118();
    __CutsceneStart();
    __MapActor_SetAnim(0, 0x16);
    __CutsceneWait(0xa);
    __PlaySound(0x98);
    __MapActor_SetSpeed(0, 0x33333, 0x19999);
    d = *(int *)(b + 0xc) - *(int *)(a + 0xc);
    if (d < 0)
        d = *(int *)(a + 0xc) - *(int *)(b + 0xc);
    k = 0x80 << 11;
    *(int *)(a + 0x28) = ((d >> 14) << 14) + k;
    __MapActor_SetAnim(0, 7);
    __Actor_TravelTo(a, *(int *)(b + 8), *(int *)(b + 0xc), *(int *)(b + 0x10));
    __WaitFrames(0xa);
    p = *(unsigned char **)(a + 0x50);
    p[9] |= 0xc;
    __MapActor_WaitMovement(0);
    while ((*(int *)(b + 0xc) >> 14) < (*(int *)(a + 0xc) >> 14))
        __WaitFrames(1);
    __CutsceneEnd();
    __PlaySound(0x9f);
    OvlFunc_932_200b850(arg, 0);
    __WaitFrames(0x14);
    __Func_809202c();
}
