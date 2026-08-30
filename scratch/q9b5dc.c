extern unsigned char gState[];
extern void Func_809b450(unsigned char *e);

void Func_809b5dc(unsigned char *e)
{
    unsigned char *g;
    short *b;
    short m;
    int n;
    int v;

    n = *(short *)(e + 0x64);
    b = (short *)(e + 0x66);
    v = (*b)++;
    g = gState;
    m = *(short *)(g + (0xed << 1));
    if (m == 1) {
        if (v % 7 == 0)
            Func_809b450(e);
    } else {
        if (v % 5 == 0)
            Func_809b450(e);
    }
    if (n == 1)
        *(short *)(e + 6) += 0xc0 << 4;
}
