extern unsigned char *__MapActor_GetActor(int);
extern void __CutsceneStart(void);
extern void __Func_80933f8(int, int, int, int);
extern void __WaitFrames(int);
extern void __MapActor_SetPos(int, int, int);
extern void __SetCameraTarget(int, int);
extern void __MapTransitionIn(void);
extern void __MapActor_SetSpeed(int, int, int);
extern void __Func_80921c4(int, int, int);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int);
extern void __Func_8091e9c(int);
extern void __CutsceneEnd(void);

void OvlFunc_881_20097fc(void)
{
    unsigned char *a;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    *(int *)(a + 0x1c) = 0xa0 << 9;
    *(int *)(a + 0x18) = 0xa0 << 9;
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    __MapActor_SetSpeed(8, 0x6666, 0x3333);
    __Func_80921c4(8, 0x14a8, 0x918);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x66);
    __CutsceneEnd();
}
