extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __Func_8079664(int n);
extern void __AddPartyMember(int n);
extern void __CutsceneStart(void);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __WaitFrames(int n);
extern void __SetCameraTarget(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __Func_8092158(int slot, int x, int z);
extern void __Func_8091e9c(int n);

void OvlFunc_953_200a820(void)
{
    unsigned char *a;
    int s1, t1, s2, t2;

    s1 = 0x19999;
    t1 = 0xcccc;
    s2 = 0x19999;
    t2 = 0xcccc;
    if (__GetFlag(5) != 0) {
        __SetFlag(0x16d);
        __Func_8079664(5);
        __AddPartyMember(3);
    }
    __CutsceneStart();
    __MapActor_SetPos(0xb, 0xd9 << 18, 0x93 << 18);
    __WaitFrames(1);
    __SetCameraTarget(0xb, 1);
    __MapActor_SetSpeed(0xb, s1, t1);
    __MapActor_SetSpeed(0, s2, t2);
    a = __MapActor_GetActor(0xb);
    *(unsigned short *)(a + 6) = 0x80 << 8;
    __MapTransitionIn();
    __MapActor_SetAnim(0, 2);
    __MapActor_SetAnim(0xb, 2);
    __MapActor_TravelTo(0, 0xc8 << 2, 0x93 << 2);
    __Func_8092158(0xb, 0xc0 << 2, 0x93 << 2);
    __MapActor_TravelTo(0, 0xaf << 2, 0x93 << 2);
    __Func_8092158(0xb, 0xa7 << 2, 0x93 << 2);
    __MapActor_TravelTo(0, 0x96 << 2, 0x93 << 2);
    __MapActor_TravelTo(0xb, 0x8e << 2, 0x93 << 2);
    __MapTransitionOut();
    __WaitMapTransition();
    __Func_8091e9c(0x15);
}
