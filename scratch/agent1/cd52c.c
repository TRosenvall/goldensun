extern unsigned char *iwram_3001eec;
extern void Func_80d6888(int t, int color, int sanim, int idx, int dur, int slot);

void Func_80cd52c(void)
{
    unsigned char *base = iwram_3001eec;
    unsigned char *p;
    int i, off;

    for (i = 0, off = 0x24, p = base + 0x7818; i != 8; i++, off += 2, p++) {
        if (*p != 0) {
            *p = *p - 1;
            if (*p == 0)
                Func_80d6888(*(short *)(*(char **)(base + 0x7828) + off), 0, -1, -1, 0, i);
        }
    }
}
