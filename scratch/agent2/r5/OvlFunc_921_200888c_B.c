extern unsigned char *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __Func_80b0278(int a, int b);
extern void __CutsceneStart(void);
extern void __Func_809280c(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneEnd(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern void __MapActor_RunScript(int slot, void *script);
extern unsigned char gScript_921__0200a5ec[];

void OvlFunc_921_200888c(void)
{
    unsigned char *a;
    int f, k;

    k = 0x80 << 7;
    a = __MapActor_GetActor(0);
    f = *(short *)(a + 6);
    if (__GetFlag(0x881)) {
        if ((unsigned int)((f << 16) + 0x5fff0000) <= 0x3ffe0000) {
            __Func_80b0278(0xb, 0xd);
        } else {
            __CutsceneStart();
            __Func_809280c(0xd, 0, 0);
            __CutsceneWait(0xa);
            __MessageID(0x164d);
            __ActorMessage(0xd, 0);
            __Func_8092adc(0xd, k, 0xa);
            __CutsceneEnd();
        }
    } else {
        if ((unsigned int)((f << 16) + 0x5fff0000) <= 0x3ffe0000) {
            __CutsceneStart();
            __Func_80933d4(0xc0 << 11, 0xc0 << 8);
            __Func_80933f8(0xd5 << 17, -1, 0xf6 << 17, 1);
            __Func_8093530();
            __CutsceneWait(0x14);
            __MapActor_RunScript(0xd, gScript_921__0200a5ec);
            __MessageID(0x1543);
            __ActorMessage(0xd, 0);
            __Func_80933f8(0xd5 << 17, -1, 0x9a << 18, 1);
            __Func_8093530();
            __CutsceneEnd();
        }
    }
}
