extern unsigned char gScript_881__0200d1b8[];
extern unsigned char gScript_881__0200d158[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __SetCameraTarget(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern int OvlFunc_881_200b41c(void);
extern void __MapActor_SetBehavior(int slot, void *s);
extern void __SetFlag(int id);
extern void __Func_8091e9c(int n);

void OvlFunc_881_2009a98(void)
{
    unsigned char *e;

    e = __MapActor_GetActor(8);
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x1f080000, 0xc8 << 16);
    *(int *)(e + 0x18) = 0xa0 << 9;
    *(int *)(e + 0x1c) = 0xa0 << 9;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    __MapActor_SetSpeed(8, 0x9999, 0x4ccc);
    e += 0x64;
    *(short *)e = 0;
    if (OvlFunc_881_200b41c() == 0xb)
        __MapActor_SetBehavior(8, gScript_881__0200d1b8);
    else
        __MapActor_SetBehavior(8, gScript_881__0200d158);
    do {
        __WaitFrames(1);
    } while (*(short *)e == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x6a);
    __CutsceneEnd();
}
