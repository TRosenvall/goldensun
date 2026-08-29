struct A { unsigned char pad00[0x18]; int f18; int f1c; };

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __SetCameraTarget(int slot, int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_881_20097fc(void)
{
    struct A *a;

    a = __MapActor_GetActor(8);
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    a->f1c = 0xa0 << 9;
    a->f18 = 0xa0 << 9;
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
