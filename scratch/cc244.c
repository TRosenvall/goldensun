extern char *iwram_3001ebc;
extern void Func_801c2d0(void);
extern void Func_801c2e4(void);
extern int Func_8028920(int n);
extern int _Func_808ce74(void);
extern int _Func_80a5b94(void);
extern int _Func_80aa56c(void);
extern int _Func_80a24d0(void);
extern int _Func_80a7478(void);

void Func_801c244(void)
{
    char *p;
    int r;
    int v;

    p = iwram_3001ebc;
    r = 0;
    for (;;) {
        Func_801c2d0();
        r = Func_8028920(r);
        Func_801c2e4();
        switch (r) {
        case 0:
            v = _Func_808ce74();
            if (v == 0)
                v = 0xff;
            *(unsigned short *)(p + (0xbd << 1)) = v;
            return;
        case 1:
            if (_Func_80a5b94() != -1)
                return;
            break;
        case 2:
            if (_Func_80aa56c() == 0)
                return;
            break;
        case 3:
            if (_Func_80a24d0() != -1)
                return;
            break;
        case 4:
            if (_Func_80a7478() != -1)
                return;
            break;
        default:
            return;
        }
    }
}
