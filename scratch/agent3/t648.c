extern void __CutsceneStart(void);
extern void __Func_8092848(int, int, int);
extern void __MessageID(int);
extern void OvlFunc_953_2009c48(int);
extern void __Func_8092adc(int, int, int);
extern void __MapActor_DoAnim(int, int);
extern void OvlFunc_953_2009c5c(int, int);
extern void __CutsceneEnd(void);

void OvlFunc_953_2008648(void)
{
    __CutsceneStart();
    __Func_8092848(0x12, 0, 0x14);
    __MessageID(0x2122);
    OvlFunc_953_2009c48(0x12);
    __Func_8092adc(0x12, 0xd0 << 8, 0x14);
    __Func_8092adc(0x12, 0xb0 << 8, 0x14);
    __Func_8092adc(0x12, 0x80 << 8, 0x28);
    __Func_8092848(0x12, 0, 0x14);
    OvlFunc_953_2009c48(0x12);
    __MapActor_DoAnim(0x12, 3);
    OvlFunc_953_2009c48(0x12);
    OvlFunc_953_2009c5c(0x12, 0xa0 << 7);
    __CutsceneEnd();
}
