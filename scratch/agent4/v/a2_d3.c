extern unsigned char *iwram_3001e8c;

struct Win {
    unsigned char pad00[0xc];
    unsigned short f0c;
    unsigned short f0e;
};

void Func_80a2268(struct Win *win, int x, int y, int w, int h, int bank)
{
    int off, n, mask;
    unsigned char *base;
    unsigned short *p;

    base = iwram_3001e8c;
    x = x + win->f0c + 1;
    y = y + win->f0e + 1;
    bank <<= 12;
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
        off = (y << 6) + (x << 1);
        mask = 0xffffefff;
        do {
            p = (unsigned short *)(off + base);
            n = w;
            while (n != 0) {
                *p = (*p & mask) | bank;
                n--;
                p++;
            }
            h--;
            off += 0x40;
        } while (h != 0);
        *(base + 0xea3) = 1;
    }
}
