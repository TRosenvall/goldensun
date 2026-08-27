extern unsigned int gBuffer[];

void Func_8010704(int sx, int sy, int w, int h, int dx, int dy)
{
    unsigned int *src;
    unsigned int *dst;
    unsigned int *s;
    unsigned int *d;
    unsigned short y;
    unsigned short x;

    src = gBuffer;
    dst = src + ((dy << 7) + dx);
    src += (sy << 7) + sx;
    for (y = 0; y < h; y++) {
        d = dst + (y << 7);
        s = src + (y << 7);
        for (x = 0; x < w; x++) {
            *d = (*d & 0xfff) | (*s & 0xfffff000);
            d++;
            s++;
        }
    }
}
