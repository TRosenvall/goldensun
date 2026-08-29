struct A { unsigned char pad00[0x50]; unsigned char *f50; };

extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern unsigned char L1782[] __asm__(".L1782");
extern void __Func_8010560(void *t, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_901_2008a80(int a, int b, int c);

void OvlFunc_901_2008b40(void)
{
    struct A *a;
    unsigned char *m;
    unsigned char *p;
    int e5, e6;

    a = __MapActor_GetActor(0);
    m = a->f50;
    __PlaySound(0x9e);
    __Func_8010560(L1782, 0x36, 0xd);
    p = (unsigned char *)a + 0x23;
    e5 = 0x17;
    e6 = 0xc;
    __Func_8010704(0x21, 0x14, 1, 3, e5, e6);
    *p &= 0xfe;
    m[9] |= 0xc;
    OvlFunc_901_2008a80(0xbc << 1, 0xe0, 8);
}
