extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __WaitFrames(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __SetDestMap(int map, int n);

void OvlFunc_881_200b84c(void)
{
    unsigned char *gs, *a, *b, *p;
    int i, z;

    gs = gState;
    a = __MapActor_GetActor(*(int *)(gs + 0x1f4));
    b = __MapActor_GetActor(0x36);
    __CutsceneStart();
    __Func_80933f8(-1, -1, -1, 0);
    __PlaySound(0xdb);
    __Actor_SetSpriteFlags(a, 0);
    z = 0;
    b[0x55] = z;
    p = a + 0x55;
    *p = z;
    *(int *)(a + 0x28) = z;
    p += 0xc;
    *p = 1;
    b[0x61] = 1;
    for (i = 0x3b; i >= 0; i--) {
        *(int *)(a + 0x28) += 0x3333;
        *(int *)(b + 0x28) += 0x3333;
        __WaitFrames(1);
    }
    __MapTransitionOut();
    __WaitMapTransition();
    __CutsceneEnd();
    __SetFlag(0x91 << 1);
    __SetDestMap(2, 0x1b);
}
