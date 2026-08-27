extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetUnit(int id);

void Func_80ae7fc(unsigned char *out)
{
    unsigned char *base;
    unsigned short *p;
    unsigned char *m;
    int i, j, one, bit;
    int e, n, set;

    base = iwram_3001f2c;
    for (i = 0; i < base[0x219]; i++) {
        one = 1;
        p = (unsigned short *)(base + (0x82 << 2));
        e = 0;
        n = 0;
        m = _GetUnit(p[i]) + 0xf8;
        for (; e <= 3; e++) {
            set = *(int *)(m + 0x10);
            for (j = 0; j <= 0x13; j++) {
                bit = one << j;
                if ((set & bit) != 0 || (*(int *)m & bit) != 0)
                    n++;
            }
            m += 4;
        }
        out[i] = n;
    }
}
