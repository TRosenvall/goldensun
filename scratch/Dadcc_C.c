extern int iwram_3001e40;

void OvlFunc_924_200adcc(void)
{
    unsigned short *d;
    unsigned short *s;
    unsigned int i;
    if ((iwram_3001e40 & 7) == 0) {
        d = (unsigned short *)0x50000c2;
        *(unsigned short *)0x50000ce = *d;
        i = 0;
        s = (unsigned short *)0x50000c4;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 5);
    }
}
