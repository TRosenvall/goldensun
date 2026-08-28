extern char *iwram_3001e8c;

void Func_80167ac(unsigned char *a)
{
    char *p;
    int o;

    p = iwram_3001e8c;
    o = 0xeae;
    *(unsigned short *)(p + o) = *(unsigned short *)(a + 0x16);
    o -= 2;
    *(unsigned short *)(p + o) = *(unsigned short *)(a + 0x18);
    *(unsigned short *)(p + 0xea8) = *(unsigned short *)(a + 0x1a);
}
