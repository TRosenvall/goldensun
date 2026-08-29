extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_80925cc(int slot, int n);
extern void __Func_8092adc(int slot, int a, int b);

void OvlFunc_966_2009090(void)
{
    __MessageID(0x28b0);
    __CutsceneWait(0x14);
    __Func_80925cc(0x16, 2);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_Emote(0, 0x80 << 1, 0);
    __MapActor_Emote(1, 0x80 << 1, 0);
    __MapActor_Emote(3, 0x80 << 1, 0);
    __MapActor_Emote(2, 0x80 << 1, 0x37);
    __CutsceneWait(0xa);
    __Func_80925cc(1, 2);
    __CutsceneWait(0x14);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0x16, 4);
    __CutsceneWait(0x14);
    __ActorMessage(0x16, 0);
    __CutsceneWait(0xa);
    __MapActor_Emote(3, 0x81 << 1, 0x28);
    __ActorMessage(3, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(1, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(1, 0);
    __CutsceneWait(0xa);
    __Func_8092adc(2, 0x80 << 8, 0);
    __CutsceneWait(0x41);
    __Func_8092adc(2, 0xc0 << 8, 0);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(2, 3);
    __CutsceneWait(0x1e);
    __ActorMessage(2, 0);
}
