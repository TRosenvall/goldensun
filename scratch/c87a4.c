extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __Func_80b0278(int a, int b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_809280c(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapActor_RunScript(int slot, void *s);
extern unsigned char gScript_921__0200a5ec[];

void OvlFunc_921_20087a4(void)
{
    unsigned char *a;
    int v;
    unsigned int r;
    int w1;
    int c1, c2, d1, d2;
    int m1, m2;

    w1 = 0x80 << 7;
    c1 = 0xd5 << 17;  c2 = 0xf6 << 17;  m1 = -1;
    d1 = 0xd5 << 17;  d2 = 0x9a << 18;  m2 = -1;
    a = __MapActor_GetActor(0);
    v = *(short *)(a + 6);
    if (__GetFlag(0x881) != 0) {
        r = (unsigned int)((v << 16) + 0x5fff0000);
        if (r <= 0x3ffe0000) {
            __Func_80b0278(0xa, 0xc);
        } else {
            __CutsceneStart();
            __Func_809280c(0xc, 0, 0);
            __CutsceneWait(0xa);
            __MessageID(0x164b);
            __ActorMessage(0xc, 0);
            __Func_8092adc(0xc, w1, 0xa);
            __CutsceneEnd();
        }
    } else {
        r = (unsigned int)((v << 16) + 0x5fff0000);
        if (r <= 0x3ffe0000) {
            __CutsceneStart();
            __Func_80933d4(0xc0 << 11, 0xc0 << 8);
            __Func_80933f8(c1, m1, c2, 1);
            __Func_8093530();
            __CutsceneWait(0x14);
            __MapActor_RunScript(0xc, gScript_921__0200a5ec);
            __MessageID(0x153e);
            __ActorMessage(0xc, 0);
            __Func_80933f8(d1, m2, d2, 1);
            __Func_8093530();
            __CutsceneEnd();
        }
    }
}
