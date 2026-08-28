extern signed char L37403[] __asm__(".L37403");
extern signed char L373f7[] __asm__(".L373f7");

extern int _GetNumDjinn(int n);
extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern void Func_8028808(int a, int b, int c);
extern int Func_8028574(int n);
extern void Func_802851c(void);

int Func_8028920(int idx)
{
    int m;
    int k;
    int v;

    m = 0;
    if (_GetNumDjinn(-1) == 0)
        m = 1;
    k = m * 6;
    v = L37403[idx + m * 6] - 1;
    k = m * 6;
    if (v < 0)
        v = 0;
    Func_80284dc();
    AddMenuBarOption(1);
    if (m == 0)
        AddMenuBarOption(0xf);
    AddMenuBarOption(2);
    AddMenuBarOption(7);
    Func_8028808(0x11, 7, 0);
    v = Func_8028574(v);
    Func_802851c();
    if (v >= 0)
        v = L373f7[v + k + 1];
    return v;
}
