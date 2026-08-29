extern unsigned char *iwram_3001e74;
extern void _Func_80198dc(void);
extern void Func_80b6ae0(unsigned short *buf);
extern void _Func_8019908(int id, int a);
extern void _Func_80175a0(int msg);
extern void WaitTextPrompt(void);
extern void _Func_80174d8(void);
extern int _MSG_810;
extern int _MSG_811;

void Func_80b595c(int n)
{
    unsigned short buf[8];
    unsigned char *g;
    int i;
    unsigned short *p;
    int t;

    g = iwram_3001e74;
    _Func_80198dc();
    p = buf;
    Func_80b6ae0(buf);
    for (i = 0; i != n; i++) {
        _Func_8019908(*p++, 1);
        if (i == n - 1)
            _Func_80175a0((int)&_MSG_811);
        else
            _Func_80175a0((int)&_MSG_810);
        WaitTextPrompt();
    }
    _Func_80174d8();
    t = g[0x45];
    if (t == 1) {
        _Func_80198dc();
        _Func_8019908(0, 1);
        _Func_80175a0(0x812);
        WaitTextPrompt();
    } else if (t == 2) {
        _Func_80198dc();
        _Func_8019908(0, 1);
        _Func_80175a0(0x813);
        WaitTextPrompt();
    }
}
