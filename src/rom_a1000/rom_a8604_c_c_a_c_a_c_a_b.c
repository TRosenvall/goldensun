extern unsigned char *iwram_3001f2c;
extern void *_Func_801eb64(int a, int b, int c, int d, int e);

int Func_80a9cf8(int a)
{
    unsigned char *p;
    void **q;
    int i;
    int n;

    p = iwram_3001f2c;
    i = 0;
    n = 0xa8;
    q = (void **)(p + 0xc8);
    do {
        *q++ = _Func_801eb64(2, i, a, 0xf8, n);
        i++;
    } while (i <= 7);
    return 1;
}
