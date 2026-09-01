extern unsigned char *iwram_3001f2c;
extern void Func_80a9d84(void);
extern void Func_80a17c4(void *x);

void Func_80a9d3c(unsigned char *flags)
{
    unsigned char *p;
    void **q;
    unsigned char *x;
    int i;
    int y;
    int k;

    p = iwram_3001f2c;
    Func_80a9d84();
    i = 0;
    q = (void **)(p + 0xc8);
    y = 0x58;
again:
    {
        x = *q++;
        if (x != 0 && flags[i] != 0) {
            k = 8;
            *(unsigned short *)(x + 6) = k;
            *(unsigned short *)(x + 8) = y;
            x[0xf] = 0xf0;
            Func_80a17c4(x);
            y += 0x10;
        }
        i++;
    }
    if (i <= 4)
        goto again;
}
