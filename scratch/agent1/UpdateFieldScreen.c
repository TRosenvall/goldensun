struct Layer {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    int f24;
    unsigned short f28;
    unsigned short f2a;
    unsigned char pad2c[4];
};

struct Map {
    int *f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0xd4];
    int fe4;
    int fe8;
    int fec;
    int ff0;
    int ff4;
    int ff8;
    unsigned char padfc[8];
    struct Layer layers[3];
};

struct Pt {
    short x;
    short y;
};

extern unsigned int iwram_3001e70;
extern struct Pt iwram_3001ad0[];
extern int Func_8000888(int a, int b);
extern int Random(void);
extern void UpdateScreenEdge_H(int i, int a, int b);
extern void UpdateScreenEdge_V(int i, int a, int b);

static inline int call_via(int (*f)(int, int), int a, int b)
{
    register int _a __asm__("r0") = a;
    register int _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r2", "r12"
    );
    return _a;
}

void UpdateFieldScreen(void)
{
    struct Map *m;
    struct Layer *L;
    struct Pt *pt;
    int *pe4;
    int *pe8;
    int *q;
    int x, y, b;
    int lox, hix, loy, hiy;
    int px, py;
    int t, u, v;
    unsigned int i;

    m = *(struct Map **)&iwram_3001e70;
    L = m->layers;
    q = m->f0;
    if (q == 0)
        return;
    x = *q++ - 0x780000;
    b = *q++;
    y = *q - b - 0x600000;
    lox = m->fec + m->f4;
    hix = m->ff4 - m->f4 - 0xf00000;
    loy = m->ff0 + m->f8;
    hiy = m->ff8 - m->f8 - 0xa00000;
    if (lox > hix)
        hix = lox;
    if (loy > hiy)
        hiy = loy;
    if (x < lox)
        x = lox;
    if (x > hix)
        x = hix;
    if (y < loy)
        y = loy;
    if (y > hiy)
        y = hiy;
    if (m->f4 != 0) {
        t = Random();
        u = Random();
        v = m->f4;
        x += call_via(Func_8000888, v, t - u);
        m->f4 = call_via(Func_8000888, v, m->fc);
    }
    if (m->f8 != 0) {
        t = Random();
        u = Random();
        v = m->f8;
        y += call_via(Func_8000888, v, t - u);
        m->f8 = call_via(Func_8000888, v, m->fc);
    }
    m->fe4 = x;
    m->fe8 = y;
    {
        register int (*g)(int, int) __asm__("r9") = Func_8000888;

        for (i = 0; i <= 2; i++) {
            x = call_via(g, m->fe4, L->f10);
            y = call_via(g, m->fe8, L->f14);
            if (L->f18 != 0) {
                t = L->f20 + L->f18;
                x += t;
                L->f20 = t;
                x &= (L->f28 << 19) | 0x7ffff;
            }
            if (L->f1c != 0) {
                t = L->f24 + L->f1c;
                y += t;
                L->f24 = t;
                y &= (L->f2a << 19) | 0x7ffff;
            }
            x += L->f8;
            y += L->fc;
            px = x / 0x80000;
            py = y / 0x80000;
            if (((L->f0 ^ x) & (0x80 << 12)) != 0) {
                if (L->f0 < x)
                    UpdateScreenEdge_H(i, px + 0x1e, py);
                else
                    UpdateScreenEdge_H(i, px, py);
            }
            if (((L->f4 ^ y) & (0x80 << 13)) != 0) {
                if (L->f4 < y)
                    UpdateScreenEdge_V(i, px, py + 0x14);
                else
                    UpdateScreenEdge_V(i, px, py);
            }
            pt = &iwram_3001ad0[3 - i];
            pt->x = x >> 16;
            pt->y = y >> 16;
            L->f0 = x;
            L->f4 = y;
            L++;
        }
    }
}
