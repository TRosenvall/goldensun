extern void __Func_808c4c0(void);
extern void __Func_80936a0(int, int);
extern void __Func_8093710(void);
extern void __Func_808c44c(void);
extern void __Func_80925cc(int, int);
extern void __MessageID(int);
extern void __ActorMessage(int, int);
extern void __CutsceneWait(int);
extern void __PlaySound(int);
extern void __Func_802899c(int, int);
extern void __ClearFlag(int);
extern void __SetFlag(int);
extern void __Func_80aa56c(void);
extern void __MapActor_Jump(int, int, int);
extern void __Func_8091eb0(int, int);

void OvlFunc_881_2009c08(void)
{
    __Func_808c4c0();
    __Func_80936a0(0x80 << 9, 6);
    __Func_8093710();
    __Func_808c44c();
    __Func_80925cc(8, 2);
    __MessageID(0xc66);
    __ActorMessage(8, 0);
    __CutsceneWait(0x1e);
    __PlaySound(0x6f);
    __Func_802899c(0, 2);
    __ClearFlag(0x16f);
    __ClearFlag(0x171);
    __Func_80aa56c();
    __MapActor_Jump(8, 4, 0x1e);
    __MessageID(0xc67);
    __ActorMessage(8, 0);
    __ClearFlag(0x16d);
    __SetFlag(0x173);
    __Func_80aa56c();
    __CutsceneWait(0x1e);
    __Func_8091eb0(0xc, 6);
}
