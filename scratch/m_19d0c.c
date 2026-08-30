extern unsigned char *iwram_3001e8c;

void Func_8019d0c(void)
{
    unsigned char *p;
    int off;
    int v;

    p = iwram_3001e8c;
    off = 0x12ec;
    v = 0x3e7;
    *(short *)(p + off) = v;
    off += 2;
    *(short *)(p + off) = v;
}
