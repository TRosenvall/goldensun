extern unsigned char *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_808e118(void);
extern void __MessageID(int id);
extern void __CutsceneWait(int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __Func_809280c(int a, int b, int c);
extern void __ActorMessage(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092304(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, void *b);
extern unsigned char ActorCmd_ARRAY_944__02009314[];

void OvlFunc_967_2008308(void)
{
    unsigned char *a;
    int m;
    short w;

    a = __MapActor_GetActor(0);
    m = *(unsigned short *)(a + 6) + (0x80 << 6);
    m &= 0xffffc000;
    w = m;
    __SetFlag(0xc0 << 2);
    __CutsceneStart();
    __Func_808e118();
    __MessageID(0x26ec);
    __CutsceneWait(0x32);
    __MapActor_Emote(0xe, (0x81 << 1), 0x32);
    __Func_809280c(0xe, 0, 0x14);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 4);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0xe, 2);
    __CutsceneWait(0x1e);
    __ActorMessage(0xe, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(0xe, 3);
    __CutsceneWait(0x14);
    __ActorMessage(0xe, 0);
    if ((w << 16) == (0x80 << 24)) {
        __Func_8092304(0, 0, 0x10);
        __Func_8092adc(0, 0xc0 << 8, 0);
        __CutsceneWait(0x14);
    }
    __MapActor_SetBehavior(0xe, ActorCmd_ARRAY_944__02009314);
    __CutsceneEnd();
}
