# 0 "scratch/CL_one.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "scratch/CL_one.c"
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

void Func_80b0a20(struct A *a, int x, int y)
{
    struct B *b;
    int m;
    int t;
    int z;

    b = a->p;
    m = 0xffff;
    a->fd = 1;
    b->f6 = x;
    a->f8 = x;
    a->f4 = x;
    x &= m;
    x &= 0x1ff;
    z = 0;
    a->fc = z;
    t = b->f16;
    t = 0xfffffe00 & t;
    t |= x;
    b->f16 = t;
    a->fa = y;
    a->f6 = y;
    a->p->f8 = y;
    y &= m;
    a->p->f14 = y;
}
