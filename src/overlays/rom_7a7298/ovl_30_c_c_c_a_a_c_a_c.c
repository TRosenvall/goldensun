extern int __GetFlag(int);
extern void __SetFlag(int);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int);
extern void __ActorMessage(int, int);
extern void __Func_809280c(int a, int b, int c);
extern int __Func_8093054(int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010560(void *a, int b, int c);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetBehavior(int slot, void *script);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8092adc(int slot, int a, int b);
extern void __Func_809259c(int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_RunScript(int slot, void *script);
extern void __MapActor_Jump(int slot, int a, int b);
extern void __MapActor_Surprise(int slot, int a);
extern unsigned char gScript_921__0200a4f4[];
extern unsigned char gScript_921__0200a564[];
extern unsigned char L31c0[] __asm__(".L31c0");
extern unsigned char L31d6[] __asm__(".L31d6");
extern unsigned char L2508[] __asm__(".L2508");

void OvlFunc_921_2008384(void)
{
    char *a;
    int n;
    int e1;
    int e2;
    int e3;
    int e4;

    e1 = 0x80 << 1;
    e2 = 0xd0 << 8;
    e3 = 0x19999;
    e4 = 0xc0 << 6;
    if (__GetFlag(0x881) != 0) {
        __CutsceneStart();
        __Func_809280c(9, 0, 0);
        __CutsceneWait(0xa);
        __MessageID(0x1644);
        __Func_8093054(9, 0);
        __CutsceneEnd();
        return;
    }
    if (__GetFlag(0x82b) != 0) {
        __CutsceneStart();
        __MapActor_SetAnim(9, 7);
        __Func_8010560(L31c0, 0xa, 0x45);
        __MessageID(0x156c);
        __ActorMessage(9, 0);
        __MapActor_SetAnim(9, 8);
        __Func_8010560(L31d6, 0xa, 0x45);
        __CutsceneEnd();
        return;
    }
    __CutsceneStart();
    a = __MapActor_GetActor(9) + 0x64;
    n = 0xa;
    *(short *)a = n;
    __MapActor_SetBehavior(9, gScript_921__0200a4f4);
    __MessageID(0x1534);
    __ActorMessage(9, 0);
    __MapActor_SetIdle(8);
    __MapActor_Emote(8, e1, 0x28);
    __Func_8092adc(8, e2, 0xa);
    __Func_809259c(8, 2);
    __Func_8093040(8, 0, 0x14);
    __MapActor_SetBehavior(0, gScript_921__0200a564);
    __MapActor_SetSpeed(8, e3, 0xcccc);
    __MapActor_RunScript(8, L2508);
    __CutsceneWait(0x28);
    __MapActor_Jump(8, 2, 0);
    __Func_809259c(8, 2);
    __MapActor_Surprise(8, 0x81 << 1);
    __CutsceneWait(0x3c);
    __Func_8093040(8, 0, 0xa);
    __Func_8092adc(8, e4, 0x14);
    __Func_809259c(8, 2);
    __ActorMessage(8, 0);
    a = __MapActor_GetActor(8) + 0x59;
    *a ^= 2;
    __SetFlag(0x82c);
    __CutsceneEnd();
}
