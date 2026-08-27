extern int _MSG_b33;
extern void _SetTextColor(int color);
extern void _Func_801e7c0(int msg, int win, int x, int y);

void Func_80a45cc(signed char *p, int win)
{
    int base;

    _SetTextColor(0xf);
    if (p[0] == -1)
        _SetTextColor(0xe);
    base = (int)&_MSG_b33;
    _Func_801e7c0(base, win, 0, 0x18);
    _SetTextColor(0xf);
    if (p[1] == -1)
        _SetTextColor(0xe);
    _Func_801e7c0(base + 1, win, 0x20, 0x18);
    _SetTextColor(0xf);
    if (p[3] == -1)
        _SetTextColor(0xe);
    _Func_801e7c0(base + 2, win, 0, 0x20);
    _SetTextColor(0xf);
    if (p[5] == -1)
        _SetTextColor(0xe);
    _Func_801e7c0(base + 3, win, 0x50, 0x20);
    _SetTextColor(0xf);
    if (p[2] == -1)
        _SetTextColor(0xe);
    _Func_801e7c0(base + 4, win, 0x50, 0x18);
    _SetTextColor(0xf);
    if (p[4] == -1)
        _SetTextColor(0xe);
    _Func_801e7c0(base + 5, win, 0x20, 0x20);
    _SetTextColor(0xf);
}
