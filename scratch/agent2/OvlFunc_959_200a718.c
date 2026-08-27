extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __Func_809218c(int slot, int a, int b);
extern void __MapActor_WaitMovement(int slot);
extern void __Func_8092adc(int slot, int a, int b);

void OvlFunc_959_200a718(void)
{
    __MapActor_SetSpeed(2, 0x10000, 0x8000);
    __Func_809218c(2, 0x1f8, 0xd8);
    __MapActor_SetSpeed(3, 0x10000, 0x8000);
    __Func_809218c(3, 0x1b8, 0xe8);
    __MapActor_SetSpeed(1, 0x10000, 0x8000);
    __Func_809218c(1, 0x1e0, 0xe0);
    __MapActor_WaitMovement(1);
    __Func_8092adc(1, 0xc000, 0);
    __MapActor_WaitMovement(2);
    __Func_8092adc(2, 0xc000, 0);
    __MapActor_WaitMovement(3);
    __Func_8092adc(3, 0xc000, 0);
    __Func_8092adc(0, 0xc000, 0);
}
