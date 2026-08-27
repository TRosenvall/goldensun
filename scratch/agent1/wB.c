extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern unsigned char L1d28[] __asm__(".L1d28");

void OvlFunc_907_2008cb4(void)
{
    unsigned char *a;
    unsigned int i;
    int m;
    int n;
    int x;
    int z;

    a = __MapActor_GetActor(8);
    x = *(int *)(a + 8);
    x >>= 20;
    z = *(int *)(a + 0x10);
    z >>= 20;
    for (i = 0; i <= 0x13; i += 2) {
        m = L1d28[i];
        n = L1d28[i + 1];
        __Func_8010704(1, 0, 1, 1, m, n);
    }
    __Func_8010704(0, 0, 1, 1, x, z);
}
