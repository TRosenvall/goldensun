extern unsigned char *iwram_3001e8c;

void Func_8019d0c(void)
{
    unsigned char *p;
    int off;
    int v;
    unsigned char *q;

    p = iwram_3001e8c;
    off = 0x12ec;
    v = 0x3e7;
    q = p + off;
    *(short *)q = v;
    off += 2;
    q = p + off;
    *(short *)q = v;
}
