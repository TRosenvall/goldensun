typedef struct {
    int a;
    int b;
    int c;
    int d;
    int e;
    int f;
} V;

extern volatile int iwram_3001e40;
extern int __Random(void);
extern void __PlaySound(int id);
extern void OvlFunc_922_2009c18(int a, int b, int c, int d, int e, int f, int g,
                                void *h);

void OvlFunc_922_2009d78(void)
{
    V v;
    int n;
    int p1;
    int p2;
    int p3;
    int q;
    int x;
    int y;

    p1 = 0x9a << 17;
    p2 = 0x80 << 15;
    p3 = 0xde << 16;
    q = 0xd0001;
    n = iwram_3001e40 & 3;
    if (n == 0) {
        v.b = 0xa;
        v.c = 0x80 << 8;
        v.d = 0x80 << 8;
        v.e = 0x1cccc;
        v.f = 0x1cccc;
        if ((iwram_3001e40 & 7) == 0)
            __PlaySound(0x88);
        x = 0xffff0000 - ((((unsigned int)(__Random() * 2)) >> 16) << 16);
        y = -((((unsigned int)(__Random() * 3)) >> 16) * 0x3333);
        OvlFunc_922_2009c18(p1, p2, p3, x, y, n, q, &v);
    }
}
