extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_809280c(int a, int b, int c);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_TravelTo(int a, int b, int c);
extern void __MapActor_WaitMovement(int a);
extern void __MapActor_SetPos(int a, int b, int c);
extern void __SetFlag(int id);

void OvlFunc_958_2008f44(void)
{
    unsigned char *e;

    __CutsceneStart();
    __Func_809280c(0xb, 0, 0);
    __Func_809280c(0, 0xb, 0);
    __MapActor_SetAnim(0, 1);
    __CutsceneWait(0xa);
    __Func_8092848(0, 0xb, 0);
    __MessageID(0x23d9);
    __ActorMessage(0xb, 0);
    __MapActor_SetAnim(0xb, 2);
    e = __MapActor_GetActor(0);
    if (e != 0)
        __MapActor_TravelTo(0xb, *(short *)(e + 0xa), *(short *)(e + 0x12));
    __MapActor_WaitMovement(0xb);
    __MapActor_SetPos(0xb, 0, 0);
    __CutsceneWait(0x14);
    __SetFlag(0x9a << 4);
    __CutsceneEnd();
}
