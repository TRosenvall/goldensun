extern void __Func_8092950(int slot, int pal);
extern void __MapActor_SetAnimSpeed(int slot, int s);
extern void __MapActor_SetAnim(int slot, int a);

void OvlFunc_953_200ab1c(void)
{
    __Func_8092950(0xc, 3);
    __Func_8092950(0xd, 0);
    __Func_8092950(0xe, 4);
    __Func_8092950(0xf, 1);
    __Func_8092950(0x10, 5);
    __Func_8092950(0x11, 2);
    __Func_8092950(0x12, 6);
    __MapActor_SetAnimSpeed(0xd, 0xa);
    __MapActor_SetAnimSpeed(0xe, 0x14);
    __MapActor_SetAnim(0xf, 0);
    __MapActor_SetAnimSpeed(0x10, 0x28);
    __MapActor_SetAnimSpeed(0x11, 0x32);
    __MapActor_SetAnimSpeed(0x12, 0x3c);
}
