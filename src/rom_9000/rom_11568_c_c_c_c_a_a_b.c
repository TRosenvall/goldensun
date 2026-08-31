extern char *galloc_ewram(int tag, int size);

void Func_8011b00(void)
{
    char *base;
    char *p;
    unsigned short i;
    unsigned short j;
    int off;
    int zero;

    base = galloc_ewram(0x1c, 0xb4);
    zero = 0;
    p = base;
    for (j = 0; j != 4; j++) {
        *(int *)p = 0;
        *(unsigned short *)(p + 4) = 0;
        *(unsigned short *)(p + 6) = 0;
        *(unsigned short *)(p + 8) = 0;
        *(unsigned short *)(p + 0xa) = 0;
        for (i = 0; i != 16; i++) {
            off = i * 2 + 0xc;
            *(unsigned short *)(p + off) = 0;
        }
        p += 0x2c;
    }
    *(unsigned short *)(base + 0xb0) = zero;
}
