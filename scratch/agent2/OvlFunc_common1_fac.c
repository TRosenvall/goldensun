extern short gL13[] __asm__(".L13");
extern short gL3[] __asm__(".L3");

extern void __PlaySound(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_common1_88c(int a);
extern int OvlFunc_common1_e10(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern int __Func_80f954c(void);
extern void __WaitFrames(int n);
extern void __MapTransitionOut(void);

void OvlFunc_common1_fac(int a)
{
    int t;
    int b;
    int e;
    short *p;
    short *q;

    __PlaySound(0xf7);
    __MapTransitionIn();
    __WaitMapTransition();
    q = gL13;
    t = a * 60;
    q[13] = t;
    p = gL3;
    b = a;
    if (a < 0)
        b = -a;
    p[13] = b * 60;
    if (a < 0) {
        __CutsceneWait(0x1e);
        __PlaySound(0x56);
        OvlFunc_common1_88c(8);
        OvlFunc_common1_e10(3, 1);
        __CutsceneWait(a * -60 + 0x3c);
        e = 0;
    } else {
        __CutsceneWait(0x1e);
        __PlaySound(a + 0x5a);
        OvlFunc_common1_88c(4);
        OvlFunc_common1_e10(3, 0);
        __CutsceneWait(t + 0x3c);
        e = 8;
    }
    __MapActor_Emote(e, 0x105, 0);
    while (__Func_80f954c() != 0)
        __WaitFrames(1);
    __PlaySound(0x13);
    __CutsceneWait(0x1e);
    __PlaySound(0x121);
    __MapTransitionOut();
    __WaitMapTransition();
}
