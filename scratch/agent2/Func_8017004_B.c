typedef unsigned char u8;
typedef unsigned short u16;

struct Win {
    u8 pad0[8];
    u16 f8;
    u16 fa;
    u16 fc;
    u16 fe;
    u8 pad10[8];
    short f18;
    short f1a;
    u16 f1c;
    u16 f1e;
    u16 f20;
    u16 f22;
};

extern int Func_80008ac(int a, int b);
extern void Func_80170f8(int a, int b, int c, int d);

void Func_8017004(struct Win *p, int amount)
{
    int v[3];
    int (*g)(int, int);
    int s0;
    int s1;
    int d;
    int A;
    int B;
    int C;
    int D;
    int r;

    s0 = p->f18;
    s1 = p->f1a;
    d = s1 - s0;
    v[0] = (s0 * p->f8) << 16;
    g = Func_80008ac;
    v[1] = s1 << 17;
    r = g(v[1], v[0]);
    v[2] = r;
    r >>= 16;
    A = r + p->fc;
    v[0] = (d * p->f8) << 16;
    r = g(v[1], v[0]);
    v[2] = r;
    B = r >> 15;
    v[0] = (s0 * p->fa) << 16;
    v[1] = p->f1a << 17;
    r = g(v[1], v[0]);
    v[2] = r;
    r >>= 16;
    C = r + p->fe;
    v[0] = (d * p->fa) << 16;
    r = g(v[1], v[0]);
    v[2] = r;
    D = r >> 15;
    Func_80170f8(A, C, B, D);
    if (amount != 0) {
        p->f1c = A;
        p->f1e = C;
        p->f20 = B;
        p->f22 = D;
    }
}
