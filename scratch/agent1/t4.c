extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_TravelTo(int slot, int x, int y);
extern void __MapActor_WaitMovement(int slot);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __Func_8092adc(int a, int b, int c);

void OvlFunc_882_200998c(void)
{
    unsigned char *p;

    __CutsceneStart();
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_SetPos(0x16, *(int *)(p + 8), *(int *)(p + 0x10));
    __MapActor_SetSpeed(0x16, 0x80 << 9, 0x80 << 8);
    __Func_80921c4(0x16, 0x119, 0x1fb);
    __Func_8092848(0x16, 0, 0);
    __CutsceneWait(0x1e);
    __MessageID(0xe7b);
    __ActorMessage(0x16, 0);
    __Func_809280c(0, 0x16, 0);
    __CutsceneWait(0xa);
    __Func_80925cc(0, 1);
    __CutsceneWait(0x14);
    __Func_8092adc(0x16, 0x80 << 7, 0);
    __ActorMessage(0x16, 0);
    __MapActor_SetAnim(0x16, 2);
    p = __MapActor_GetActor(0);
    if (p != 0)
        __MapActor_TravelTo(0x16, *(short *)(p + 0xa), *(short *)(p + 0x12));
    __MapActor_WaitMovement(0x16);
    __MapActor_SetPos(0x16, 0, 0);
    __Func_80921c4(0, 0x80 << 1, 0x205);
    __CutsceneEnd();
}
