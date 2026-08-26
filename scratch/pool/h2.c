extern unsigned char gBuffer[];
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];

void OvlFunc_916_2008098(int col, int row, int w, int h, int page, int x0, int y0)
{
    unsigned int *src;
    int x, y, i;
    unsigned int t;

    src = (unsigned int *)gBuffer + (col + (row << 7));
    for (y = y0; y < y0 + h; y++) {
        for (x = x0; x < x0 + w; x++) {
            t = *src++;
            i = ((((y & 0xf) + (page << 4)) << 5) + (x & 0xf)) << 2;
            ((int *)0x6002800)[i >> 2] = *(int *)(ewram_2020000 + ((t & 0xfff) << 3));
            ((int *)0x6002840)[i >> 2] = *(int *)(ewram_2020004 + ((t & 0xfff) << 3));
        }
        src += 0x80 - w;
    }
}
