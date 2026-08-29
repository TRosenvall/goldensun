extern int _K_1f;

unsigned short OvlFunc_914_2008b24(unsigned short c, int d)
{
    short b, g, r;
    int m;

    m = (int)&_K_1f;
    r = c & 0x1f;
    g = (c >> 5) & m;
    b = (c >> 10) & m;
    r = r + r / (d * 4);
    g = g - g / d;
    b = b - b / d;
    if (r > 0x1f) r = 0x1f;
    return r | ((b << 10) | (g << 5));
}
