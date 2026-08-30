extern int OvlFunc_883_200d610(int *p, int *q);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_883_200d64c(unsigned char *a, unsigned char *b, int c, int d)
{
    unsigned char *f5b;
    unsigned char *f62;
    int *pb;
    int *pa;
    int r;
    int t;
    int m;
    int h;

    r = 0;
    f5b = a + 0x5b;
    f62 = a + 0x62;
    if (*f5b == 1 && *f62 == 0) {
        __Actor_SetAnim(a, 1);
        return 1;
    }
    pb = (int *)(b + 8);
    pa = (int *)(a + 8);
    if (OvlFunc_883_200d610(pb, pa) >= c && d == 0)
        goto lose;
    t = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10), *pb - *pa);
    m = 0xf0 << 8;
    h = *(unsigned short *)(a + 6) & m;
    if ((t & m) != h && ((t + (0x80 << 5)) & m) != h
        && ((t + 0xfffff000) & m) != h && d == 0)
        goto lose;
    *f5b = 1;
    __Actor_SetAnim(a, 1);
    r = 1;
    *f62 = 1;
    return r;
lose:
    *f5b = d;
    __Actor_SetAnim(a, 2);
    *f62 = d;
    return r;
}
