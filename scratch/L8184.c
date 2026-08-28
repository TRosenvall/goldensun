extern unsigned char *iwram_3001ebc;
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

void OvlFunc_938_2008184(void)
{
    unsigned char *base;
    int m;
    int x;
    int y;

    m = -0x10;
    x = 0x80 << 8;
    y = 0x80 << 7;
    base = iwram_3001ebc;
    __CutsceneStart();
    __CutsceneWait(0xa);
    if (*(short *)(base + (0xb6 << 1)) == 4)
        __PlaySound(0xbc);
    else
        __PlaySound(0x9e);
    __Func_80118a8(1);
    __Func_80118a8(2);
    __CutsceneWait(0xa);
    __MapActor_SetSpeed(0, x, y);
    __MapActor_SetAnim(0, 2);
    if (*(short *)(base + (0xb6 << 1)) == 4)
        __Func_809228c(0, 0, m);
    else
        __Func_8092208(0, 3, m);
    __CutsceneWait(0x10);
    __Func_8091e9c(*(short *)(base + (0xb6 << 1)));
    __Func_80118c0(1);
    __Func_80118c0(2);
    __CutsceneEnd();
}
