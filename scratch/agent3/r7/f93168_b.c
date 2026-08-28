extern int _Func_8017658(int a, int b, int c, int d);
extern int _Func_8017394(int a);
extern void WaitFrames(int n);
extern unsigned char *iwram_3001ebc;

void Func_8093168(int a0, int a1, int x, int y)
{
    unsigned char *p = iwram_3001ebc;
    int xx = x;
    int yy = y;
    int h;

    if (yy > 0x77)
        yy += 0x20;
    else
        yy -= 0x20;
    if (x < 8)
        xx = 8;
    if (xx > 0x138)
        xx = 0x138;
    if (yy < 0x14)
        yy = 0x14;
    if (yy > 0xdc)
        yy = 0xdc;
    h = _Func_8017658(*(short *)(p + 0x1d8), xx, yy, 1);
    while (_Func_8017394(h) == 0)
        WaitFrames(1);
    *(unsigned short *)(p + 0x1d8) += 1;
}
