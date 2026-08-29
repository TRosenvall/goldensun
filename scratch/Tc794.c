extern int _MSG_244f;
extern int _MSG_2455;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_80925cc(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_959_200c794(void)
{
    int m;

    __CutsceneStart();
    if (__GetFlag(0x941)) {
        __MessageID(0x2566);
        __ActorMessage(0x12, 0);
        __CutsceneEnd();
        return;
    }
    if (__GetFlag(0x313)) {
        __MessageID(0x2457);
        __Func_8092c40(0x19, 0);
        __CutsceneEnd();
        return;
    }
    __MapActor_Emote(0x19, 0x81 << 1, 0x1e);
    __Func_809280c(0x19, 0, 0);
    m = (int)&_MSG_244f;
    __MessageID(m);
    __ActorMessage(0x19, 0);
    __Func_809280c(0x19, 0x18, 0);
    __Func_8093500(0x18, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __Func_8093500(0, 1);
    __CutsceneWait(0x14);
    __MapActor_Emote(0x19, 0x105, 0x3c);
    __MessageID(m + 1);
    __ActorMessage(0x19, 0);
    __MapActor_Emote(0x19, 0x107, 0x3c);
    __MessageID(m + 2);
    __ActorMessage(0x19, 0);
    __CutsceneWait(0x46);
    __MapActor_Emote(0x19, 0x80 << 1, 0x3c);
    __Func_809280c(0x19, 0, 0);
    __MessageID(m + 3);
    __Func_8092c40(0x19, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(m + 4);
        __Func_8092c40(0x19, 0);
    } else {
        __MessageID(m + 5);
        __Func_8092c40(0x19, 0);
    }
    __CutsceneWait(0x3c);
    __MapActor_Emote(0x19, 0x105, 0x3c);
    m = (int)&_MSG_2455;
    __MessageID(m);
    __Func_8092c40(0x19, 0);
    __Func_80925cc(0x19, 1);
    __MessageID(m + 1);
    __Func_8092c40(0x19, 0);
    m += 2;
    __MapActor_DoAnim(0x19, 3);
    __MessageID(m);
    __Func_8092c40(0x19, 0);
    __SetFlag(0x313);
    __CutsceneEnd();
}
