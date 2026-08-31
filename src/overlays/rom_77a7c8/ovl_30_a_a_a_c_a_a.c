extern char *iwram_3001ebc;
extern unsigned char gState[];
extern void __Func_8091f14(int a, int b);

void OvlFunc_881_200808c(void)
{
    char *p;
    int *q;
    int *r;
    unsigned char *g;

    p = iwram_3001ebc;
    g = gState;
    q = (int *)(g + (0x8e << 2));
    r = (int *)(p + (0xd6 << 1));
    if (*q >= *r * 9 / 10) {
        __Func_8091f14(0x809, 0x2a);
        *(int *)(p + (0xd4 << 1)) = 0;
    }
}
