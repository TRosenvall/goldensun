struct P {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

extern unsigned char *iwram_3001e8c;
extern void BufferString(int id, int n);
extern void Func_801de5c(void *dst, void *a, void *b, int c);

void Func_801e7c0(int id, struct P *pl, unsigned int a2, unsigned int d)
{
    unsigned char *g;
    unsigned short *c;
    int z;
    unsigned int idx;
    unsigned char *v;
    int o;

    g = iwram_3001e8c;
    c = (unsigned short *)(g + 0x12b2);
    z = 0;
    *c = z;
    BufferString(id, 1);
    o = *c << 1;
    o += 0xeb0;
    *(unsigned short *)(g + o) = z;
    *c = (*c + 1) & 0x1ff;
    idx = (((pl->fe + (d >> 3)) + 1) << 5) + (pl->fc + (a2 >> 3)) + 1;
    if (idx < (0xa0 << 2)) {
        idx <<= 1;
        v = (unsigned char *)idx + 0x6002000;
        Func_801de5c(g + 0xeb0, g + idx, v, a2 & 7);
    }
}
