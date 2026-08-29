extern int __GetFlag(int id);
extern int __CutsceneStart(void);
extern int __CutsceneEnd(void);
extern int __CutsceneWait(int n);
extern int __MessageID(int id);
extern int __ActorMessage(int slot, int a);
extern int __Func_809280c(int a, int b, int c);
extern int __Func_80925cc(int a, int b);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern int __Func_809259c(int a, int b);
extern int __Func_8092adc(int a, int b, int c);

void OvlFunc_885_20080dc(void)
{
    int m;

    __CutsceneStart();
    if (__GetFlag(0x815)) {
        __MessageID(0x11c4);
        __ActorMessage(0xc, 0);
    } else {
        m = 0xf76;
        __MessageID(m);
        __Func_809280c(0xc, 0, 0xa);
        __Func_80925cc(0xc, 2);
        __CutsceneWait(6);
        __Func_8092c40(0xc, 0);
        if (__Func_8091c7c(0, 0) == 0)
            __MessageID(m + 1);
        else
            __MessageID(m + 2);
        __Func_809259c(0xc, 3);
        __ActorMessage(0xc, 0);
        __Func_8092adc(0xc, 0xc0 << 8, 0xa);
    }
    __CutsceneEnd();
}
