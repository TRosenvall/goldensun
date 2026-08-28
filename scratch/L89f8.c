extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_Jump(int a, int b, int c);
extern void OvlFunc_901_2008970(int a, int b, int c, int d);
extern void __Func_80921c4(int a, int b, int c);

void OvlFunc_901_20089f8(void)
{
    int a1, b1, b3, c1, c2;

    a1 = 0x81 << 1;
    b1 = 0xc4 << 1;
    b3 = 0xe0 << 11;
    c1 = 0xcc << 1;
    c2 = 0xcc << 1;
    __CutsceneStart();
    __PlaySound(0x64);
    __CutsceneWait(0x28);
    if (__GetFlag(0x867) == 0) {
        __MapActor_Surprise(0x15, a1);
        __MapActor_Jump(0x15, 4, 0);
        __CutsceneWait(0xc);
        __MapActor_Jump(0x15, 4, 0);
        __CutsceneWait(0x14);
        OvlFunc_901_2008970(0x15, b1, 0x68, b3);
        __CutsceneWait(0x14);
        __Func_80921c4(0x15, c1, 0x68);
        __Func_80921c4(0x15, c2, 0x78);
        __SetFlag(0x867);
    }
    __CutsceneEnd();
}
