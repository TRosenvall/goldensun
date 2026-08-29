extern unsigned int iwram_3001e40;
extern int gScript_969__0200e004;
extern int L6008 __asm__(".L6008");
extern int L600c __asm__(".L600c");
extern int _umodsi3_RAM(unsigned int a, int b);
extern int _divsi3_RAM(int a, int b);

void OvlFunc_924_2008d58(void)
{
    volatile unsigned short t;
    unsigned int i;
    int c;
    int mix;
    unsigned short *p;

    t = 0;
    if (_umodsi3_RAM(iwram_3001e40, 5) == 0) {
        gScript_969__0200e004 = (gScript_969__0200e004 + 4) & 0x1f;
        i = 0;
        do {
            t = *(unsigned short *)(0x5000000 + ((0x6e - i) << 1)) & 0x1f;
            c = t;
            if (i <= 2)
                c -= _divsi3_RAM(c << 2, 10);
            p = (unsigned short *)(0x5000000 + ((0x6f - i) << 1));
            mix = (L600c << 10) | (L6008 << 5);
            c |= mix;
            *p = c;
            i++;
        } while (i <= 5);
        *(unsigned short *)0x50000d2 = gScript_969__0200e004 | mix;
    }
}
