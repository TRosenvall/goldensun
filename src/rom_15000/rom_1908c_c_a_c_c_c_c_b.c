extern int iwram_3001e8c;
extern volatile int gKeyHeld;
extern int _Func_80f954c(void);

int Func_801999c(unsigned char *p)
{
    char *g;
    int f;
    int z;

    f = 0;
    g = (char *)iwram_3001e8c;
    if (g[0x12f9] != 0) {
        if (_Func_80f954c() == 0)
            f = 1;
    }
    if ((gKeyHeld & 0x303) != 0)
        f = 1;
    if (f != 0) {
        z = 0;
        *(unsigned short *)(p + 0x14) = z;
        return 1;
    }
    return 0;
}
