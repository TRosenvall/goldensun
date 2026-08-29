int Func_80a3ddc(unsigned char *rec, short *dst)
{
    short *p;
    short *out;
    unsigned short *src;
    int v;
    int off;
    int k;
    int n;

    p = dst + 31;
    do {
        *p = 0;
        p--;
    } while ((int)p >= (int)dst);

    n = 0;
    src = (unsigned short *)(rec + 0xd8);
    off = 0;
    out = dst;
    for (k = 0xe; k >= 0; k--, off += 2) {
        *(short *)(off + (char *)dst) = 0;
        v = *src;
        src++;
        if (v != 0) {
            *out = v;
            n++;
            out++;
        }
    }
    return n;
}
