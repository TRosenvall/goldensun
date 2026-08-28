extern int iwram_3001e40;

void OvlFunc_924_200adcc(void)
{
    unsigned short *d;
    unsigned short *s;
    unsigned short *save;
    unsigned int i;

    if ((iwram_3001e40 & 7) == 0) {
        d = (unsigned short *)0x50000c2;
        save = (unsigned short *)0x50000ce;
        *save = *d;
        s = (unsigned short *)0x50000c4;
        i = 0;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 5);
    }
}
