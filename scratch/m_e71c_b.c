extern unsigned char *iwram_3001e8c;

void SetTextColor(unsigned short c)
{
    unsigned char *p;
    int off;

    p = iwram_3001e8c;
    off = 0xeae;
    *(short *)(p + off) = c & 0xf;
}
