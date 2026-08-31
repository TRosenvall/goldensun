extern int _Func_8079ef8(int a);

int GetMoveDisplayEffect(unsigned char *m)
{
    int t;
    int k;
    int r;

    r = 0;
    t = m[1] & 0xf;
    if (t == 1)
        r = 1;
    if (t == 0xb)
        r = 2;
    k = m[3];
    if (k == 3)
        r = 3;
    if (k == 4)
        r = 4;
    if (k == 0x40)
        r = 6;
    if (_Func_8079ef8(*((volatile unsigned char *)m + 3)) != 0)
        r = 5;
    return r;
}
