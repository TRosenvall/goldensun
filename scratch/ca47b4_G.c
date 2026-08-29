extern char *iwram_3001f2c;
extern void Func_80a10d0(void *p, int a, int b, int c, int d, int e);
extern void Func_80a22f4(void);
extern void Func_80a4924(void *p, int v);

int Func_80a47b4(int idx)
{
    char *base;
    char *p;
    int off;

    base = iwram_3001f2c;
    p = base + 0x30;
    Func_80a10d0(p, 0, 0, 0xd, 0xa, 2);
    Func_80a22f4();
    off = idx * 2 + 0x178;
    if (*(unsigned short *)(base + off) != 0)
        Func_80a4924(*(void **)p, *(unsigned short *)(base + off));
    return 1;
}
