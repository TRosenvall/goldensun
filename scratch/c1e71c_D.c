extern unsigned char *iwram_3001e8c;
extern int _CONST_F;

void SetTextColor(int colour)
{
    int mask;

    mask = (int)&_CONST_F;
    *(unsigned short *)(iwram_3001e8c + 0xeae) = mask & colour;
}
