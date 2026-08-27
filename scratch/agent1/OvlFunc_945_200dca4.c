extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200c890(int a, int b, int c, int d);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_DoAnim(int a, int b);

void OvlFunc_945_200dca4(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xf, 1, 0);
    OvlFunc_945_200c890(9, 0xea << 1, 0x9a << 2, 0x80 << 8);
    OvlFunc_945_200c8e8(8, 1, 0x14);
    __Func_80925cc(9, 2);
    __CutsceneWait(0x14);
    __Func_8092adc(8, 0xd0 << 8, 0x50);
    __Func_8092adc(8, 0, 0x14);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    OvlFunc_945_200c8e8(9, 0x15, 0);
}
