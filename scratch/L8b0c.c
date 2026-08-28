extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_WaitMovement(int slot);
extern void __ClearFlag(int id);

void OvlFunc_939_2008b0c(void)
{
    __CutsceneStart();
    __MapActor_SetAnim(0, 1);
    __MessageID(0x24cf);
    __ActorMessage(1, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x64);
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, 0, 0xc);
    __MapActor_WaitMovement(0);
    __MapActor_SetAnim(0, 1);
    __ClearFlag(0x243);
    __CutsceneEnd();
}
