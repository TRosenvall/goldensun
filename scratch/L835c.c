extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Surprise(int a, int b);
extern void __Func_809280c(int a, int b, int c);

void OvlFunc_908_200835c(void)
{
    int a1;
    int a3;

    a1 = 0x80 << 8;
    a3 = 0xc0 << 8;
    __CutsceneStart();
    if (__GetFlag(0xc0 << 2) == 0) {
        __MessageID(0x16ff);
        __ActorMessage(0x15, 0);
        __Func_8092adc(0x15, a1, 0x14);
        __ActorMessage(0x15, 0);
        __Func_809259c(0x16, 2);
        __MapActor_Surprise(0x16, 0x81 << 1);
        __CutsceneWait(0x3c);
        __ActorMessage(0x16, 0);
        __CutsceneWait(0xa);
        __SetFlag(0xc0 << 2);
    }
    __Func_809280c(0x15, 0, 0);
    __MessageID(0x1702);
    __ActorMessage(0x15, 0);
    __Func_8092adc(0x15, a3, 0xa);
    __CutsceneEnd();
}
