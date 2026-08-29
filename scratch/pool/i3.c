extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092b08(int slot, int n);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_923_2008f48(int a)
{
    __CutsceneStart();
    __PlaySound(0xe4);
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __Func_8092b08(0, 2);
    __Func_809228c(0, 0, -8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __CutsceneWait(8);
    __MapActor_SetPos(0, (a << 19) + (0x80 << 12), 0);
    __CutsceneWait(0x1e);
}
