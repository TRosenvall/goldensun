extern unsigned int iwram_3001f30;
extern void __Func_8096fb0(int a, int b);
extern void __Func_80970f8(int a, int b);
extern void __Func_809728c(void);
extern void __FieldMove(int a);
extern void __Func_8097174(void);

void OvlFunc_903_2008d68(void)
{
    unsigned char *p;
    unsigned char v;

    p = (unsigned char *)iwram_3001f30;
    __Func_8096fb0(0x4e, 1);
    __Func_80970f8(2, 0xf);
    p += 0x71c;
    v = 8;
    *p |= v;
    __Func_809728c();
    __FieldMove(1);
    __Func_8097174();
}
