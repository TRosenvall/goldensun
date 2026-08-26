extern unsigned char *iwram_3001f2c;
extern int Func_80b27b0(int a, int b);
extern void *Func_80b2884(int id);
extern void _Func_8016478(int a);
extern void _Func_8019908(void *h, int n);
extern void _DrawSmallText(void *s, int a, int b, int c);

void Func_80b2ed8(int a, int b)
{
    int lang;
    void *h;
    int id;
    void *s;

    lang = *(signed char *)(iwram_3001f2c + 0x3aa);
    h = Func_80b2778(b, lang);
    if (a != 0) {
        _Func_8016478(a);
        if (Func_80b27b0(b, lang))
            id = 0xd2c;
        else
            id = 0xd2d;
        s = Func_80b2884(id);
        _Func_8019908(h, 5);
        _DrawSmallText(s, a, 0, 0);
    }
}
