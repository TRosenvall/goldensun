typedef unsigned char u8;

#define ewram_202c800 ((u8 *)0x202c800)
#define ewram_202c000 ((u8 *)0x202c000)

int Func_8012204(int *p)
{
    int zc, xc, cell, sub, v;
    unsigned int m;
    u8 *q;

    xc = p[0] >> 17;
    zc = p[2] >> 17;
    cell = (((zc / 8) & 0x3f) << 6) + ((xc / 8) & 0x3f);
    sub = (((zc / 2) & 3) << 1) + ((xc / 4) & 1);

    q = ewram_202c800 + ((u8 *)0x6005000)[cell] * 8 + sub;
    m = *q;
    if (m != 0) {
        if (xc & 2)
            v = m >> 4;
        else
            v = m & 0xf;
        if (v != 0)
            return v;
    }
    q = ewram_202c000 + ((u8 *)0x6004000)[cell] * 8 + sub;
    m = *q;
    if (m != 0) {
        if (xc & 2)
            v = m >> 4;
        else
            v = m & 0xf;
        if (v != 0)
            return v;
    }
    return 7;
}
