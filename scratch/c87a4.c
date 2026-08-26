struct E { unsigned char pad00[6]; short f6; };

extern struct E *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_RunScript(int slot, void *s);
extern void __Func_80b0278(int shop, int slot);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80933d4(int a, int b);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void __Func_8093530(void);
extern unsigned char gScript_921__0200a5ec[];

void OvlFunc_921_20087a4(void)
{
    struct E *e;
    short v;

    e = __MapActor_GetActor(0);
    v = e->f6;
    if (__GetFlag(0x881)) {
        if ((unsigned short)(v - 0xa001) <= 0x3ffe) {
            __Func_80b0278(0xa, 0xc);
            return;
        }
        __CutsceneStart();
        __Func_809280c(0xc, 0, 0);
        __CutsceneWait(0xa);
        __MessageID(0x164b);
        __ActorMessage(0xc, 0);
        __Func_8092adc(0xc, 0x80 << 7, 0xa);
        __CutsceneEnd();
    } else if ((unsigned short)(v - 0xa001) <= 0x3ffe) {
        __CutsceneStart();
        __Func_80933d4(0xc0 << 11, 0xc0 << 8);
        __Func_80933f8(0xd5 << 17, -1, 0xf6 << 17, 1);
        __Func_8093530();
        __CutsceneWait(0x14);
        __MapActor_RunScript(0xc, gScript_921__0200a5ec);
        __MessageID(0x153e);
        __ActorMessage(0xc, 0);
        __Func_80933f8(0xd5 << 17, -1, 0x9a << 18, 1);
        __Func_8093530();
        __CutsceneEnd();
    }
}
