extern unsigned char *iwram_3001e8c;
extern void *__Func_8004970(int size);
extern int __DecompressLZ(void *src, void *dst);
extern void __free(void *p);

struct S {
    unsigned char pad00[8];
    unsigned short f08;
    unsigned char pad0a[2];
    unsigned short f0c;
    unsigned short f0e;
};

void OvlFunc_880_2008cfc(struct S *s, void *src)
{
    unsigned char *base;
    void *buf;
    short *d1;
    short *d2;
    int off;
    int i, j;

    base = iwram_3001e8c;
    buf = __Func_8004970(0xc0 << 2);
    __DecompressLZ(src, buf);
    off = ((s->f0e << 5) + s->f0c) << 1;
    d1 = (short *)(0x6002000 + off);
    base += off;
    d2 = (short *)base;
    for (i = 0; i <= 7; i++) {
        for (j = 0; j <= 0xf; j++) {
            short v = (s->f08 * i + j) | ~0xfff;
            *d1 = v;
            *d2 = v;
            d1++;
            d2++;
        }
        d1 += 0x10;
        d2 += 0x10;
    }
    __free(buf);
}
