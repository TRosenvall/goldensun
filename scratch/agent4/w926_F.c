extern void __Func_8092adc(int, int, int);
extern void __MapActor_Emote(int, int, int);
extern void __Func_8093040(int, int, int);
extern void __Func_8010704(int, int, int, int, int, int);

void OvlFunc_926_2008658(void)
{
    int a0, a1, a2, a3;
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
    a0 = 0xa;
    a1 = 0x1a;
    a2 = 1;
    a3 = 1;
    s0 = 0xa;
    s1 = 0x18;
    __Func_8010704(a0, a1, a2, a3, s0, s1);
    __CutsceneEnd();
}
