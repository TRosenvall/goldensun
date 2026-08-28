extern unsigned char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int a);
extern void __MessageID(int id);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetSpeed(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_SetPos(int a, int b, int c);

void OvlFunc_909_20085f4(void)
{
    unsigned char *base;
    int p1;
    int p2;
    int p3;
    int p4;
    int p5;
    int p6;
    int p7;
    int sp1;

    p1 = 0x9999;
    p2 = 0x4ccc;
    p3 = 0x26e;
    p4 = 0xcccc;
    p5 = 0x6666;
    p6 = 0x23a;
    p7 = 0x2f6;
    sp1 = 0xf0 << 8;
    if (__GetFlag(0x84e) == 0)
        return;
    __CutsceneStart();
    __Func_809280c(0, 0x13, 0);
    __MapActor_SetSpeed(0x13, p1, p2);
    __Func_80921c4(0x13, p3, 0xbf << 2);
    __Func_8092adc(0x13, sp1, 0x14);
    __MapActor_DoAnim(0x13, 3);
    __MapActor_DoAnim(0x11, 3);
    __CutsceneWait(0x14);
    __Func_809280c(0x13, 0, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x13, 3);
    __MessageID(0x1749);
    __Func_8093040(0x13, 0, 0xa);
    __MapActor_SetSpeed(0x13, p4, p5);
    __Func_80921c4(0x13, p6, p7);
    __MapActor_SetPos(0x13, 0, 0);
    base = iwram_3001ebc;
    *(int *)(base + 0x1c0) = 0x209;
    __SetFlag(0x85e);
    __SetFlag(0x333);
    __CutsceneEnd();
}
