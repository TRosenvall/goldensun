/* Declarations inlined from the batch-250 scratch header h520.h when this
 * candidate was landed in batch 300 -- the tree keeps its .c files
 * self-contained rather than carrying per-function headers. */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x18 - 0x10];
    unsigned short f18;
    unsigned char pad1a[0x1c - 0x1a];
    void *f1c;
    unsigned char pad20[0x22 - 0x20];
    unsigned short f22;
    unsigned char pad24[0x28 - 0x24];
};

extern unsigned int iwram_3001e40;
extern unsigned int __Random(void);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct P *p);


void OvlFunc_968_200c520(int x, int y)
{
    struct P t;
    struct P *q;
    register struct P *tp __asm__("r8");
    int v;
    int r;
    unsigned int a;
    int d;
    int n;

    q = &t;
    q->f8 = 0xb333;
    q->fc = 0xb333;
    tp = q;
    n = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
    q = tp;
    q->f22 = n;
    v = iwram_3001e40 & 3;
    if (v == 0) {
        r = __Random() * 2 >> 16;
        if (r != 0) {
            a = __Random();
            d = (int)(((__Random() * 5 >> 16) << 16) + (0xe0 << 11)) / 10;
            OvlFunc_968_2008118((x + ((a * 2 >> 16) << 4)) << 16, 0,
                                y << 19, 0, v, d, 0x88 << 16, tp);
        } else {
            OvlFunc_968_2008118((x + (__Random() * 17 >> 16)) << 16, 0,
                                (x << 19) + 0xfffc0000, 0, r, r,
                                0x88 << 16, tp);
        }
    }
}
