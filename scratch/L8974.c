extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809280c(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_921_2008974(void)
{
    unsigned char *a;
    int v;
    unsigned int r;
    int w1, w2;

    w1 = 0x80 << 7;
    w2 = 0xa0 << 7;
    a = __MapActor_GetActor(0);
    v = *(short *)(a + 6);
    if (__GetFlag(0x881) != 0) {
        r = (unsigned int)((v << 16) + 0x5fff0000);
        if (r <= 0x3ffe0000) {
            __Func_80b0278(0xc, 0xf);
        } else {
            __CutsceneStart();
            __Func_809280c(0xf, 0, 0);
            __MessageID(0x164f);
            __ActorMessage(0xf, 0);
            __Func_8092adc(0xf, w1, 0);
            __CutsceneEnd();
        }
    } else {
        r = (unsigned int)((v << 16) + 0x5fff0000);
        if (r <= 0x3ffe0000) {
            __CutsceneStart();
            __MessageID(0x1546);
            __ActorMessage(0xe, 0);
            __Func_80b0278(0xc, 0xe);
            __CutsceneEnd();
        } else {
            __Func_809280c(0xe, 0, 0xa);
            __MessageID(0x1547);
            __ActorMessage(0xe, 0);
            __Func_8092adc(0xe, w2, 0xa);
        }
    }
}
