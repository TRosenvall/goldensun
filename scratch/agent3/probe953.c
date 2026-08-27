extern void __CutsceneStart(void);
extern void __MapActor_SetSpeed(int, int, int);
extern void __MapTransitionIn(void);
extern void __MapActor_SetAnim(int, int);
extern void __Func_8092158(int, int, int);
extern void __MapActor_TravelTo(int, int, int);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern int  __GetFlag(int);
extern void __Func_8091e9c(int);

void OvlFunc_953_200a5f0(void)
{
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x19999, 0xcccc);
    __MapTransitionIn();
    __MapActor_SetAnim(0, 2);
    __Func_8092158(0, 0xc3 << 2, 0xd6 << 1);
    __Func_8092158(0, 0xdc << 2, 0xd7 << 1);
    __MapActor_TravelTo(0, 0xf5 << 2, 0xd8 << 1);
    __MapTransitionOut();
    __WaitMapTransition();
    if (__GetFlag(0x90f))
        __Func_8091e9c(0x20);
    else
        __Func_8091e9c(0xc);
}
