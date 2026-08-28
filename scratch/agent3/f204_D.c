typedef unsigned char u8;

extern u8 ewram_202c800[][8];
extern u8 ewram_202c000[][8];

int Func_8012204(int *p)
{
    int zc, xc, cell, sub, v;
    unsigned int m;

    zc = p[2] >> 17;
    xc = p[0] >> 17;
    cell = (((zc / 8) & 0x3f) << 6) + ((xc / 8) & 0x3f);
    sub = (((zc / 2) & 3) << 1) + ((xc / 4) & 1);

    m = ewram_202c800[((u8 *)0x6005000)[cell]][sub];
    if (m != 0) {
        if (xc & 2)
            v = m >> 4;
        else
            v = m & 0xf;
        if (v != 0)
            return v;
    }
    m = ewram_202c000[((u8 *)0x6004000)[cell]][sub];
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
