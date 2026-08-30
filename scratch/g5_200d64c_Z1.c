struct Ent {
    unsigned char pad0[6];
    unsigned short f6;
    int f8;
    unsigned char pad0c[0x10 - 0xc];
    int f10;
    unsigned char pad14[0x5b - 0x14];
    unsigned char f5b;
    unsigned char pad5c[0x62 - 0x5c];
    unsigned char f62;
};
extern int OvlFunc_883_200d610(int *p, int *q);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(struct Ent *a, int n);

int OvlFunc_883_200d64c(struct Ent *a, struct Ent *b, int c, int d)
{
    int r;
    int t;
    int m;
    int h;
    int v0;
    int v1;
    int v2;

    r = 0;
    if (a->f5b == 1 && a->f62 == 0) {
        __Actor_SetAnim(a, 1);
        return 1;
    }
    if (OvlFunc_883_200d610(&b->f8, &a->f8) >= c && d == 0)
        goto lose;
    t = (unsigned short)__atan2(b->f10 - a->f10,
                                *&b->f8 - *&a->f8);
    m = 0xf0 << 8;
    v2 = (t + 0xfffff000) & m;
    v1 = (t + (0x80 << 5)) & m;
    v0 = t & m;
    h = m & a->f6;
    if (v0 != h && v1 != h && v2 != h && d == 0)
        goto lose;
    a->f5b = 1;
    __Actor_SetAnim(a, 1);
    r = 1;
    a->f62 = 1;
    goto done;
lose:
    a->f5b = d;
    __Actor_SetAnim(a, 2);
    a->f62 = d;
done:
    return r;
}
