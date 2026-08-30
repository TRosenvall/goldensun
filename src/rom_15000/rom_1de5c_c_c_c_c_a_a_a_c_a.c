extern unsigned char *iwram_3001e90;
extern int _Func_80b6a60(int n);
extern int _GetPartySize(void);

void Func_801eea0(int flags)
{
    unsigned char *p;
    int n;
    int rows;
    int x;
    int w;
    int z;

    p = iwram_3001e90;
    rows = 4;
    if ((*(unsigned char **)((char *)&iwram_3001e90 - 4))[0xea5] != 0) {
        n = _Func_80b6a60(0);
        rows = 3;
    } else {
        n = _GetPartySize();
    }
    if (flags & 1)
        rows++;
    else
        flags &= ~2;
    n = n * 6;
    x = n + 1;
    if (flags & 2)
        x = n + 6;
    w = 0x1e - x;
    z = 0;
    *(short *)(p + 4) = w;
    *(short *)(p + 6) = z;
    *(short *)(p + 8) = x;
    *(short *)(p + 0xa) = rows;
    *(short *)(p + 0xc) = flags;
}
