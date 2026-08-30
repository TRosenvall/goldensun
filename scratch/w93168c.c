extern unsigned char *iwram_3001ebc;
extern int _Func_8017658(int id, int a, int b, int c);
extern int _Func_8017394(int h);
extern void WaitFrames(int n);

void Func_8093168(int p0, int p1, int a, int b)
{
    unsigned char *p;
    short *c;
    int v;
    int h;
    int x;
    int y;

    x = a;
    p = iwram_3001ebc;
    y = b;
    v = x;
    if (y > 0x77)
        y += 0x20;
    else
        y -= 0x20;
    if (!(x >= 8))
        v = 8;
    if (v > (0x9c << 1))
        v = 0x9c << 1;
    if (y < 0x14)
        y = 0x14;
    if (y > 0xdc)
        y = 0xdc;
    h = _Func_8017658(*(short *)(p + (0xec << 1)), v, y, 1);
    while (_Func_8017394(h) == 0)
        WaitFrames(1);
    c = (short *)(p + (0xec << 1));
    (*c)++;
}
