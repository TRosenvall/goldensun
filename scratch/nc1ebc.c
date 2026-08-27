extern unsigned char *iwram_3001e74;
extern unsigned char *_GetUnit(void);

void Func_80c1ebc(void)
{
    unsigned char *b;
    unsigned char *u;
    unsigned char *p;
    int n;
    int i;
    int k;
    int t;
    int bit;
    int o;

    b = iwram_3001e74;
    n = b[0x40];
    u = _GetUnit();
    if (u[0x129] != 0)
        return;
    p = u;
    t = p[0x94 << 1];
    for (i = 0; i < n; i++)
        if (*(unsigned short *)(b + 0x10 + i * 2) == t)
            break;
    if (i == n)
        return;
    o = i * 4 + 0x1c;
    if (*(int *)(b + o) == 0)
        return;
    k = 0;
    while (u[k] != 0) {
        k++;
        if (k > 0xd)
            break;
    }
    bit = 0x20;
    if (k > 0)
        bit = u[k - 1] - 0x31;
    *(int *)(b + o) &= ~(1 << bit);
}
