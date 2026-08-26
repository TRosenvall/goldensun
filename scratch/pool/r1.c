struct S { unsigned short a; unsigned short pad; int b; };

extern void Func_802281c(struct S *s);
extern void _Func_80c10e8(struct S *s, int n);

void Func_80270ac(void)
{
    struct S s;
    int u;

    s.b = 0;
    s.a = 0xff;
    Func_802281c(&s);
    _Func_80c10e8(&s, 1);
}
