extern int _MSG_240d;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetIdle(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __MapTransitionOut(void);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);

void OvlFunc_959_2009b24(int slot)
{
    int base;

    __CutsceneStart();
    __CutsceneStart();
    __MapActor_Emote(slot, 0x80 << 1, 1);
    __Func_809228c(slot, 0, 0);
    __MapActor_SetBehavior(slot, 1);
    __MapActor_SetAnim(slot, 0);
    __Func_809280c(slot, 0, 0);
    __MapActor_SetAnim(0, 1);
    __Func_809228c(slot, 0, 0);
    __MapActor_SetBehavior(slot, 1);
    __MapActor_SetIdle(slot);
    __MapActor_SetAnim(slot, 0);
    __MapActor_SetBehavior(0, 1);
    base = (int)(&_MSG_240d);
    __MessageID(base);
    __ActorMessage(slot, 0);
    __Func_809280c(0, slot, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x3c);
    __MessageID(base + 1);
    __ActorMessage(slot, 0);
    __MapTransitionOut();
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x3c);
    __CutsceneEnd();
}
