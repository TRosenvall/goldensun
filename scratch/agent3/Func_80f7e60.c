extern unsigned char *ewram_2004c00;
extern void Func_80f7e34(int idx);
extern void Func_80f7df0(void);

void Func_80f7e60(int base, int n, unsigned char *slot)
{
    unsigned char *p;
    int i;
    int j;
    int k;
    int c;
    int off;

    for (i = 0; i < n; i++) {
        Func_80f7e34((base + 0x124 + i) & 0x3ff);
        j = base + i;
        p = ewram_2004c00;
        k = *(int *)(p + 0x4438);
        c = slot[k];
        k++;
        *(int *)(p + 0x4438) = k;
        if (k == *(int *)(p + 0x4440)) {
            off = (j & 0x3ff) << 2;
            off += 0x3404;
            *(int *)(p + off) = -1;
            break;
        }
        off = (j & 0x3ff) << 2;
        off += 0x3404;
        *(int *)(p + off) = c;
        Func_80f7df0();
    }
    for (i++; i < n; i++) {
        j = (base + i) & 0x3ff;
        Func_80f7e34(j);
        j <<= 2;
        j += 0x3404;
        *(int *)(ewram_2004c00 + j) = -1;
    }
}
