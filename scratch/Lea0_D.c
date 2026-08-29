extern struct { char pad[0x1e]; short v; } L11 __asm__(".L11");

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __MapTransitionIn(void);
extern void __WaitMapTransition(void);
extern void __PlayMapMusic(void);
extern int __Func_80f954c(void);
extern void OvlFunc_common1_88c(int a);
extern void OvlFunc_common1_e10(int a, int b);

void OvlFunc_common1_ea0(int a)
{
    if (a == 0) {
        __CutsceneStart();
        __MapTransitionIn();
        __WaitMapTransition();
        __CutsceneWait(0x1e);
        __PlaySound(0x59);
        OvlFunc_common1_88c(0);
        OvlFunc_common1_e10(1, 0);
        __CutsceneWait(0x78);
        __CutsceneEnd();
        return;
    }
    __PlaySound(0xf7);
    __CutsceneStart();
    __MapTransitionIn();
    __WaitMapTransition();
    L11.v = a * 60;
    __CutsceneWait(0x1e);
    __PlaySound(a + 0x5a);
    OvlFunc_common1_88c(a);
    OvlFunc_common1_e10(1, 0);
    __CutsceneWait(0x78);
    while (__Func_80f954c())
        __WaitFrames(1);
    __PlaySound(0x121);
    OvlFunc_common1_88c(5);
    OvlFunc_common1_e10(2, 0);
    __PlaySound(0xec);
    __CutsceneWait(0x3c);
    OvlFunc_common1_e10(2, 1);
    __PlaySound(0xec);
    __CutsceneWait(0x3c);
    OvlFunc_common1_88c(6);
    OvlFunc_common1_e10(2, 0);
    __PlaySound(0xec);
    __CutsceneWait(0x3c);
    OvlFunc_common1_88c(7);
    OvlFunc_common1_e10(4, 0);
    __PlaySound(0xed);
    __PlayMapMusic();
    __CutsceneEnd();
    __SetFlag(0x123);
}
