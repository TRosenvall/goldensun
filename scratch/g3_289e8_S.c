extern signed char L3740f[] __asm__(".L3740f");
extern short ewram_200200c;
extern short ewram_2002010;

extern int Func_801f77c(int a);
extern void Func_80284dc(void);
extern void AddMenuBarOption(int option);
extern void Func_8028808(int a, int b, int c);
extern int Func_8028574(int a);
extern void Func_802851c(void);

int Func_80289e8(int a)
{
    unsigned int mode;
    int r;
    int sel;
    int v;

    mode = 0;
    r = 0;
    v = Func_801f77c(a);
    if (v < 0)
        return -1;
    if (v == 0)
        return 0;
    if (v == 3) {
        mode = 1;
    } else if (v == 0x67) {
        mode = 2;
    } else if (v > 0x64) {
        mode = 3;
    } else {
        r = 1;
    }
    Func_80284dc();
    if (mode == 0 || mode == 3)
        AddMenuBarOption(0x15);
    if (mode <= 1)
        AddMenuBarOption(0x16);
    if (mode == 0 || mode == 3)
        AddMenuBarOption(0x17);
    AddMenuBarOption(0x18);
    if (ewram_200200c != 0)
        AddMenuBarOption(0x1d);
    if (ewram_2002010 != 0)
        AddMenuBarOption(0x1e);
    Func_8028808(0x11, 7, 0);
    sel = Func_8028574(r);
    Func_802851c();
    if (sel >= 0)
        sel = L3740f[sel + mode * 6];
    return sel;
}
