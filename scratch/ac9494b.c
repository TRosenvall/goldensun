extern unsigned char gState[];
extern int _AREA_7e;
extern unsigned char gOvl_0200b2bc[];
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_946_2009494(void)
{
    unsigned char *gp;
    int n;

    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __MapTransitionIn();
    __WaitMapTransition();
    gp = gState;
    __SetFlag(*(short *)(gp + (0xe0 << 1)) + 0x8c8 - (int)&_AREA_7e);
    __CutsceneWait(0x1e);
    __Func_8010560(gOvl_0200b2bc, 0x2c, 7);
    n = -0x10;
    __Func_8092208(0, 3, n);
    __Func_8091e9c(3);
    __CutsceneEnd();
}
