typedef unsigned short u16;

void Func_80aac84(int add)
{
    int i, j, row, base;
    int idx, b, g, r, v;
    int mask;
    unsigned int c;

    row = 15;
    i = 0;
    mask = 31;
outer:
    j = 0;
    base = row << 4;
inner:
    idx = base + j;
    c = ((u16 *)0x05000000)[idx];
    b = (c >> 10) & mask;
    g = (c >> 5) & mask;
    r = c & mask;
    b += add;
    g += add;
    r += add;
    if (b > 31) b = 31;
    if (g > 31) g = 31;
    if (r > 31) r = 31;
    if (b < 0) b = 0;
    if (g < 0) g = 0;
    if (r < 0) r = 0;
    v = (b << 10) | (g << 5) | r;
    ((u16 *)0x05000000)[idx - 16] = v;
    j++;
    if (j <= 15)
        goto inner;
    row = 5;
    if (i != 0) {
        row = 7;
        add = -12;
    }
    i++;
    if (i <= 2)
        goto outer;
}
