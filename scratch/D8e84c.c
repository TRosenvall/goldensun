extern unsigned char L7570[] __asm__(".L7570");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008e84(void)
{
    int z;

    z = 0;
    __PlaySound(0x9e);
    __Func_8010560(L7570, 0x2d, 0x27);
    __Func_809218c(z, 0x83 << 1, 0x325);
    __Func_8091e9c(6);
}
