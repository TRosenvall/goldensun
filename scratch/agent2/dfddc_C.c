void Func_80dfddc(unsigned char *src, unsigned char *dst, int n, int m)
{
    int i;
    int j;
    int srcoff;
    int k;
    unsigned char *p;
    unsigned char *q;

    i = 0;
    srcoff = 0;
    if (m == 0)
        return;
    k = m;
    do {
        p = dst + k - 1;
        q = src + srcoff;
        for (j = 0; j != n; j++) {
            *p = *q;
            q++;
            p += m;
        }
        i++;
        srcoff += n;
        k--;
    } while (i != m);
}
