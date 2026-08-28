void Func_80aac84(int delta)
{
    int i;
    int j;
    int n;
    int idx;
    int r, g, b;
    unsigned int c;
    int off;
    int mask;

    n = 0xf;
    i = 0;
    mask = 0x1f;
outer:
    idx = n << 4;
    j = 0;
inner:
    off = (idx + j) * 2;
    c = *(unsigned short *)(0x5000000 + off);
    r = (c >> 10) & mask;
    g = (c >> 5) & mask;
    b = c & mask;
    r += delta;
    g += delta;
    b += delta;
    if (r > 0x1f)
        r = 0x1f;
    if (g > 0x1f)
        g = 0x1f;
    if (b > 0x1f)
        b = 0x1f;
    if (r < 0)
        r = 0;
    if (g < 0)
        g = 0;
    if (b < 0)
        b = 0;
    g <<= 5;
    {
        unsigned short *d = (unsigned short *)(0x4ffffe0 + off);
        *d = (r << 10) | g | b;
    }
    j++;
    if (j <= 0xf)
        goto inner;
    n = 5;
    if (i != 0) {
        delta = -12;
        n = 7;
    }
    i++;
    if (i <= 2)
        goto outer;
}
