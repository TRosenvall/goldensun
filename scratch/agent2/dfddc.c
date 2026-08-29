void Func_80dfddc(unsigned char *src, unsigned char *dst, int n, int m)
{
    int i;
    int j;
    unsigned char *p;
    unsigned char *q;

    for (i = 0; i != m; i++) {
        p = dst + (m - 1 - i);
        q = src + i * n;
        for (j = 0; j != n; j++) {
            *p = *q;
            q++;
            p += m;
        }
    }
}
