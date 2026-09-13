struct B {
    unsigned char pad00[6];
    unsigned short f6;
    unsigned short f8;
    unsigned char pad0a[0xa];
    unsigned char f14;
    unsigned char pad15;
    unsigned short f16;
};

struct A {
    struct B *p;
    unsigned short f4;
    unsigned short f6;
    unsigned short f8;
    unsigned short fa;
    unsigned char fc;
    unsigned char fd;
};

void Func_80b09fc(struct A *a, int n, int y, int f)
{
    struct B *b;

    b = a->p;
    a->f4 = b->f6;
    a->f6 = b->f8;
    a->f8 = n;
    a->fa = y;
    a->fd = f;
    a->fc = 0;
}
