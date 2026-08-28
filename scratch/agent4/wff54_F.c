extern int gBuffer[];
extern unsigned short ewram_2020000[];
extern unsigned short ewram_2020004[];

void UpdateScreenEdge_H(int blk, int x, int y)
{
    unsigned short *dst;
    unsigned short *t0;
    unsigned short *t1;
    int mrow, mx, drow, xx, b;
    unsigned int i;
    int v, k, o;

    dst = (unsigned short *)(0x6002800 + (blk << 11));
    mrow = ((y / 2) & 0x7f) << 7;
    mx = (x / 2) & 0x7f;
    drow = (y & 0x1e) << 5;
    xx = x & 0x1e;
    b = x & 1;
    for (i = 0; i <= 10; i++) {
        v = gBuffer[mrow + mx];
        k = (v & 0xfff) * 4 + b;
        o = drow + xx + b;
        t0 = ewram_2020000;
        dst[o] = t0[k];
        t1 = ewram_2020004;
        dst[o + 0x20] = t1[k];
        mrow = (mrow + 0x80) & 0x3f80;
        drow = (drow + 0x40) & 0x3c0;
    }
}
