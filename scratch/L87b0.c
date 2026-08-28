extern int _MSG_2399;
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_80925cc(int a, int b);

void OvlFunc_950_20087b0(int slot)
{
    int m;

    __CutsceneStart();
    if (__GetFlag(0x8bd) == 0) {
        m = (int)(&_MSG_2399);
        __MessageID(m);
        __Func_8092c40(slot, 0);
        if (__Func_8091c7c(0, 0) == 0) {
            __CutsceneWait(0xa);
            __MessageID(m + 1);
        } else {
            __MessageID(m + 2);
        }
        __ActorMessage(slot, 0);
    } else {
        if (__GetFlag(0x8be) == 0) {
            __SetFlag(0x8be);
            __MessageID(0x239c);
            __ActorMessage(slot, 0);
            __CutsceneWait(0xa);
            __Func_80925cc(slot, 2);
            __CutsceneWait(0x14);
        }
        __MessageID(0x239d);
        __ActorMessage(slot, 0);
    }
    __CutsceneEnd();
}
