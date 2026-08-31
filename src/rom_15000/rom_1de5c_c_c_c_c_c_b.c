extern int iwram_3001e8c;

void Func_801fda8(void *a, int x, int y, int w, int h)
{
    char *base;
    int off;
    int i;
    unsigned short *p;

    base = (char *)iwram_3001e8c;
    x = x + *(unsigned short *)((char *)a + 0xc) + 1;
    y = y + *(unsigned short *)((char *)a + 0xe) + 1;
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
            i = w;
            p = (unsigned short *)(off + (int)base);
            while (i != 0) {
                i--;
                *p = 0xe006;
                p++;
            }
            h--;
            off += 0x40;
        } while (h != 0);
        *(unsigned char *)(base + 0xea3) = 1;
    }
}
