extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_808e118(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);

void OvlFunc_952_200be40(void)
{
    int base;
    int a1, a2, b1, b2, b3;

    a1 = 0xa0 << 7; a2 = 0xa0 << 7;
    b1 = 0xc0 << 6; b2 = 0xc0 << 6; b3 = 0xc0 << 6;
    __SetFlag(0x96c);
    __CutsceneStart();
    __Func_808e118();
    __Func_8092adc(8, a1, 0);
    __Func_8092adc(9, b1, 0);
    __Func_80921c4(0, 0xc8, 0x88 << 1);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __CutsceneWait(0x14);
    base = 0x2233;
    __MessageID(base);
    __Func_8092c40(8, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MessageID(base + 1);
        __ActorMessage(8, 0);
    } else {
        __CutsceneWait(0x14);
        __MessageID(base + 2);
        __ActorMessage(8, 0);
        __CutsceneWait(0x14);
        __Func_8092848(8, 9, 0x3c);
        __Func_8092adc(9, b2, 0);
        __CutsceneWait(0x28);
        __Func_80925cc(9, 2);
        __CutsceneWait(0x1e);
        __Func_8092848(8, 9, 0x1e);
        __MapActor_DoAnim(9, 3);
        __CutsceneWait(0x1e);
        __MapActor_Emote(8, 0x81 << 1, 0x32);
        __Func_8092adc(8, a2, 0);
        __Func_8092adc(9, b3, 0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(8, 4);
        __CutsceneWait(0x14);
        __ActorMessage(8, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        __ActorMessage(8, 0);
    }
    __CutsceneEnd();
}
