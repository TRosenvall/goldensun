extern int Func_80b6a60(unsigned short *buf);
extern unsigned char *_GetUnit(int id);
extern unsigned char *_GetSummonInfo(int k);
extern int *_Func_8077330(int n);

void Func_80b5d3c(void)
{
    unsigned char e[4];
    unsigned short buf[10];
    unsigned char *s;
    unsigned short *p;
    int count;
    int i;
    int j;
    int m;
    int off;
    int mask;

    count = Func_80b6a60(buf);
    mask = 0;
    for (i = 0; i <= 3; i++) {
        e[i] = 0;
        if (count > 0) {
            off = 0x118;
            p = buf;
            off += i;
            j = count;
            do {
                e[i] += _GetUnit(*p++)[off];
            } while (--j != 0);
        }
    }
    for (i = 0; i <= 0x1f; i++) {
        s = _GetSummonInfo(i);
        if (s != 0) {
            s += 4;
            for (m = 0; m <= 3; m++)
                if (e[m] < s[m])
                    break;
            if (m == 4)
                mask |= 1 << i;
        }
    }
    *_Func_8077330(0) = mask;
}
