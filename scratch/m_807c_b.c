extern int OvlFunc_949_2008040(int *p, int *q, int n);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern unsigned char *__MapActor_GetActor(int slot);

int OvlFunc_949_200807c(unsigned char *a, unsigned char *b, int c, int d)
{
    int *pb;
    int *pa;
    int r;
    int t;
    int m;
    int h;
    int far;
    int base;
    int p2;
    int m1;
    int p1;

    pa = (int *)(a + 8);
    pb = (int *)(b + 8);
    r = 0;
    if (OvlFunc_949_2008040(pb, pa, 0) >= c && d == 0) {
        a[0x5b] = r;
        __Actor_SetAnim(a, 2);
    } else {
        t = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10), *pb - *pa);
        m = 0xf0 << 8;
        far = (t + 0xffffe000) & m;
        p2 = t + (0x80 << 6);
        m1 = t + 0xfffff000;
        p1 = t + (0x80 << 5);
        h = *(unsigned short *)(a + 6);
        base = t & m;
        h = h & m;
        p2 = p2 & m;
        m1 = m1 & m;
        p1 = p1 & m;
        if (base == h || p1 == h || m1 == h || d != 0) {
            a[0x5b] = 1;
            __Actor_SetAnim(a, 1);
            r = 1;
        }
        if (b == __MapActor_GetActor(0) && (p2 == h || far == h)) {
            a[0x5b] = 1;
            __Actor_SetAnim(a, 1);
            r = 1;
        }
    }
    return r;
}
