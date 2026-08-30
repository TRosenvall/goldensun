extern unsigned char *iwram_3001f2c;

extern int Func_80a10d0(int *q, int b, int c, int d, int e, int f);
extern unsigned char *_Func_801ec6c(int a, int b, int c, int d, int e, int f);
extern void Func_80a33d4(unsigned char *p, int e);
extern void Func_80a9cf8(int e);
extern void Func_80a8604(int e, int a, int c);

void Func_80a8088(int a)
{
    unsigned char *p;
    unsigned char *r;
    int *q;
    int e;
    int ok;
    unsigned char **w;
    unsigned short *h;

    p = iwram_3001f2c;
    e = *(int *)(p + 0x24);
    ok = 0;
    if (e == 0) {
        q = (int *)(p + 0x24);
        ok = Func_80a10d0(q, 0, 5, 0x1e, 0xf, 2);
        e = *q;
    }
    if (ok != 0) {
        r = _Func_801ec6c(a, 0, 0, e, 0, 0);
        w = (unsigned char **)(p + (0xbe << 1));
        *w = r;
        r[0xf] = 0xf0;
        h = (unsigned short *)(p + (0xbe << 1) + 0xa4);
        if (*h == 3)
            Func_80a33d4(p, e);
        Func_80a9cf8(e);
        Func_80a8604(e, a, 0x80 << 1);
    } else {
        Func_80a8604(e, a, 0);
    }
}
