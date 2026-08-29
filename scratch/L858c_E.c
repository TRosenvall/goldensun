extern int _MSG_1cb1;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __WaitFrames(int n);

void OvlFunc_901_200858c(void)
{
    unsigned char *a;
    unsigned short *p;
    int saved;
    int m;
    int v;
    int w1;

    w1 = 0x80 << 1;
    a = __MapActor_GetActor(0xe);
    saved = *(short *)(a + 6);
    p = (unsigned short *)(a + 0x64);
    *p = 2 | *p;
    __CutsceneStart();
    m = (int)(&_MSG_1cb1);
    __MessageID(m);
    __MapActor_SetAnim(0xe, 0);
    __Func_8092848(0xe, 0, 2);
    if (__GetFlag(0xc0 << 2) == 0) {
        __MapActor_Emote(0xe, w1, 0x3c);
        __Func_8093040(0xe, 0, 0xa);
        __Func_8093040(0xe, 0, 0xa);
        __SetFlag(0xc0 << 2);
    }
    __MessageID(m + 2);
    __Func_8093040(0xe, 0, 0xa);
    *(short *)(a + 6) = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    v = 1;
    *p = v;
    __SetFlag(0x307);
}
