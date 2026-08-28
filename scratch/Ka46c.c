extern int __GetFlag(int id);
extern void __Func_8010788(int a, int b, int c, int d, int e, int f);

void OvlFunc_965_200a46c(void)
{
    int m;
    int n;

    if (!__GetFlag(0x985)) {
        m = 0x11;
        n = 0x4e;
        __Func_8010788(0x24, 0x4e, 1, 2, m, n);
    } else {
        m = 0x11;
        n = 0x4e;
        __Func_8010788(0x22, 0x4e, 1, 2, m, n);
    }
}
