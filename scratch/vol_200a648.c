extern volatile unsigned int iwram_3001e40;

void OvlFunc_924_200a648(void)
{
    unsigned short *dst;
    unsigned short *src;
    unsigned int i;

    if ((iwram_3001e40 & 7) != 0)
        return;
    dst = (unsigned short *)0x5000050;
    *(unsigned short *)0x500005e = *dst;
    src = (unsigned short *)0x5000052;
    i = 0;
    do {
        *dst = *src;
        i++;
        src++;
        dst++;
    } while (i <= 6);
}
