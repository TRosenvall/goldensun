extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __MessageID(int id);
extern void __MapActor_Emote(int actor, int a, int b);
extern int __Func_80925cc(int actor, int a);
extern void __Func_8093040(int actor, int a, int b);
extern int __Func_809280c(int actor, int a, int b);
extern void __Func_8092adc(int actor, int a, int b);
void OvlFunc_909_2008150(void)
{
    __CutsceneStart();
    __MapActor_Emote(0xe, 0x81 << 1, 0);
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
