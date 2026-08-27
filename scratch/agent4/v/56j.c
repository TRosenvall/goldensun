extern unsigned char *iwram_3001f2c;
extern int _MSG_75;

struct D {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
    unsigned char pad14[4];
    int f18;
};

extern void _Func_8016498(int a);
extern void WaitFrames(int n);
extern void _Func_801e7c0(int a, int b, int c, int d);
extern void Func_80a2268(int win, int x, int y, int w, int h, int bank);

int Func_80a5614(int a0, int a1, struct D *d)
{
    unsigned char *s;
    int v;
    int off;
    int mask;
    int i, y, one;

    s = iwram_3001f2c;
    d->f18 = d->f8 * 5 + d->f10;
    _Func_8016498(*(int *)(s + 0x2c));
    WaitFrames(1);
    off = d->f18;
    off <<= 1;
    off += 0xe4 << 1;
    if (*(unsigned short *)(s + off) != 0) {
        mask = 0x1ff;
        v = *(unsigned short *)(s + off);
        _Func_801e7c0((mask & v) + (int)(&_MSG_75), *(int *)(s + 0x2c), 0, 0);
    }
    one = 1;
    i = 0;
    y = 1;
    for (; i <= 4; i++) {
        if (i == d->f10)
            Func_80a2268(*(int *)(s + 0x20), 1, y, 0xe, one, 0xe);
        else
            Func_80a2268(*(int *)(s + 0x20), 1, y, 0xe, one, 0xf);
        y += 2;
    }
    WaitFrames(1);
    return 1;
}
