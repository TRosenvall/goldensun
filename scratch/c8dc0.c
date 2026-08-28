extern unsigned char L7544[] __asm__(".L7544");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008dc0(void)
{
    int a0, a1;

    __PlaySound(0xbc);
    __Func_8010560(L7544, 0x2d, 0xb);
    a0 = 0;
    a1 = 0x101;
    __Func_809218c(a0, a1, 0xd2 << 1);
    __Func_8091e9c(0xb);
}
