extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092848(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __MapActor_Surprise(int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_SetAnim(int a, int b);
extern void __MapActor_TravelTo(int slot, int x, int z);
extern void __MapActor_WaitMovement(int slot);

void OvlFunc_882_2009a64(int px, int pz)
{
    unsigned char *a;
    int v1, v2;
    int s1;

    v1 = 0x80 << 9;
    v2 = 0x80 << 8;
    s1 = 0x81 << 1;
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(0x16, *(int *)(a + 8), *(int *)(a + 0x10));
    __MapActor_SetSpeed(0x16, v1, v2);
    __Func_80921c4(0x16, px, pz);
    __Func_8092848(0, 0x16, 0);
    __CutsceneWait(0x14);
    __MapActor_Surprise(0, s1);
    __CutsceneWait(0x28);
    __MessageID(0xe7d);
    __ActorMessage(0x16, 0);
    __Func_80925cc(0x16, 2);
    __ActorMessage(0x16, 0);
    __MapActor_DoAnim(0, 3);
    __MapActor_SetAnim(0x16, 2);
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_TravelTo(0x16, *(short *)(a + 0xa), *(short *)(a + 0x12));
    __MapActor_WaitMovement(0x16);
    __MapActor_SetPos(0x16, 0, 0);
}
