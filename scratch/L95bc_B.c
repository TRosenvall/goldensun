extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapTransitionIn(void);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8096fb0(int a, int b);
extern void __Func_809728c(void);
extern void __FieldMove(int n);
extern void __Func_8097174(void);

void OvlFunc_939_20095bc(void)
{
    __CutsceneStart();
    __MapTransitionIn();
    __Func_809218c(0, 0x98, 0xa8);
    __MapActor_WaitMovement(0);
    __CutsceneWait(0x14);
    __Func_8096fb0(0x92, 1);
    __Func_80970f8(0, 0);
    __Func_809728c();
    __FieldMove(1);
    __Func_8097174();
    __Func_809218c(0, 0x90, 0xb8);
    __MapActor_WaitMovement(0);
    __Func_809218c(0, 0x58, 0xb8);
    __MapActor_WaitMovement(0);
    __Func_809218c(0, 0x58, 0xc8);
    __MapActor_WaitMovement(0);
    __Func_809218c(0, 0x48, 0xc8);
    __MapActor_WaitMovement(0);
    __Func_809218c(0, 0x48, 0x90 << 1);
    __MapActor_WaitMovement(0);
    __Func_809218c(0, 0x58, 0x90 << 1);
    __MapActor_WaitMovement(0);
    __CutsceneEnd();
}
