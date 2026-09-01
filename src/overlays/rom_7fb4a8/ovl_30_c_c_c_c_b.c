extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);

int OvlFunc_971_2009294(int n)
{
    int i;

    if (n > 999) {
        n = 999;
    }
    for (i = 0; i <= 2; i++) {
        __CopyMapTiles(0x1b, n % 10, 0x10 - i, 8, 1, 1);
        n = n / 10;
    }
    __Func_800fe9c();
}
