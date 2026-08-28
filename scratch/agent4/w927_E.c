extern int *__MapActor_GetActor(int);
extern void __Func_8092b08(int, int);
extern int __MapActor_SetSpeed(int, int, int);
extern void __PlaySound(int);
extern void __Actor_SetSpriteFlags(int *, int);
extern void __Func_8092158(int, int, int);
extern void __MapActor_SetPos(int, int, int);

void OvlFunc_927_2008d90(int a, int b, int c, int d)
{
    int *act;

    act = __MapActor_GetActor(a);
    __Func_8092b08(a, 1);
    __MapActor_SetSpeed(a, 0x30000, 0x18000);
    __PlaySound(0x98);
    act[0x28 / 4] = d;
    act[0x48 / 4] = 0x8000;
    act[0x44 / 4] = 0;
    __Actor_SetSpriteFlags(act, 0);
    __Func_8092158(a, b, c);
    b <<= 16;
    c <<= 16;
    __MapActor_SetPos(a, b, c);
    __Actor_SetSpriteFlags(act, 1);
    act[0x48 / 4] = 0x10000;
}
