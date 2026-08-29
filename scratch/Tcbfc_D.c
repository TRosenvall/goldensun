extern int _MSG_242e;
extern int _MSG_2430;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_WaitScript(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_959_200cbfc(void)
{
    int m;

    if (__GetFlag(0x226)) {
        __MessageID(0x2434);
        __ActorMessage(0x14, 0);
        return;
    }
    __CutsceneStart();
    __Func_809280c(0x14, 0, 0);
    if (__GetFlag(0x227) == 0) {
        __MapActor_Jump(0x14, 4, 0);
        __MapActor_SetIdle(0x14);
        __MapActor_WaitScript(0x14);
        __CutsceneWait(0x14);
        m = (int)&_MSG_242e;
        __MessageID(m);
        __ActorMessage(0x14, 0);
        __MapActor_Emote(0x14, 0x81 << 1, 0x1e);
        m += 1;
        __MessageID(m);
        __ActorMessage(0x14, 0);
        __CutsceneWait(0x1e);
        __MapActor_SetAnim(0x14, 4);
        __CutsceneWait(0x1e);
    }
    m = (int)&_MSG_2430;
    __MessageID(m);
    __ActorMessage(0x14, 0);
    __MapActor_Emote(0x14, 0x101, 0x28);
    __MessageID(m + 1);
    __Func_8092c40(0x14, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(m + 2);
        __Func_8092c40(0x14, 0);
        __SetFlag(0x226);
    } else {
        __MessageID(m + 3);
        __Func_8092c40(0x14, 0);
    }
    __SetFlag(0x227);
    __CutsceneEnd();
}
