extern unsigned char Laf224[] __asm__(".Laf224");
extern unsigned char Laf228[] __asm__(".Laf228");
extern void _Func_801ea08(int val, int base, int y, int w, int e);
extern void _UIDrawText(unsigned char *s, int y, int x, int e);

void Func_80a4db4(int val, int a, int y, int w, int e)
{
    int d;
    int pos;

    _Func_801ea08(val, 3, y, w, e);
    d = 1;
    if ((val < 0 ? -val : val) > 9)
        d = 2;
    if ((val < 0 ? -val : val) > 0x63)
        d = 3;
    pos = w - (d << 3) + 0x10;
    if (val > 0)
        _UIDrawText(Laf224, y, pos, e);
    else
        _UIDrawText(Laf228, y, pos, e);
}
