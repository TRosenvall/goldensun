struct W {
    unsigned char pad0[0x17e];
    short f17e;
    unsigned char pad1[0x19e - 0x180];
    short f19e;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern unsigned char ewram_2020000[];
extern unsigned char gBuffer[];

extern unsigned char *GetFieldActor(int e);

void Func_8099678(void)
{
    unsigned char *g;
    struct W *base;
    unsigned char *a;
    unsigned char *p;
    unsigned char *tbl;
    int mode;
    int x;
    int y;
    int off;
    int k;

    g = gState;
    g += 0x1f4;
    base = (struct W *)iwram_3001ebc;
    a = GetFieldActor(*(int *)g);
    mode = base->f19e;
    tbl = *(unsigned char **)((unsigned char *)&iwram_3001ebc - 0x4c);
    if (mode == 3) {
        x = *(int *)(a + 8) / 0x200000;
        y = *(int *)(a + 0x10) / 0x200000;
        p = ewram_2020000 + ((x & 0x1f) + ((y & 0x1f) << 5)) * 4;
    } else {
        k = *(a + 0x22);
        if (k <= 2) {
            off = k * 48 + 0x130;
            p = *(unsigned char **)(tbl + off);
        } else {
            p = gBuffer;
        }
        x = *(int *)(a + 8) / 0x100000;
        y = *(int *)(a + 0x10) / 0x100000;
        p += (x + (y << 7)) * 4;
    }
    if (p[2] != 0xfb)
        base->f17e = 0x2092;
}
