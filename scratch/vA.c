unsigned short OvlFunc_914_2008b24(unsigned short c, int d)
{
    short r, g, b;
    int m;
    m = 0x1f;
    r = c & m;
    g = (c >> 5) & m;
    b = (c >> 10) & m;
    r = r + r / (d * 4);
    g = g - g / d;
    b = b - b / d;
    if (r > 0x1f) r = 0x1f;
    return r | (g << 5) | (b << 10);
}
