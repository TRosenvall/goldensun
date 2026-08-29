extern unsigned char *iwram_3001e8c;
extern int _CONST_F;

void SetTextColor(int colour)
{
    int mask;
    unsigned short *p;

    mask = (int)&_CONST_F;
    colour = colour & mask;
    p = (unsigned short *)(iwram_3001e8c + 0xeae);
    *p = colour;
}
