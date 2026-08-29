extern unsigned char gState[];
extern int L2018[] __asm__(".L2018");
extern unsigned int __Random(void);

int OvlFunc_951_2008d70(int n)
{
    unsigned char *g;
    int idx;
    int v;

    if (n < 0)
        return 0;
    if (n == 5)
        n = __Random() * 5 >> 16;
    g = gState;
    idx = n + (0x9a << 1);
    v = *(signed char *)(g + idx);
    v = v + (__Random() * 2 >> 16);
    v = v + 4;
    v = v % 3;
    g[idx] = v;
    return L2018[n * 3 + v];
}
