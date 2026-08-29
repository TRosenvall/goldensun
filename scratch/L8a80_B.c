extern unsigned char *iwram_3001ebc;
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_901_2008a80(int a, int b, int c)
{
    unsigned int p;
    int x;
    int y;

    x = 0x80 << 8;
    y = 0x80 << 7;
    __MapActor_SetSpeed(0, x, y);
    __Func_809218c(0, a, b);
    p = (unsigned int)iwram_3001ebc;
    p += 0xe4 << 1;
    *(int *)p = 0x10;
    __Func_8091e9c(c);
}
