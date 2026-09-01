extern unsigned char *iwram_3001e8c;

extern int BufferString(int id, int mode);
extern void Func_801868c(int n, int a, int b, int c, int e, int f, int g);

int TextBox(int id, int a, int b, int c, int e)
{
    unsigned char *p;
    int n;
    int off;

    p = iwram_3001e8c;
    n = BufferString(id, 0);
    off = (n << 1) + 0xeb0;
    if (*(unsigned short *)(p + off) == 0)
        return 0;
    Func_801868c(n, a, b, c, e, 0, 0);
    return 1;
}
