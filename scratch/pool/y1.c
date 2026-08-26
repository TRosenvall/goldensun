extern unsigned char iwram_3001ebc[];
extern void __MapActor_SetSpeed(unsigned int, int, int);
extern void __Func_809218c(int, int, int);
extern void __Func_8091e9c(int);

void OvlFunc_898_2008ef4(int a, int b, int c)
{
    char *base;

    __MapActor_SetSpeed(0, 0x8000, 0x4000);
    __Func_809218c(0, a, b);
    base = *(char **)iwram_3001ebc;
    *(int *)(base + (0xe4 << 1)) = 0x10;
    __Func_8091e9c(c);
}
