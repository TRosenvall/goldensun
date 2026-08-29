extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetUnit(int);
extern unsigned char *_GetItemInfo(int);
extern int Func_80b19cc(int);
extern int _GetInventoryItem(int, int);
extern void Func_80b04dc(int);
extern void Func_80b0a6c(int, int, int);
extern int Func_80b1614(int, int, int);
extern void WaitFrames(int);
extern void _Func_80a17c4(int);

int Func_80b1e80(int a, int b)
{
    unsigned char *g;
    unsigned char *u;
    unsigned char *info;
    unsigned char *p;
    int idx;
    int cnt;
    int q;
    int m;
    int x, y;
    int res;

    g = iwram_3001f2c;
    u = _GetUnit(a);
    idx = b * 2 + 0xd8;
    info = _GetItemInfo(*(unsigned short *)(u + idx));
    res = 1;
    q = Func_80b19cc(*(unsigned short *)(u + idx));
    cnt = _GetInventoryItem(a, b);
    m = 0x10;
    m &= info[3];
    if (m != 0 && cnt > 1) {
        Func_80b04dc(0xcad);
        x = *(short *)(g + 0x388);
        p = g + 0x380;
        *(unsigned char *)(*(int *)p + 5) = 4;
        y = *(short *)(g + 0x38a);
        *(g + 0x3a8) = 0xc;
        Func_80b0a6c(0, 0x80, 0x30);
        res = Func_80b1614(0, cnt, q);
        WaitFrames(1);
        _Func_80a17c4(*(int *)p);
        Func_80b0a6c(0, x, y);
    }
    return res;
}
