extern unsigned char *iwram_3001e8c;
extern void Func_8017004(void *, int);
extern void Func_8016230(void *);
extern void ClearUIRegion(int, int, int, int);

typedef struct {
    int f0;
    int f4;
    short f8;
    short fa;
    short fc;
    short fe;
    short f10;
    short f12;
    short f14;
    unsigned short f16;
    short f18;
    short f1a;
    short f1c;
    short f1e;
    short f20;
    short f22;
} W;

void Func_8016f2c(void)
{
    unsigned char *g;
    unsigned char *fp;
    unsigned char *fq;
    W *w;
    int i;

    g = iwram_3001e8c;
    w = (W *)(g + 0x500);
    i = 0;
    do {
        if (w->f16 != 0) {
            if (w->f18 != 0) {
                Func_8017004(w, 0);
                w->f18 = w->f18 - 1;
            } else if (w->f1a != 0) {
                Func_8016230(w);
            }
        } else if (w->f1a != 0) {
            if (w->f18 != w->f1a) {
                ClearUIRegion(w->f1c, w->f1e, w->f20, w->f22);
                Func_8017004(w, 1);
                w->f18 = w->f18 + 1;
                fp = g + 0xea3;
                *fp = 1;
            } else {
                ClearUIRegion(w->f1c, w->f1e, w->f20, w->f22);
                fq = g + 0xea3;
                w->f0 = 0;
                w->f4 = 0;
                w->f8 = 0;
                w->fa = 0;
                w->fc = 0;
                w->fe = 0;
                w->f10 = 0;
                w->f12 = 0;
                w->f14 = 0;
                w->f16 = 0;
                w->f18 = 0;
                w->f1a = 0;
                w->f1c = 0;
                w->f1e = 0;
                w->f20 = 0;
                w->f22 = 0;
                *fq = 1;
            }
        }
        i++;
        w++;
    } while (i != 8);
}
