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
    int k1;
    int k2;
    int k3;
    int k4;
    int sp1;

    k1 = 0x13;
    k2 = 0x13;
    k3 = 0x13;
    k4 = 0x13;
    sp1 = 0xf0 << 8;
    if (__GetFlag(0x84e) == 0)
        return;
    __CutsceneStart();
    __Func_809280c(0, 0x13, 0);
    __MapActor_SetSpeed(k1, 0x9999, 0x4ccc);
    __Func_80921c4(k2, 0x26e, 0xbf << 2);
    __Func_8092adc(0x13, sp1, 0x14);
    __MapActor_DoAnim(0x13, 3);
    __MapActor_DoAnim(0x11, 3);
    __CutsceneWait(0x14);
    __Func_809280c(0x13, 0, 0);
    __CutsceneWait(0x14);
    __MapActor_DoAnim(0x13, 3);
    __MessageID(0x1749);
    __Func_8093040(0x13, 0, 0xa);
    __MapActor_SetSpeed(k3, 0xcccc, 0x6666);
    __Func_80921c4(k4, 0x23a, 0x2f6);
    __MapActor_SetPos(0x13, 0, 0);
    base = iwram_3001ebc;
    *(int *)(base + 0x1c0) = 0x209;
    __SetFlag(0x85e);
    __SetFlag(0x333);
    __CutsceneEnd();
}
