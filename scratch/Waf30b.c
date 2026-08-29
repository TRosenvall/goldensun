extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_968_2008058(unsigned int, unsigned int, unsigned int, unsigned int);
extern void __Func_8092708(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_968_200af30(void)
{
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x8000, 0x4000);
    __Func_80921c4(0, 0x82 << 2, 0xb2 << 2);
    __Func_8092adc(0, 0x80 << 7, 0xa);
    OvlFunc_968_2008058(0x82 << 18, 0, 0xc4 << 18, 0xdf);
    __Func_8092708(0, 6, 0);
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x14);
    __CutsceneEnd();
}
