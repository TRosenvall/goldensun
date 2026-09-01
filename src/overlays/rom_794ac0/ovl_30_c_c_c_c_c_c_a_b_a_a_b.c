extern const unsigned char _TBL_4f2c[] __asm__(".L4f2c");

unsigned char *OvlFunc_899_200c704(int *p)
{
    unsigned char *t;
    unsigned char *r;
    int x;
    int y;
    unsigned int i;

    x = (p[0] - 0x400000) >> 19;
    y = (p[2] - 0x2700000) >> 19;
    t = (unsigned char *)_TBL_4f2c;
    r = 0;
    for (i = 0; i <= 0x24; i++, t += 0x10) {
        if (t[0] == x || t[0] + 1 == x) {
            if (t[1] == y || t[1] + 1 == y) {
                r = t;
                break;
            }
        }
    }
    return r;
}
