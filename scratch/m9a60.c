extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __Func_80933d4(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_945_200c86c(int slot);
extern int OvlFunc_945_200c880(int slot, int v);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);

void OvlFunc_945_2009a60(void)
{
    int x;
    int m;

    x = 0xe0 << 17;
    m = -1;
    if (__GetFlag(0x922)) {
        __CutsceneStart();
        __Func_808e118();
        __Func_80933d4(0x19999, 0x3333);
        OvlFunc_945_200c8ac(x, m, 0x27e0000, 0x10000028);
        __MessageID(0x1d26);
        OvlFunc_945_200c86c(8);
        OvlFunc_945_200c86c(0xa);
        OvlFunc_945_200c880(8, 0xc0 << 6);
        OvlFunc_945_200c86c(8);
        OvlFunc_945_200c880(0xa, 0xd0 << 8);
        OvlFunc_945_200c86c(0xa);
        OvlFunc_945_200c880(9, 0xa0 << 7);
        OvlFunc_945_200c86c(9);
        __Func_8092adc(8, 0, 0x14);
        OvlFunc_945_200c86c(8);
        OvlFunc_945_200c880(9, 0x80 << 8);
        OvlFunc_945_200c86c(9);
        OvlFunc_945_200c86c(0xa);
        OvlFunc_945_200c86c(8);
        OvlFunc_945_200c880(0xa, 0xb0 << 8);
        OvlFunc_945_200c86c(8);
        __SetFlag(0x92 << 4);
        __CutsceneEnd();
    }
}
