extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_80922c4(int a, int b, int c);
extern void __PlaySound(int id);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);

void OvlFunc_939_2008c74(void)
{
    int a;
    int b;
    int v;

    __SetFlag(0x242);
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x3333, 0x1999);
    __MapActor_GetActor(0)[0x55] = 0;
    __MapActor_SetAnim(0, 2);
    v = 8;
    __Func_80922c4(0, 0, -v);
    __PlaySound(0x9e);
    a = 0x29;
    b = 4;
    __Func_80105d4(0x35, 4, 2, 2, a, b);
    __CutsceneWait(0xa);
    __Func_80105d4(0x35, 6, 2, 2, a, b);
    __CutsceneWait(0xa);
    __Func_8091e9c(1);
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
}
