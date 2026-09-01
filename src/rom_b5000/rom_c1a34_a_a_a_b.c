extern unsigned char Lc5c38[] __asm__(".Lc5c38");
extern unsigned int Random(void);

int Func_80c1fa8(unsigned int id)
{
    int buf[5];
    unsigned char *e;
    unsigned char *p;
    unsigned char *q;
    int *base;
    int *w;
    int n;
    int i;
    int idx;
    int k;

    n = 0;
    if (id >= (0xbe << 1))
        id = 1;
    base = buf;
    e = Lc5c38 + id * 16;
    q = e + 1;
    p = e + 0xb;
    w = base;
    for (i = 4; i >= 0; i--, q++) {
        if (*p++ != 0) {
            *w++ = q[0] + 8;
            n++;
        }
    }
    idx = (Random() * n) >> 16;
    k = idx * 4;
    return *(int *)((char *)base + k);
}
