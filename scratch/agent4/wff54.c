extern int gBuffer[];
extern unsigned short ewram_2020000[];
extern unsigned short ewram_2020004[];

void UpdateScreenEdge_H(int blk, int x, int y)
{
    unsigned short *dst;
    int my, mx, yy, xx, b;
    unsigned int i;
    int v, k, o;

    dst = (unsigned short *)(0x6002800 + (blk << 11));
    my = (y / 2) & 0x7f;
    mx = (x / 2) & 0x7f;
    yy = y & 0x1e;
    xx = x & 0x1e;
    b = x & 1;
    for (i = 0; i <= 10; i++) {
        v = gBuffer[my * 128 + mx];
        k = (v & 0xfff) * 4 + b;
        o = yy * 32 + xx + b;
        dst[o] = ewram_2020000[k];
        dst[o + 0x20] = ewram_2020004[k];
        my = (my + 1) & 0x7f;
        yy = (yy + 2) & 0x1e;
    }
}
