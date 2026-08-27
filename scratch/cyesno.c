extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern void Func_8028808(int a, int b, int c);
extern int Func_8028574(int a);
extern void Func_802851c(void);

int YesNoMenu(int a, int b, int c, int d)
{
    int x;
    int y;
    int res;

    x = c;
    y = 0;
    Func_80284dc();
    if (x == 0)
        x = 3;
    if (a != 0)
        y = 0x11;
    AddMenuBarOption(5);
    AddMenuBarOption(6);
    Func_8028808(y, x, b);
    res = Func_8028574(d);
    Func_802851c();
    if (res == -1)
        res = 1;
    return res;
}
