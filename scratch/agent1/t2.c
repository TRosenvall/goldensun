struct Scroll {
    unsigned char pad00[8];
    int f08;
    unsigned char pad0c[8];
    int f14;
};

extern unsigned char *iwram_3001f2c;
extern int _MSG_182;
extern void _Func_8016498(int w);
extern void _Func_801e41c(int w, int a, int b, int c, int d);
extern void Func_80a2324(int a, int b, int w, int x, int y);
extern void Func_80a21b0(int w, int a, int b, int c, int d);
extern void _Func_801e7c0(int str, int a, int b, int c);

int Func_80a56c8(int window, int unused, struct Scroll *d)
{
    unsigned char *st;
    unsigned short *p;
    unsigned int base;
    unsigned char count, i;
    int m;

    st = iwram_3001f2c;
    _Func_8016498(window);
    _Func_801e41c(window, 0, 0xb, 0x10, 0xb);
    base = d->f08 * 5;
    count = d->f14 - base;
    if (count > 5)
        count = 5;
    Func_80a2324(5, base, window, 0x74, 0x22);
    Func_80a21b0(window, d->f14, 5, d->f08, 0xf);
    i = 0;
    if (count > i) {
        p = (unsigned short *)(st + (base << 1) + 0x1c8);
        m = 0x1ff;
        do {
            id = m & *p;
            _Func_801e7c0(id + (int)&_MSG_182, *(int *)(st + 0x20), 0x18, (i << 4) + 8);
            i++;
            p++;
        } while (count > i);
    }
    return 1;
}
