extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_909_2008150(void)
{
    int e;

    __CutsceneStart();
    e = 0x81 << 1;
    __MapActor_Emote(0xe, e, 0);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x28);
    __MessageID(0x1764);
    __Func_8093040(0xe, 0, 0x14);
    __Func_809280c(0xe, 0, 0);
    __CutsceneWait(0x14);
    __Func_8093040(0xe, 0, 0xa);
    __Func_8092adc(0xe, 0xb0 << 8, 0xa);
    __CutsceneEnd();
}
