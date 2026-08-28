typedef unsigned char u8;
typedef unsigned short u16;

struct Aff {
    short f0;
    short f2;
    u16 f4;
};

extern u8 iwram_3001d00;
extern u8 iwram_3001d40[];
extern int divsi3_RAM(int a, int b);
extern int __divsi3(int a, int b);
extern int sin(int a);
extern int cos(int a);

int Func_8003d28(struct Aff *p)
{
    int a;
    int b;
    int c;
    int n;
    int t;
    int s;
    int co;
    int (*g)(int, int);
    int m;
    u8 *q;
    short *w;

    a = p->f0;
    n = iwram_3001d00;
    b = p->f2;
    c = p->f4;
    if (n > 0x1f)
        return 0;
    m = n * 8;
    q = iwram_3001d40 + m;
    if ((a == b || -a == b) && c == 0) {
        g = divsi3_RAM;
        t = g(0x10000, b);
        s = t;
        if (-a == b)
            s = -t;
        *(int *)q = (u16)s;
        *(int *)(q + 4) = t << 16;
    } else {
        s = sin(c);
        co = cos(c);
        w = (short *)q;
        *w = __divsi3(co, a);
        *(w + 1) = __divsi3(s, a);
        s = -s;
        *(w + 2) = __divsi3(s, b);
        *(w + 3) = __divsi3(co, b);
    }
    iwram_3001d00 = n + 1;
    return n;
}
