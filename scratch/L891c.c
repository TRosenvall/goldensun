extern unsigned char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_899_200c624(int a, int b, int c);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_80925cc(int a, int b);
extern int __Func_8078500(void);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_808f1c0(int a, int b);
extern void __Func_8091a58(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_899_200891c(void)
{
    unsigned short *p;
    unsigned char *base;
    unsigned int off;
    int w1;

    w1 = 0x80 << 7;
    __CutsceneStart();
    OvlFunc_899_200c624(0x12, 0, 2);
    if (__GetFlag(0x85b) == 0) {
        __MessageID(0x137c);
        __Func_8092c40(0x12, 0);
    } else {
        __MessageID(0x1385);
        __Func_8092c40(0x12, 0);
    }
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __ActorMessage(0x12, 0);
        __CutsceneWait(0x14);
        __Func_80925cc(0x12, 2);
        __CutsceneWait(0x14);
        if (__Func_8078500() == 0) {
            __MapActor_DoAnim(0x12, 4);
            __CutsceneWait(0x14);
            __MessageID(0x1384);
            __ActorMessage(0x12, 0);
        } else {
            __Func_808f1c0(0xe7, 3);
            __Func_8091a58(0xe7, 0);
            __SetFlag(0x85b);
        }
    } else {
        base = iwram_3001ebc;
        off = 0xec;
        off <<= 1;
        p = (unsigned short *)(base + off);
        *p = *p + 1;
        __CutsceneWait(0x14);
        __MapActor_DoAnim(0x12, 3);
        __CutsceneWait(0x14);
        __ActorMessage(0x12, 0);
    }
    __Func_8092adc(0x12, w1, 0);
    __CutsceneEnd();
}
