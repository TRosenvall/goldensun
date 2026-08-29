extern unsigned char *iwram_3001e8c;

void Func_801e260(int x, int y, unsigned int w, unsigned int h)
{
    unsigned char *base;
    unsigned short *p;
    char *q;
    unsigned int i, j, v, o;
    int style;

    base = iwram_3001e8c;
    q = (char *)(((y << 5) + x) << 1);
    p = (unsigned short *)(q + (unsigned int)base);
    style = base[0xea2];
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            v = *p++ & 0x3ff;
            if ((v >= 0x80 && v <= 0xff) || (style != 0 && v > 0x1ff && v <= 0x27f)) {
                o = ((v & 0xff) ^ 0x80) + 0xda0;
                base[o] = base[o] & 0xfc;
            }
        }
        p = (unsigned short *)((char *)p + ((0x20 - w) << 1));
    }
}
