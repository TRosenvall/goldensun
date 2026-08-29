extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __Func_8092adc(int a, int b, int c);
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void __MapActor_SetBehavior(int slot, void *b);
extern unsigned char gScript_882__0200cec8[];

void OvlFunc_882_200ad28(void)
{
    unsigned char *a;
    int w1;
    unsigned int r;

    w1 = 0x80 << 6;
    __CutsceneStart();
    __MapActor_SetAnim(0xa, 1);
    __CutsceneWait(0xa);
    __Func_8092848(0xa, 0, 0x14);
    if (__GetFlag(0x30d) != 0) {
        __MessageID(0xea5);
        __Func_8093040(0xa, 0, 0xa);
    } else {
        __MessageID(0xea4);
        __Func_809259c(0xa, 1);
        __Func_8093040(0xa, 0, 0xa);
        __Func_809259c(0xa, 2);
        __Func_8093040(0xa, 0, 0xa);
    }
    __Func_8092adc(0xa, w1, 0x14);
    __MapActor_SetAnim(0xa, 5);
    __CutsceneWait(0xa);
    a = __MapActor_GetActor(0xa);
    r = __Random() % 0x5a;
    a += 0x64;
    *(short *)a = r + 0x3c;
    __MapActor_SetBehavior(0xa, gScript_882__0200cec8);
    __CutsceneWait(0x14);
    __SetFlag(0x30d);
    __CutsceneEnd();
}
