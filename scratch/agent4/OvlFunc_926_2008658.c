extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int);
extern void __SetFlag(int);
extern void __MessageID(int);
extern void __Func_809280c(int, int, int);
extern void __Func_80925cc(int, int);
extern void __Func_8092adc(int, int, int);
extern void __Func_8093054(int, int);
extern void __MapActor_Emote(int, int, int);
extern void __Func_8093040(int, int, int);
extern void __MapActor_DoAnim(int, int);
extern void __Func_8010704(int, int, int, int, int, int);

void OvlFunc_926_2008658(void)
{
    int s0;
    int s1;

    __CutsceneStart();
    __SetFlag(0x894);
    __Func_809280c(9, 0, 0);
    __CutsceneWait(0xa);
    __MessageID(0x17b7);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0x8000, 0x14);
    __Func_8093054(9, 0);
    __CutsceneWait(0xa);
    __MapActor_Emote(9, 0x100, 0x50);
    __Func_8092adc(9, 0xd000, 0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __Func_8093040(9, 0, 0x14);
    __Func_8092adc(9, 0, 0x14);
    __MapActor_DoAnim(9, 3);
    __CutsceneWait(0x14);
    __Func_8093040(9, 0, 0x14);
    s0 = 0xa;
    s1 = 0x18;
    __Func_8010704(0xa, 0x1a, 1, 1, s0, s1);
    __CutsceneEnd();
}
