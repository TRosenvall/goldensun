extern void __Func_8078a08(int a);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void __SetFlag(int id);
extern void __CutsceneEnd(void);

void OvlFunc_898_20092c0(void)
{
    __Func_8078a08(0xe7);
    __CutsceneStart();
    __CutsceneWait(0xa);
    __Func_80925cc(0x13, 2);
    __MapActor_SetSpeed(0x13, 0xcccc, 0x6666);
    __Func_80921c4(0x13, 0xd8, 0xcc << 1);
    __CutsceneWait(0xa);
    __Func_8092adc(0x13, 0x80 << 7, 0x14);
    __MapActor_Jump(0x13, 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x13, 6, 0);
    __CutsceneWait(0x1e);
    __MapActor_Jump(0x13, 6, 0);
    __CutsceneWait(0x1e);
    __Func_80921c4(0x13, 0xd8, 0xc4 << 1);
    __CutsceneWait(0xa);
    __Func_8092adc(0x13, 0x80 << 7, 0x14);
    __SetFlag(0x858);
    __CutsceneEnd();
}
