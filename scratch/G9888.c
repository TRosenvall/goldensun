extern unsigned char gScript_881__0200d158[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __SetFlag(int id);
extern void __Func_8091e9c(int n);

void OvlFunc_881_2009888(void)
{
    unsigned char *a;
    short *w;
    int n1;
    int n2;
    int n3;
    int v;

    n1 = -1;
    n2 = -1;
    n3 = -1;
    a = __MapActor_GetActor(8);
    __CutsceneStart();
    __Func_80933f8(n1, n2, n3, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x1f080000, 0xc8 << 16);
    v = 0xa0 << 9;
    *(int *)(a + 0x18) = v;
    *(int *)(a + 0x1c) = v;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    __MapActor_SetSpeed(8, 0x9999, 0x4ccc);
    w = (short *)(a + 0x64);
    *w = 0;
    __MapActor_SetBehavior(8, gScript_881__0200d158);
    do {
        __WaitFrames(1);
    } while (*w == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x67);
    __CutsceneEnd();
}
