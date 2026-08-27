extern unsigned char *iwram_3001e8c;
extern void *__Func_8004970(int size);
extern void __free(void *p);

struct S {
    unsigned char pad00[0xc];
    unsigned short f0c;
    unsigned short f0e;
};

void OvlFunc_880_2008d74(struct S *s)
{
    unsigned char *base;
    void *buf;
    short *d1;
    short *d2;
    int off;
    int i, j, k, t;

    base = iwram_3001e8c;
    buf = __Func_8004970(0xc0 << 2);
    off = ((s->f0e << 5) + s->f0c) << 1;
    d1 = (short *)(0x6002000 + off);
    base += off;
    d2 = (short *)base;
    i = 0;
    k = 0;
    do {
        t = k + 0x20;
        for (j = 0xf; j >= 0; j--) {
            short v = t | ~0xfff;
            *d1 = v;
            *d2 = v;
            t++;
            d1++;
            d2++;
        }
        d1 += 0x10;
        d2 += 0x10;
        i++;
        k += 0x10;
    } while (i <= 7);
    __free(buf);
}
