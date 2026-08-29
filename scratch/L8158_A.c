extern unsigned char *iwram_3001ebc;
extern short L1ca8[] __asm__(".L1ca8");
extern unsigned char L1cee[] __asm__(".L1cee");
extern unsigned char L1cd8[] __asm__(".L1cd8");
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Func_8010560(unsigned char *s, int a, int b);
extern void __Func_80922c4(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);

void OvlFunc_966_2008158(void)
{
    unsigned char *base;
    unsigned char *base2;
    unsigned char *a;
    unsigned int off;
    unsigned int off2;
    unsigned int q;
    unsigned int q2;
    int v;
    int x, y;
    int m1, m2;

    m1 = -0x10;
    m2 = -0x10;
    base = iwram_3001ebc;
    off = 0xb6;
    off <<= 1;
    q = (unsigned int)base;
    q += off;
    v = *(short *)q;
    x = L1ca8[v * 2];
    y = L1ca8[v * 2 + 1];
    a = __MapActor_GetActor(0);
    a += 0x55;
    *a = 2;
    __PlaySound(0x9e);
    if (v == 6) {
        __Func_8010560(L1cee, (unsigned short)x, (unsigned short)y);
        __Func_80922c4(0, 0, m1);
    } else {
        __Func_8010560(L1cd8, (unsigned short)x, (unsigned short)y);
        __Func_8092208(0, 2, m2);
    }
    __CutsceneWait(0xa);
    base2 = iwram_3001ebc;
    off2 = 0xe4;
    off2 <<= 1;
    q2 = (unsigned int)base2;
    q2 += off2;
    *(int *)q2 = 0x10;
    __Func_8091e9c(v);
}
