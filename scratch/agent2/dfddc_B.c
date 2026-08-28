void Func_80dfddc(unsigned char *src, unsigned char *dst, int n, int m)
{
    int i;
    int j;
    int srcoff;
    int k;
    unsigned char *p;
    unsigned char *q;

    k = m;
    srcoff = 0;
    for (i = 0; i != m; i++) {
        p = dst + k - 1;
        q = src + srcoff;
        for (j = 0; j != n; j++) {
            *p = *q;
            q++;
            p += m;
        }
        srcoff += n;
        k--;
    }
}
