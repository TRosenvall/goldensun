extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __Func_808c4c0(void);
extern void __Func_808c44c(void);
extern void __Func_8093710(void);
extern void __Func_80aa56c(void);
extern void __Func_80936a0(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_802899c(int a, int b);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_881_2009c08(void)
{
    int f1, f2, f3, f4;

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
    f1 = 0x16f;
    __ClearFlag(f1);
    f2 = 0x171;
    __ClearFlag(f2);
    __Func_80aa56c();
    __MapActor_Jump(8, 4, 0x1e);
    __MessageID(0xc67);
    __ActorMessage(8, 0);
    f3 = 0x16f;
    __ClearFlag(f3);
    f4 = 0x171;
    __SetFlag(f4);
    __Func_80aa56c();
    __CutsceneWait(0x1e);
    __Func_8091eb0(0xc, 6);
}
