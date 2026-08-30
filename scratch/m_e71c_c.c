extern unsigned char *iwram_3001e8c;

void SetTextColor(int c)
{
    unsigned char *p;
    int off;

    p = iwram_3001e8c;
    off = 0xeae;
    *(unsigned short *)(p + off) = c & 0xf;
}
