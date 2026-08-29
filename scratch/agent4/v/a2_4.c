extern unsigned char *iwram_3001e8c;

struct Win {
    unsigned char pad00[0xc];
    unsigned short f0c;
    unsigned short f0e;
};

void Func_80a2268(struct Win *win, int x, int y, int w, int h, int bank)
{
    unsigned short *p;
    int off;
    int n;

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
        do {
            p = (unsigned short *)(off + iwram_3001e8c);
            n = w;
            while (n != 0) {
                *p = (*p & 0xffffefff) | bank;
                n--;
                p++;
            }
            h--;
            off += 0x40;
        } while (h != 0);
        *(iwram_3001e8c + 0xea3) = 1;
    }
}
