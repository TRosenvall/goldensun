extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_8093054(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_Emote(int a, int b, int c);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_926_2008658(void)
{
    int n;
    int m;

    __CutsceneStart();
    __SetFlag(0x894);
    __Func_809280c(9, 0, 0);
    __CutsceneWait(0xa);
    __MessageID(0x17b7);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(0, 0x80 << 8, 0x14);
    __Func_8093054(9, 0);
    __CutsceneWait(0xa);
    __MapActor_Emote(9, 0x80 << 1, 0x50);
    __Func_8092adc(9, 0xd0 << 8, 0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __Func_8093040(9, 0, 0x14);
    __Func_8092adc(9, 0, 0x14);
    __MapActor_DoAnim(9, 3);
    __CutsceneWait(0x14);
    __Func_8093040(9, 0, 0x14);
    n = 0xa;
    m = 0x18;
    __Func_8010704(0xa, 0x1a, 1, 1, n, m);
    __CutsceneEnd();
}
