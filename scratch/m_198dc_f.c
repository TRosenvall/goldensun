extern unsigned char *iwram_3001e8c;

void Func_80198dc(void)
{
    unsigned char *b;
    int *p;
    short *q;
    int off;
    int i;
    int z;

    off = 0x12dc;
    b = iwram_3001e8c;
    q = (short *)(b + off);
    off -= 0x20;
    z = 0;
    i = 0;
    p = (int *)(b + off);
    do {
        i++;
        *p++ = z;
        *q = z;
        q++;
    } while (i != 8);
}
