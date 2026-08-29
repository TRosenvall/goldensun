extern unsigned char *iwram_3001ebc;
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __Func_8091e9c(int n);

void OvlFunc_899_20099a4(void)
{
    unsigned int a;
    int x;
    int y;
    int m;
    int n;

    x = 0x80 << 8;
    y = 0x80 << 7;
    m = 0xb6 << 2;
    n = 0xcc << 1;
    __MapActor_SetSpeed(0, x, y);
    __Func_809218c(0, m, n);
    a = (unsigned int)iwram_3001ebc;
    a += 0xe4 << 1;
    *(int *)a = 0x10;
    __PlaySound(0x7b);
    __Func_8091e9c(0xf);
}
