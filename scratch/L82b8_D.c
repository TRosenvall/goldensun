extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapActor_Surprise(int a, int b);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_921_20082b8(void)
{
    int w1;
    int w2;

    w1 = 0xc0 << 6;
    w2 = 0xc0 << 6;
    __CutsceneStart();
    if (__GetFlag(0x881) != 0) {
        __MessageID(0x163c);
        __ActorMessage(0xa, 0);
        __MapActor_Surprise(0xa, 0x81 << 1);
        __CutsceneWait(0x28);
        __MapActor_SetAnim(0xa, 1);
        __CutsceneWait(0x14);
        __Func_809280c(0xa, 0, 0x14);
        __Func_8093054(0xa, 0);
        __Func_8092adc(0xa, w1, 0xa);
        __MapActor_SetAnim(0xa, 9);
    } else {
        __MessageID(0x152d);
        __ActorMessage(0xa, 0);
        __MapActor_Surprise(0xa, 0x81 << 1);
        __CutsceneWait(0x28);
        __MapActor_SetAnim(0xa, 1);
        __CutsceneWait(0x14);
        __Func_809280c(0xa, 0, 0x14);
        __ActorMessage(0xa, 0);
        __Func_8092adc(0xa, w2, 0xa);
        __MapActor_SetAnim(0xa, 9);
    }
    __CutsceneEnd();
}
