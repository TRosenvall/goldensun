struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x10];
    int f24;
    int f28;
    int f2c;
    unsigned char pad30[8];
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x28];
    void *f6c;
};

extern unsigned char *iwram_3001f30;
extern void _Actor_SetAnim(struct Actor *a, int n);
extern void _PlaySound(int id);
extern void Func_8096f14(void);
extern void Func_8096f50(void);
extern void Func_8096cdc(struct Actor *a, int b, int c);
extern void _Func_8019908(int a, int b);
extern void _Func_801776c(int a, int b);
extern int _GetFlag(int id);
extern void Func_8097174(void);

void Func_809728c(void)
{
    unsigned char *s;
    unsigned char *g;
    struct Actor *a;
    int n;
    signed char *f;

    s = iwram_3001f30;
    g = ((unsigned char **)&iwram_3001f30)[-29];
    a = *(struct Actor **)(s + 0x10);
    n = *(short *)(s + 0x1c);
    _Actor_SetAnim(a, 0x14);
    a->f38 = a->f8;
    a->f3c = a->fc;
    a->f40 = a->f10;
    a->f24 = 0;
    a->f28 = 0;
    a->f2c = 0;
    f = (signed char *)(0x22 + s);
    if (*f != 0) {
        _PlaySound(0xd4);
        a->f6c = (void *)Func_8096f14;
    }
    if (*(signed char *)(s + 0x23) != 0) {
        Func_8096cdc(a, 1, 0);
        _Func_8019908(n, 4);
        if (*(signed char *)(s + 0x21) != 0)
            _Func_801776c(0x926, *(signed char *)(s + 0x71c));
        else
            _Func_801776c(0x926, *(signed char *)(s + 0x71c));
        Func_8096cdc(a, 0, 0x10);
    }
    if (_GetFlag(0xa0 << 1) != 0) {
        if (*f != 0)
            a->f6c = (void *)Func_8096f50;
        _Actor_SetAnim(a, 0x15);
    } else {
        Func_8097174();
    }
    *(char *)(0xcc7 + g) = 1;
}
