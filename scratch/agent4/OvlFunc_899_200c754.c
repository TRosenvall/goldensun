extern unsigned char gTbl[] __asm__(".L4f2c");
unsigned char *OvlFunc_899_200c754(unsigned char *p, short *q)
{
    unsigned int i; int best; int sel; int d; short v; short w; unsigned short u; unsigned short base;
    sel = -1;
    base = *(unsigned short *)q;
    best = 0x8000;
    v = *q;
    p += 4;
    for (i = 0; i <= 2; i++) {
        w = p[1] << 8;
        u = w;
        d = (short)(u - base);
        if (d < 0) d = -d;
        if (p[0] != 0xff && d < best) { best = d; sel = p[0]; v = w; }
        p += 4;
    }
    if (sel == -1) return 0;
    *q = v;
    return gTbl + sel * 16;
}
