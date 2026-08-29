struct Actor {
    unsigned char pad0[0x18];
    int f18;
    int f1c;
};

extern unsigned char gScript_881__0200d1b8[];
extern unsigned char gScript_881__0200d158[];

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __WaitFrames(int n);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __SetCameraTarget(int slot, int n);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern int OvlFunc_881_200b41c(void);
extern void __MapActor_SetBehavior(int slot, unsigned char *script);
extern void __SetFlag(int id);
extern void __Func_8091e9c(int n);

void OvlFunc_881_2009a98(void)
{
    struct Actor *act;
    short *p;
    int m1;
    int m2;
    int m3;
    int z;

    act = __MapActor_GetActor(8);
    __CutsceneStart();
    m1 = -1;
    m2 = -1;
    m3 = -1;
    __Func_80933f8(m1, m2, m3, 0);
    __WaitFrames(1);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(8, 0x1f080000, 0xc8 << 16);
    act->f18 = 0xa0 << 9;
    act->f1c = 0xa0 << 9;
    __WaitFrames(1);
    __SetCameraTarget(8, 1);
    __MapTransitionIn();
    __MapActor_SetSpeed(8, 0x9999, 0x4ccc);
    p = (short *)act;
    p += 0x32;
    z = 0;
    *p = z;
    if (OvlFunc_881_200b41c() == 0xb) {
        __MapActor_SetBehavior(8, gScript_881__0200d1b8);
    } else {
        __MapActor_SetBehavior(8, gScript_881__0200d158);
    }
    do {
        __WaitFrames(1);
    } while (*(short *)((char *)p + (unsigned int)0) == 0);
    __MapTransitionOut();
    __WaitMapTransition();
    __SetFlag(0x927);
    __Func_8091e9c(0x6a);
    __CutsceneEnd();
}
