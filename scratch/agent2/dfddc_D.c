void Func_80dfddc(unsigned char *src, unsigned char *dst, int n, int m)
{
    int i;
    int j;
    int srcoff;
    int k;
    unsigned char *p;
    unsigned char *q;

    i = 0;
    if (m == 0)
        return;
    srcoff = 0;
    k = m;
    do {
        p = (unsigned char *)(k + (unsigned int)dst - 1);
        q = (unsigned char *)(srcoff + (unsigned int)src);
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
