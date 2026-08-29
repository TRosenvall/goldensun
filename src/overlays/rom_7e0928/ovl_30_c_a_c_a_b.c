extern unsigned char *__MapActor_GetActor(int slot);
extern int __Func_8011f54(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int flag);

void OvlFunc_956_20085e0(void)
{
    unsigned char *a;
    int i;
    int n;
    int m;

    for (i = 0xf; i <= 0x11; i++) {
        a = __MapActor_GetActor(i);
        if (__Func_8011f54(0, *(int *)(a + 8), *(int *)(a + 0x10)) == 0) {
            a[0x23] = 2;
            a[0x55] = 0;
            m = *(int *)(a + 8);
            n = *(int *)(a + 0x10);
            m >>= 20;
            n >>= 20;
            __Func_8010704(0x53, 0xd, 1, 1, m, n);
            n = *(int *)(a + 0x10);
            m = *(int *)(a + 8);
            n >>= 20;
            m >>= 20;
            n += 0x34;
            __Func_8010704(0x53, 0xd, 1, 1, m, n);
            __SetFlag(i + 0x205);
        }
    }
}
