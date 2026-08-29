extern unsigned char *iwram_3001e8c;
extern int BufferString(int id, int mode);
extern int Func_80165d8(int a, int b, int c, int d, int e, int f);

int Func_80175c0(int dest, int id)
{
    unsigned char *p;
    int n;
    int r;
    int z;

    p = iwram_3001e8c;
    z = 0;
    *(short *)(p + 0x12f4) = z;
    *(short *)(p + 0x12f6) = z;
    n = BufferString(id, 1);
    if (*(unsigned short *)(p + (0xeb << 4) + n * 2) == 0)
        return 0;
    if (dest == 0)
        return 0;
    r = Func_80165d8(dest, n, 0, 0, z, 1);
    if (r == 0)
        return 0;
    return r;
}
