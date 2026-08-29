extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int __GetPartySize(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern int __Func_8093054(int a, int b);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern void __Func_8091e9c(int n);

void OvlFunc_common1_3e4(int slot)
{
    unsigned char *a;
    int x;
    int y;
    int px;
    int pz;

    x = 0x80 << 9;
    y = 0x80 << 8;
    a = __MapActor_GetActor(slot);
    px = *(short *)(a + 0xa);
    pz = *(short *)(a + 0x12);
    __CutsceneStart();
    if (__GetPartySize() <= 1) {
        __MessageID(0x20e5);
        if (__Func_8093054(slot, 0) == 0) {
            __MapActor_SetSpeed(0, x, y);
            __MapActor_SetSpeed(slot, x, y);
            __Func_809218c(slot, px, pz + 0x40);
            __CutsceneWait(0xf);
            __Func_80921c4(0, px, pz);
            __Func_80921c4(0, px, pz + 0x20);
            __MapTransitionOut();
            __WaitMapTransition();
            __Func_8091e9c(0xb);
        }
    } else {
        __MessageID(0x20e8);
        __ActorMessage(slot, 0);
    }
    __CutsceneEnd();
}
