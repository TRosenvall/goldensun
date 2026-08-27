extern void __CutsceneStart(void);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200c8ac(int a, int b, int c, int d);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_945_200be34(void)
{
    __CutsceneStart();
    OvlFunc_945_200c8e8(0x18, 0, 0);
    OvlFunc_945_200c8e8(0x12, 0, 0);
    __MapActor_SetPos(0, 0, 0);
    __MapActor_SetPos(0x10, 0x96 << 16, 0x24a0000);
    OvlFunc_945_200c8ac(0x9c << 16, -1, 0x86 << 18, 0x1000001);
    OvlFunc_945_200c8e8(8, 0, 0);
    __MapActor_SetSpeed(0x10, 0xcccc, 0x6666);
    __Func_80921c4(0x10, 0xa8, 0x242);
    __Func_80921c4(0x10, 0xa8, 0x22a);
    __Func_8092adc(0x10, 0x80 << 8, 0x14);
    __Func_809259c(0x10, 2);
    __MessageID(0x1e3c);
    __Func_8093040(0x10, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xc, 0);
}
