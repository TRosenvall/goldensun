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
    int slot;

    slot = 0xe;
    __CutsceneStart();
    __MapActor_Emote(slot, 0x81 << 1, 0);
    __Func_80925cc(slot, 2);
    __CutsceneWait(0x28);
    __MessageID(0x1764);
    __Func_8093040(slot, 0, 0x14);
    __Func_809280c(slot, 0, 0);
    __CutsceneWait(0x14);
    __Func_8093040(slot, 0, 0xa);
    __Func_8092adc(slot, 0xb0 << 8, 0xa);
    __CutsceneEnd();
}
