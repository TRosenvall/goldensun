extern unsigned char *iwram_3001f2c;
extern int _Func_8078480(int id);
extern int _Func_8078ad0(int id, int b);

int Func_80b0070(void)
{
    unsigned char *base;
    short *out;
    int i;
    int n;
    int c;

    base = iwram_3001f2c;
    n = 0;
    i = 0;
    out = (short *)(base + (0x9b << 2));
    do {
        c = *(signed char *)(base + 0x3a9);
        if (c == _Func_8078480(i) && _Func_8078ad0(i, 0) != 0) {
            *out = i;
            n++;
            out++;
        }
        i++;
    } while (i <= 0x1ff);
    *(short *)(base + (0x9b << 2) + n * 2) = 0;
    base[0x3a6] = n;
    return n;
}
