extern int _MSG_240d;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __MapTransitionOut(void);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __SetFlag(int id);

void OvlFunc_959_200a134(void)
{
    int base;

    __CutsceneStart();
    __Func_809228c(0, 0, 0);
    __MapActor_SetBehavior(0, 1);
    __MapActor_SetAnim(0, 1);
    __Func_809280c(0xc, 0, 0);
    __PlaySound(0x71);
    __MapActor_Emote(0xc, 0x80 << 1, 0x3c);
    base = (int)(&_MSG_240d);
    __MessageID(base);
    __ActorMessage(0xc, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x32);
    __MessageID(base + 1);
    __ActorMessage(0xc, 0);
    __MapTransitionOut();
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x3c);
    __CutsceneEnd();
    __SetFlag(0x89 << 2);
}
