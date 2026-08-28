struct Win {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

extern unsigned char *iwram_3001e8c;

void Func_80a2268(struct Win *win, int x, int y, int w, int h, int bank)
{
    unsigned int base;
    unsigned short *p;
    unsigned char *off;
    int i;
    int v;

    base = (unsigned int)iwram_3001e8c;
    x = x + win->fc + 1;
    y = y + win->fe + 1;
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
        off = (unsigned char *)((y << 6) + (x << 1));
        do {
            p = (unsigned short *)(off + base);
            i = w;
            while (i != 0) {
                v = *p;
                v &= 0xffffefff;
                v |= bank;
                *p = v;
                i--;
                p++;
            }
            h--;
            off += 0x40;
        } while (h != 0);
        *(unsigned char *)(base + 0xea3) = 1;
    }
}
