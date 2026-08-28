extern char *iwram_3001e8c;

void Func_80167ac(unsigned char *a)
{
    char *p;
    char *d;
    int o;

    p = iwram_3001e8c;
    o = 0xeae;
    d = p + o;
    *(unsigned short *)d = *(unsigned short *)(a + 0x16);
    o -= 2;
    d = p + o;
    *(unsigned short *)d = *(unsigned short *)(a + 0x18);
    d = p + 0xea8;
    *(unsigned short *)d = *(unsigned short *)(a + 0x1a);
}
