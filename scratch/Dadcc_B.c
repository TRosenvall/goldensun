extern int iwram_3001e40;

void OvlFunc_924_200adcc(void)
{
    unsigned int i;
    unsigned short *d;
    unsigned short *s;
    if ((iwram_3001e40 & 7) == 0) {
        d = (unsigned short *)0x50000c2;
        *(unsigned short *)0x50000ce = *d;
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
