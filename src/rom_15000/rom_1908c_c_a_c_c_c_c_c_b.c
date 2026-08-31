extern int iwram_3001e8c;
extern int iwram_3001af8;
extern volatile int gKeyPress;
extern int _Func_80f954c(void);

int Func_80199ec(unsigned char *p)
{
    char *g;
    int f;
    int k;
    int z;

    f = 0;
    g = (char *)iwram_3001e8c;
    if (g[0x12f9] != 0) {
        if (_Func_80f954c() == 0)
            f = 1;
    }
    k = gKeyPress;
    if (g[0xea4] != 0)
        k = iwram_3001af8;
    if ((k & 0x303) != 0)
        f = 1;
    if (f != 0) {
        z = 0;
        *(unsigned short *)(p + 0x14) = z;
        return 1;
    }
    return 0;
}
