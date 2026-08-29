unsigned short OvlFunc_914_2008b24(unsigned short c, int d)
{
    int b, g, r;
    r = (short)(c & 0x1f);
    g = (c >> 5) & 0x1f;
    b = (c >> 10) & 0x1f;
    r = (short)(r + r / (d * 4));
    g = (short)(g - g / d);
    b = (short)(b - b / d);
    if (r > 0x1f) r = 0x1f;
    return r | ((b << 10) | (g << 5));
}
