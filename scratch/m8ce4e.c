extern unsigned int iwram_3001e40;

void OvlFunc_960_2008ce4(void)
{
    unsigned short v;
    unsigned short n;

    v = iwram_3001e40 & 0x3f;
    if (v > 0x1f)
        v = 0x40 - v;
    n = (v >> 1) + 7;
    n = (unsigned short)(n | ((n << 10) | (n << 5)));
    *(volatile unsigned short *)0x500019e = n;
}
