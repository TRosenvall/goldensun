extern unsigned char *iwram_3001e70;
extern int L5b58[] __asm__(".L5b58");
extern int L5b38[] __asm__(".L5b38");
extern int L5b50[] __asm__(".L5b50");
extern int L5b60[] __asm__(".L5b60");
extern int __cos(int a);
extern int __sin(int a);
extern int __Random(void);

void OvlFunc_943_200b1a8(void)
{
    unsigned char *base;
    int *p;
    int *q;
    int c;
    int s;
    int r;
    int t;

    base = iwram_3001e70;
    p = *(int **)base;
    c = __cos(L5b58[0]);
    s = __sin(L5b38[0]);
    c >>= 1;
    *p = *p + c;
    p++;
    *p = *p + s;
    L5b58[0] += (unsigned int)(__Random() * 384) >> 16;
    r = __Random();
    L5b58[0] = (unsigned short)L5b58[0];
    t = L5b38[0];
    t += (unsigned int)(r << 9) >> 16;
    t &= 0xffff;
    L5b38[0] = t;
    q = (int *)(0x82 * 2 + base);
    q[2] = L5b50[0];
    L5b50[0] -= L5b60[0];
    if (L5b50[0] < 0)
        L5b50[0] += 0x80 << 14;
    if (L5b50[0] > (0x80 << 14))
        L5b50[0] -= 0x80 << 14;
    q[3] = L5b50[1];
    L5b50[1] -= L5b60[1];
    if (L5b50[1] < 0)
        L5b50[1] += 0x80 << 14;
}
