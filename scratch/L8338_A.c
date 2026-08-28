extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __PlaySound(int id);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_909_2008338(void)
{
    unsigned char *base;
    int v;
    int f;
    int x;
    int y;
    int m1;
    int m2;

    m1 = -0x10;
    m2 = -0x10;
    x = 0x80 << 8;
    y = 0x80 << 7;
    base = iwram_3001ebc;
    v = *(short *)(base + (0xb6 << 1));
    f = 0;
    if (v == 9) {
        if (__GetFlag(0x80 << 2) == 0) {
            __PlaySound(0xbc);
            f = 1;
        }
    } else {
        __PlaySound(0x9e);
        f = 1;
    }
    if (f != 0) {
        __Func_80118a8(1);
        __Func_80118a8(2);
    }
    __CutsceneStart();
    __CutsceneWait(0xa);
    __MapActor_SetSpeed(0, x, y);
    __MapActor_SetAnim(0, 2);
    if (*(short *)(base + (0xb6 << 1)) == 9)
        __Func_809228c(0, 0, m1);
    else
        __Func_8092208(0, 3, m2);
    __CutsceneWait(0x10);
    __Func_8091e9c(*(short *)(base + (0xb6 << 1)));
    __CutsceneEnd();
    __Func_80118c0(1);
    __Func_80118c0(2);
}
