unsigned short OvlFunc_914_2008b24(unsigned short c, int d)
{
    short r, g, b;
    r = (int)c & 0x1f;
    g = ((int)c >> 5) & 0x1f;
    b = ((int)c >> 10) & 0x1f;
    r = r + r / (d * 4);
    g = g - g / d;
    b = b - b / d;
    if (r > 0x1f) r = 0x1f;
    return r | (g << 5) | (b << 10);
}
