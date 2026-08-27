int HeightTile_6(signed char *p, int x, int y)
{
    signed char *q;
    int a, b, c, e;

    a = p[0] << 19;
    q = p + 1;
    b = q[0] << 19;
    c = q[1] << 19;
    e = y - x;
    e += 15;
    if (e == 15)
        return b;
    if ((unsigned int)e <= 14)
        return a + ((b - a) * e) / 16;
    return b + ((c - b) * (e - 15)) / 16;
}
