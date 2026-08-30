extern void __PlaySound(int id);
extern int __Func_8017658(int id, int a, int b, int c);
extern int __Func_8017364(void);
extern void __WaitFrames(int n);
extern void __Func_801faa8(void);
extern int __CloseUIBox(int h, int n);
extern void CloseBoxV(int h, int n) __asm__("__CloseUIBox");

int OvlFunc_971_2009228(void)
{
    int h;

    __PlaySound(0x55);
    h = __Func_8017658(0x292c, 5, 4, 1);
    while (__Func_8017364() == 0)
        __WaitFrames(1);
    __Func_801faa8();
    CloseBoxV(h, 1);
    __WaitFrames(1);
    h = __Func_8017658(0x292d, 5, 4, 1);
    while (__Func_8017364() == 0)
        __WaitFrames(1);
    return __CloseUIBox(h, 1);
}
