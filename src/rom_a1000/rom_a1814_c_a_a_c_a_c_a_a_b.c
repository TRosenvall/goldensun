extern char *iwram_3001f2c;
extern void Func_a1c6c(void *node, int i, int a, int k, int n);

void Func_80a1cb0(int mode)
{
    char *s;
    char *p;
    void **q;
    int i;
    int k;
    int n;

    s = iwram_3001f2c;
    k = 0x38;
    if (mode != 1)
        k = 0x28;
    p = s + 0x48;
    i = 0;
    q = (void **)p;
    n = 5;
    do {
        if (*q++ != 0)
            Func_a1c6c(p, i, 0x74, k, n);
        i++;
        p += 4;
    } while (i <= 0xe);
}
