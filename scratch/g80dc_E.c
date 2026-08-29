extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_885_20080dc(void)
{
    int m;
    int c1;
    int c2;
    int c3;
    int v;

    c1 = 0xa;
    c2 = 2;
    c3 = 0;
    v = 0xc0 << 8;
    __CutsceneStart();
    if (__GetFlag(0x815)) {
        __MessageID(0x11c4);
        __ActorMessage(0xc, 0);
    } else {
        m = 0xf76;
        __MessageID(m);
        __Func_809280c(0xc, 0, c1);
        __Func_80925cc(0xc, c2);
        __CutsceneWait(6);
        __Func_8092c40(0xc, c3);
        if (__Func_8091c7c(0, 0) == 0)
            __MessageID(m + 1);
        else
            __MessageID(m + 2);
        __Func_809259c(0xc, 3);
        __ActorMessage(0xc, 0);
        __Func_8092adc(0xc, v, 0xa);
    }
    __CutsceneEnd();
}
