extern unsigned int gState;
extern unsigned char *iwram_3001ebc;
extern void __Func_8091f14(int a, int b);

void OvlFunc_881_200808c(void)
{
    unsigned int base;
    int *p;
    unsigned int off;
    unsigned int g;
    int v;
    int lim;

    base = (unsigned int)iwram_3001ebc;
    g = (unsigned int)&gState;
    off = 0x8e;
    off <<= 2;
    g += off;
    off -= 0x8c;
    p = (int *)(base + off);
    v = *p;
    lim = (v * 9) / 10;
    if (*(int *)g >= lim) {
        __Func_8091f14(0x809, 0x2a);
        off = 0xd4;
        off <<= 1;
        p = (int *)(base + off);
        *p = 0;
    }
}
