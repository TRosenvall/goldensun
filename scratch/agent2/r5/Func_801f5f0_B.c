struct S {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

extern unsigned char *iwram_3001e8c;

void Func_801f5f0(struct S *p, int x, int y, int w, int h, int f)
{
    unsigned char *base;
    unsigned short *q;
    int off, i, bit;

    base = iwram_3001e8c;
    x = x + p->fc + 1;
    y = y + p->fe + 1;
    bit = f & 1;
    bit <<= 12;
    if (x < 0) {
        w += x;
        x = 0;
    }
    if (x + w > 0x1d)
        w = 0x1e - x;
    if (y < 0) {
        h += y;
        y = 0;
    }
    if (y + h > 0x1d)
        h = 0x14 - y;
    if (w > 0 && h > 0) {
        y <<= 6;
        off = y + x * 2;
        do {
            q = (unsigned short *)(base + off);
            i = w;
            while (i != 0) {
                int v = *q;
                v &= 0xffffefff;
                v |= bit;
                *q = v;
                q++;
                i--;
            }
            h--;
            off += 0x40;
        } while (h != 0);
        base[0xea3] = 1;
    }
}
