extern void Func_802281c(void *s);
extern void _Func_80c10e8(void *s, int n);

void Func_80270ac(void)
{
    unsigned short h[4];
    int u;

    *(int *)&h[2] = u;
    h[0] = 0xff;
    Func_802281c(h);
    _Func_80c10e8(h, 1);
}
