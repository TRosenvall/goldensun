extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_Emote(int slot, int e, int n);
extern void __MapTransitionOut(void);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_959_2009ab0(void)
{
    int id;

    __CutsceneStart();
    __Func_809228c(9, 0, 0);
    __MapActor_SetBehavior(9, 1);
    __MapActor_SetIdle(9);
    __MapActor_SetAnim(9, 0);
    __MapActor_SetBehavior(0, 1);
    id = 0x240d;
    __MessageID(id);
    __ActorMessage(9, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x3c);
    __MessageID(id + 1);
    __ActorMessage(9, 0);
    __Func_8091e9c(0x3c);
    __MapTransitionOut();
    __CutsceneEnd();
}
