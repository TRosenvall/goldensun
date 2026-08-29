extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void OvlFunc_899_20083bc(int n);
extern void __Func_8092adc(unsigned int, unsigned int, unsigned int);

void OvlFunc_899_2008428(void)
{
    __CutsceneStart();
    __MessageID(0x1253);
    OvlFunc_899_20083bc(0xf);
    __Func_8092adc(0xf, 0x80 << 8, 0);
    __CutsceneEnd();
}
