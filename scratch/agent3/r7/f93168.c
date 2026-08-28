extern int _Func_8017658(int a, int b, int c, int d);
extern int _Func_8017394(int a);
extern void WaitFrames(int n);
extern unsigned char *iwram_3001ebc;

void Func_8093168(int a0, int a1, int x, int y)
{
    unsigned char *p = iwram_3001ebc;
    int h;

    if (y > 0x77)
        y += 0x20;
    else
        y -= 0x20;
    if (x < 8)
        x = 8;
    if (x > 0x138)
        x = 0x138;
    if (y < 0x14)
        y = 0x14;
    if (y > 0xdc)
        y = 0xdc;
    h = _Func_8017658(*(short *)(p + 0x1d8), x, y, 1);
    while (_Func_8017394(h) == 0)
        WaitFrames(1);
    *(unsigned short *)(p + 0x1d8) += 1;
}
