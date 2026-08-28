extern void __PlaySound(int);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __CutsceneWait(int);
extern void __WaitFrames(int);
extern void __MapActor_Emote(int, int, int);
extern void OvlFunc_common1_88c(int);
extern int __Func_80f954c(void);
extern short tblA[] __asm__(".L13");
extern short tblB[] __asm__(".L3");

void OvlFunc_common1_fac(int n)
{
    int v;
    int t;
    int who;

    __PlaySound(0xf7);
    __MapTransitionIn();
    __WaitMapTransition();
    v = n * 60;
    tblA[13] = v;
    t = n;
    if (n < 0)
        t = -n;
    tblB[13] = t * 60;
    if (n < 0) {
        __CutsceneWait(0x1e);
        __PlaySound(0x56);
        OvlFunc_common1_88c(8);
        OvlFunc_common1_e10(3, 1);
        __CutsceneWait((n - n * 16) * 4 + 0x3c);
        who = 0;
    } else {
        __CutsceneWait(0x1e);
        __PlaySound(n + 0x5a);
        OvlFunc_common1_88c(4);
        OvlFunc_common1_e10(3, 0);
        __CutsceneWait(v + 0x3c);
        who = 8;
    }
    __MapActor_Emote(who, 0x105, 0);
    while (__Func_80f954c() != 0)
        __WaitFrames(1);
    __PlaySound(0x13);
    __CutsceneWait(0x1e);
    __PlaySound(0x121);
    __MapTransitionOut();
    __WaitMapTransition();
}
