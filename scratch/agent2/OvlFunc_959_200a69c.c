extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809218c(int slot, int a, int b);
extern void __SetCameraTarget(int slot, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetAnim(int slot, int n);

void OvlFunc_959_200a69c(void)
{
    __Func_8093500(0xb, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __MessageID(0x247c);
    __ActorMessage(0xd, 0);
    __MapActor_SetSpeed(0xb, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(0xf, 0x80 << 9, 0x80 << 8);
    __Func_809218c(0xb, 0xec << 1, 0xb4);
    __Func_809218c(0xf, 0xec << 1, 0xb4);
    __SetCameraTarget(0xb, 1);
    __MapActor_WaitMovement(0xb);
    __MapActor_SetAnim(0xb, 4);
    __CutsceneWait(0x1e);
}
