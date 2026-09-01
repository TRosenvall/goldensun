extern int iwram_3001f2c;
extern void *Func_80a1814(void *p);
extern void Func_80a1870(void *p, int a, int b, int c, int d);

void Func_80a8034(void)
{
    char *g;
    void *q;
    int z;
    int off;
    char *p;

    g = (char *)iwram_3001f2c;
    q = Func_80a1814(g);
    z = 0;
    Func_80a1870(q, 2, 2, 8, z);
    off = 0x88 << 1;
    p = g + off;
    *(int *)(g + 0x28) = z;
    *(int *)(g + 0x24) = z;
    *(int *)(g + 0x2c) = z;
    *(int *)(g + 0x20) = z;
    *p = z;
    p += 1;
    *p = z;
    g[0x89 << 1] = 8;
    g[0x113] = 2;
}
