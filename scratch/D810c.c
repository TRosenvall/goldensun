extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_966_200810c(void)
{
    __SetFlag(0x9bb);
    __MessageID(0x28b8);
    __ActorMessage(0x12, 0);
    __MapActor_SetSpeed(0x12, 0x80 << 9, 0x80 << 8);
    __Func_8092304(0x12, -0x10, 0);
    __Func_8092adc(0x12, 0, 0);
    __CutsceneWait(0xa);
}
