extern unsigned char *iwram_3001ebc;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_Emote(int a, int b, int c);
extern int __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_942_20086c8(void)
{
    unsigned char *g;
    unsigned short *p;
    int e1;
    int e2;

    e1 = 0x80 << 1;
    e2 = 0x81 << 1;
    __CutsceneStart();
    if (__GetFlag(0x8a8) != 0) {
        __Func_809280c(0xb, 0, 0);
        __CutsceneWait(0x14);
        __MessageID(0x1f1c);
        __ActorMessage(0xb, 0);
        __CutsceneEnd();
    } else {
        __CutsceneWait(0x14);
        __MapActor_Emote(0xb, e1, 0x32);
        __Func_809280c(0xb, 0, 0);
        __CutsceneWait(0x14);
        __MessageID(0x1f18);
        __ActorMessage(0xb, 0);
        if (__GetFlag(0x8a6) != 0) {
            __CutsceneWait(0x14);
            __MapActor_Emote(0xb, e2, 0x28);
            __Func_8092c40(0xb, 0);
            if (__Func_8091c7c(0, 0) == 0) {
                __CutsceneWait(0x14);
                __ActorMessage(0xb, 0);
                __SetFlag(0x8a8);
            } else {
                __CutsceneWait(0xa);
                g = iwram_3001ebc;
                p = (unsigned short *)(g + (0xec << 1));
                *p += 1;
                __ActorMessage(0xb, 0);
                __CutsceneWait(0xa);
                __Func_8092adc(0xb, 0, 0);
                __CutsceneWait(0x1e);
            }
        } else {
            __CutsceneWait(0xa);
            __Func_8092adc(0xb, 0, 0);
            __CutsceneWait(0x1e);
        }
        __CutsceneEnd();
    }
}
