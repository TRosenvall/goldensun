extern unsigned char *iwram_3001e8c;

void SetTextColor(int colour)
{
    *(unsigned short *)(iwram_3001e8c + 0xeae) = colour & 0xf;
}
