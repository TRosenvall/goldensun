struct Box {
    unsigned char pad00[0xc];
    unsigned short w;
    unsigned short h;
};

extern unsigned char iwram_3001e8c[];

void Func_8020a60(struct Box *b, int x, int y, int w, int h, int flip)
{
    unsigned short *base;
    unsigned short *p;
    int x1, y1, off, n;

    base = *(unsigned short **)iwram_3001e8c;
    x1 = b->w + x + 1;
    y1 = b->h + y + 1;
    flip <<= 12;
    if (x1 < 0) {
        w += x1;
        x1 = 0;
    }
    if (x1 + w > 0x1d)
        w = 0x1e - x1;
    if (y1 < 0) {
        h += y1;
        y1 = 0;
    }
    if (y1 + h > 0x1d)
        h = 0x14 - y1;
    if (w > 0 && h > 0) {
        off = (y1 << 6) + (x1 << 1);
        do {
            p = (unsigned short *)((char *)base + off);
            n = w;
            while (n != 0) {
                unsigned int v = *p;
                v &= ~0x1000;
                v |= flip;
                *p = v;
                n--;
                p++;
            }
            h--;
            off += 0x40;
        } while (h != 0);
        *((char *)base + 0xea3) = 1;
    }
}
