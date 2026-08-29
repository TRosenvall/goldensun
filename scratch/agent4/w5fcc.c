extern unsigned char ewram_2002240[];

int Func_8005fcc(void)
{
    unsigned char *p;
    volatile unsigned char *sc;
    unsigned int sio;
    int v;
    int hi;
    int t;
    int one;
    int r;
    int res;

    p = ewram_2002240;
    sc = (volatile unsigned char *)0x4000128;
    sio = *(volatile unsigned int *)sc;
    if (p[1] == 0) {
        v = sio;
        v &= 0x88;
        if (v == 8) {
            if ((unsigned char)(sio & 4) == 0 && *(int *)(p + 0x14) == -1) {
                *(volatile unsigned short *)0x4000208 = 0;
                t = *(volatile unsigned short *)0x4000200;
                t = (t & ~0x80) | 0x40;
                *(volatile unsigned short *)0x4000200 = t;
                one = 1;
                *(volatile unsigned short *)0x4000208 = one;
                t = sc[1];
                t &= ~0x40;
                sc[1] = t;
                *(volatile unsigned short *)0x4000202 = 0xc0;
                *(volatile unsigned int *)0x400010c = 0xc963;
                p[0] = v;
            }
            p[1] = 1;
        }
        p[0xb]++;
    }
    hi = p[2];
    hi <<= 8;
    r = p[3] | hi;
    if (p[0] == 8) {
        r |= 0x80;
    }
    res = r;
    if (p[9] != 0) {
        res |= 0x80 << 5;
    }
    if (((sio << 26) >> 30) > 1) {
        res |= 0x80 << 6;
    }
    return res;
}
