extern unsigned char L755a[] __asm__(".L755a");
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_809218c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_883_2008e54(void)
{
    int v;
    int w;

    v = 0xcb << 1;
    w = 0x2d7;
    __PlaySound(0x9e);
    __Func_8010560(L755a, 0x36, 0x20);
    __Func_809218c(0, v, w);
    __Func_8091e9c(5);
}
