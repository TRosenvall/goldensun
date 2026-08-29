extern unsigned int iwram_3001e40;
extern int L291c __asm__(".L291c");
extern int L2924 __asm__(".L2924");
extern int gOvl_0200a920;

void OvlFunc_923_2008d98(void)
{
    volatile unsigned short v;
    unsigned int i;
    int t, x;

    v = 0;
    if (iwram_3001e40 % 5 != 0)
        return;
    L291c = (L291c + 4) & 0x1f;
    for (i = 0; i <= 5; i++) {
        v = *(unsigned short *)(0x5000000 + (0x6e - i) * 2) & 0x1f;
        t = v;
        if (i <= 2)
            t -= t * 4 / 10;
        x = (L2924 << 10) | (gOvl_0200a920 << 5);
        *(unsigned short *)(0x5000000 + (0x6f - i) * 2) = t | x;
    }
    *(unsigned short *)0x50000d2 = L291c | ((L2924 << 10) | (gOvl_0200a920 << 5));
}
