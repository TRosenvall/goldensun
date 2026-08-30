extern unsigned char *iwram_3001e8c;

void Func_80198dc(void)
{
    unsigned char *b;
    int *p;
    short *q;
    int off;
    int i;
    int z;

    b = iwram_3001e8c;
    off = 0x12dc;
    q = (short *)(b + off);
    off -= 0x20;
    i = 0;
    z = 0;
    p = (int *)(b + off);
    do {
        i++;
        *p++ = z;
        *q = z;
        q++;
    } while (i != 8);
}
