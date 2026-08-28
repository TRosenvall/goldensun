extern void __MapActor_SetSpeed(int slot, int a, int b);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_80921c4(int slot, int a, int b);

void OvlFunc_933_2009874(void)
{
    __MapActor_SetSpeed(8, 0x80 << 8, 0x80 << 7);
    __MapActor_SetAnim(8, 1);
    __Func_80921c4(8, 0xa8, 0x60);
    __MapActor_SetAnim(8, 2);
}
