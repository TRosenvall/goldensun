extern unsigned char *iwram_3001e8c;
extern int _CONST_F;

void SetTextColor(int colour)
{
    unsigned char *base;
    int mask;

    base = iwram_3001e8c;
    mask = (int)&_CONST_F;
    colour = colour & mask;
    *(unsigned short *)(base + 0xeae) = colour;
}
