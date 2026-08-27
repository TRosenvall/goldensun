struct Scroll {
    unsigned char pad0[8];
    int f8;
    unsigned char padc[4];
    int f10;
    unsigned char pad14[4];
    int f18;
};

extern char *iwram_3001f2c;
extern int _MSG_75;
extern void _Func_8016498(void *w);
extern void WaitFrames(int n);
extern void _Func_801e7c0(int a, void *b, int c, int d);
extern void Func_80a2268(void *w, int b, int c, int d, int e, int f);

int Func_80a5614(int a0, int a1, struct Scroll *s)
{
    char *p;
    int off;
    int v;
    int i;
    int m;
    int one;

    p = iwram_3001f2c;
    s->f18 = s->f8 * 5 + s->f10;
    _Func_8016498(*(void **)(p + 0x2c));
    WaitFrames(1);
    off = s->f18;
    off <<= 1;
    off += 0xe4 << 1;
    v = *(unsigned short *)(p + off);
    if (v != 0) {
        m = 0x1ff;
        m &= v;
        _Func_801e7c0(m + (int)&_MSG_75, *(void **)(p + 0x2c), 0, 0);
    }
    i = 0;
    one = 1;
    for (; i <= 4; i++) {
        if (i == s->f10) {
            Func_80a2268(*(void **)(p + 0x20), 1, (i << 1) + 1, 0xe, one, 0xe);
        } else {
            Func_80a2268(*(void **)(p + 0x20), 1, (i << 1) + 1, 0xe, one, 0xf);
        }
    }
    WaitFrames(1);
    return 1;
}
