extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_809280c(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_959_200a52c(void)
{
    unsigned char *a;

    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(2, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(3, *(int *)(a + 8), *(int *)(a + 0x10));
    a = __MapActor_GetActor(0);
    if (a != 0)
        __MapActor_SetPos(1, *(int *)(a + 8), *(int *)(a + 0x10));
    __Func_8092adc(0, 0, 0);
    __MapActor_SetSpeed(2, 0xb333, 0x5999);
    __Func_809218c(2, 0xe4 << 1, 0xc0);
    __MapActor_SetSpeed(3, 0xb333, 0x5999);
    __Func_809218c(3, 0xdc << 1, 0xb8);
    __MapActor_SetSpeed(1, 0xb333, 0x5999);
    __Func_809218c(1, 0xe0 << 1, 0xf0);
    __MapActor_WaitMovement(2);
    __Func_809280c(2, 0xc, 0);
    __MapActor_WaitMovement(1);
    __MapActor_WaitMovement(3);
    __Func_809280c(1, 0xc, 0);
    __Func_809280c(3, 0xc, 0);
    __CutsceneWait(0xf);
}
