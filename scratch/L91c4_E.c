extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8092848(int a, int b, int c);
extern void __MessageID(int id);
extern void OvlFunc_953_2009c48(int a);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __ActorMessage(int a, int b);
extern unsigned char *__Func_8093554(void);
extern void __WaitFrames(int n);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int n);

void OvlFunc_953_20091c4(void)
{
    unsigned char *a;
    unsigned char *base;
    unsigned char *base2;
    int *p;
    unsigned int off;
    int f;
    int w1;
    int c1, c2, m1;

    w1 = 0xc0 << 6;
    c1 = 0x87 << 18;
    c2 = 0xd0 << 16;
    m1 = -1;
    __CutsceneStart();
    f = __GetFlag(0x8a4);
    if (f != 0) {
        __Func_8092848(0x11, 0, 0x28);
        __MessageID(0x206f);
        OvlFunc_953_2009c48(0x11);
        __Func_8092adc(0x11, w1, 0x14);
    } else {
        __Func_809259c(0x11, 2);
        __MessageID(0x206d);
        __ActorMessage(0x11, 0);
        a = __Func_8093554();
        a += 0x55;
        *a = f;
        __WaitFrames(1);
        __Func_80933d4(0x66666, 0xcccc);
        __Func_80933f8(c1, m1, c2, 1);
        __Func_8093530();
        base = iwram_3001ebc;
        off = 0xe0;
        off <<= 1;
        p = (int *)(base + off);
        off += 0x40;
        *p = off;
        base2 = base;
        off -= 0x38;
        p = (int *)(base2 + off);
        off = 0x20;
        *p = off;
        __MapTransitionOut();
        __WaitMapTransition();
        if (__GetFlag(0x8a3) != 0)
            __Func_8091e9c(0x46);
        else
            __Func_8091e9c(7);
    }
    __CutsceneEnd();
}
