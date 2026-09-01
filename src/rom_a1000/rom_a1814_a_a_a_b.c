extern void Func_80a10d0(void *p, int b, int c, int d, int e, int f);
extern char *Func_80a1778(void *q, int b, int c);

void *Func_80a1814(unsigned char *a)
{
    unsigned char *p;
    void *q;
    char *r;
    unsigned char *t;
    int z;
    int n;

    z = 0;
    *(int *)(a + 0x10) = z;
    p = a + 0x10;
    Func_80a10d0(p, 0, 0, 0xd, 5, 2);
    q = *(void **)p;
    r = Func_80a1778(q, -8, 0xb);
    r[5] = 0xd;
    a[0x1c] = 0xff;
    a[0x1d] = z;
    *(char **)(a + 0x14) = r;
    n = 0xfe;
    r[0xf] = n;
    t = *(unsigned char **)(a + 0x18);
    n -= 0xff;
    t[0xf] = n;
    return q;
}
