extern unsigned char *iwram_3001ebc;
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void OvlFunc_945_200c86c(int n);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_DoAnim(int a, int b);

void OvlFunc_945_2008728(void)
{
    unsigned char *base;
    short *p;
    unsigned int off;
    int w;
    int v;

    w = 0xd0 << 8;
    __CutsceneStart();
    if (__GetFlag(0x928) != 0) {
        __MessageID(0x1eb2);
        OvlFunc_945_200c86c(8);
        __Func_8092adc(8, w, 0x3c);
        __MapActor_DoAnim(8, 4);
        OvlFunc_945_200c86c(8);
        __MapActor_DoAnim(8, 3);
    } else if (__GetFlag(0x925) != 0) {
        __MessageID(0x1e06);
        __ActorMessage(8, 0);
    } else if (__GetFlag(0x921) != 0) {
        __MessageID(0x1dcd);
        __ActorMessage(8, 0);
        if (__GetFlag(0x925) == 0 && __GetFlag(0x924) != 0) {
            base = iwram_3001ebc;
            off = 0xb9;
            off <<= 1;
            p = (short *)(base + off);
            v = 1;
            *p = v;
        }
    } else {
        __MessageID(0x1d30);
        __ActorMessage(8, 0);
    }
    __CutsceneEnd();
}
