struct A { unsigned char pad00[0x50]; unsigned char *f50; };

extern void *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern unsigned char L28ac[] __asm__(".L28ac");
extern void __Func_8010560(void *t, int a, int b);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_898_2008ef4(int a, int b, int c);

void OvlFunc_898_200913c(void)
{
    struct A *a;
    unsigned char *m;
    unsigned char *p;
    int e5, e6;

    a = __MapActor_GetActor(0);
    m = a->f50;
    __PlaySound(0x9e);
    __Func_8010560(L28ac, 0x23, 9);
    p = (unsigned char *)a + 0x23;
    e5 = 4;
    e6 = 0xa;
    __Func_8010704(0x21, 0x14, 1, 3, e5, e6);
    *p &= 0xfe;
    m[9] |= 0xc;
    OvlFunc_898_2008ef4(0x48, 0xa0, 0xc);
}
