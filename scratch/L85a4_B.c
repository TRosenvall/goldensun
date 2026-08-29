extern void __CutsceneStart(void);
extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_808f1c0(int a, int b);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8091a58(int a, int b);
extern void __SetFlag(int id);

void OvlFunc_952_20085a4(void)
{
    int m;
    int n;

    __CutsceneStart();
    __Func_808e118();
    m = 0x2352;
    __MessageID(m);
    n = 1;
    n = -n;
    __ActorMessage(n, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x1e);
    __Func_809280c(0, 0xe, 0x1e);
    __Func_8092c40(0xe, 0);
    if (__Func_8091c7c(0, 0) != 0) {
        __MessageID(m + 2);
        __ActorMessage(0xe, 0);
    } else {
        __CutsceneWait(0x14);
        __MessageID(m + 3);
        __ActorMessage(0xe, 0);
        __CutsceneWait(0xa);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x1e);
        __Func_8092adc(0, 0x80 << 7, 0);
        __CutsceneWait(0x1e);
        __MapActor_SetPos(0x10, 0, 0);
        __Func_808f1c0(0xcd, 3);
        __MapActor_SetAnim(0, 1);
        __Func_8091a58(0xcd, 0);
        __SetFlag(0xf31);
    }
}
