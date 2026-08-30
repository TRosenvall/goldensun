extern int OvlFunc_901_2008314(int *p, int *q, int n);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_901_2008350(unsigned char *a, unsigned char *b, int c, int d)
{
    int *pb;
    int *pa;
    int r;
    int t;
    int m;
    int h;

    pb = (int *)(b + 8);
    pa = (int *)(a + 8);
    r = 0;
    if (OvlFunc_901_2008314(pb, pa, 0) >= c && d == 0) {
        a[0x5b] = r;
        __Actor_SetAnim(a, 2);
    } else {
        t = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10), *pb - *pa);
        m = 0xf0 << 8;
        h = *(unsigned short *)(a + 6) & m;
        if ((t & m) == h || ((t + (0x80 << 5)) & m) == h
            || ((t + 0xfffff000) & m) == h || d != 0) {
            a[0x5b] = 1;
            __Actor_SetAnim(a, 1);
            r = 1;
        }
    }
    return r;
}
