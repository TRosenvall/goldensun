extern unsigned char *iwram_3001e8c;
extern int _CONST_F;

void SetTextColor(int colour)
{
    *(unsigned short *)(iwram_3001e8c + 0xeae) = colour & (int)&_CONST_F;
}
