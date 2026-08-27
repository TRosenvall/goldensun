extern unsigned char gScript_884__0200ae34[];

extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_SetIdle(int slot);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __CutsceneWait(int n);
extern void __Func_809259c(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __ActorMessage(int actor, int b);
extern void __SetFlag(int id);
extern void __WaitFrames(int n);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);

void OvlFunc_884_2008674(void)
{
    unsigned char *a;
    int v;
    int w;

    a = __MapActor_GetActor(0x15);
    __CutsceneStart();
    v = 0x80 << 24;
    *(int *)(a + 0x38) = v;
    *(int *)(a + 0x3c) = v;
    *(int *)(a + 0x40) = v;
    __MapActor_SetAnim(0x15, 1);
    __MapActor_SetIdle(0x15);
    __MapActor_Emote(0x15, 0x80 << 1, 0x28);
    w = 0xb0 << 8;
    *(unsigned short *)(a + 6) = w;
    __CutsceneWait(0x14);
    __Func_809259c(0x15, 2);
    __MessageID(0x1c94);
    __Func_8093040(0x15, 0, 0x28);
    __Func_809280c(0x15, 0, 0x14);
    __Func_809259c(0x15, 2);
    __ActorMessage(0x15, 0);
    __SetFlag(0x306);
    __MapActor_SetIdle(0x15);
    __WaitFrames(1);
    __MapActor_SetBehavior(0x15, gScript_884__0200ae34);
    __CutsceneEnd();
}
