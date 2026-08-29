extern unsigned char *_GetUnit(void);
extern int Func_80bf208(int a, int b, int c);

int Func_80bf37c(int a)
{
    unsigned char *p;
    int n;

    p = _GetUnit() + (0x9c << 1);
    n = *p;
    if (n == 0)
        return 0;
    n += 0xff;
    *p = n;
    if ((unsigned char)n == 0)
        return 1;
    if (Func_80bf208(a, *p, 0x1e) == 0)
        return 0;
    *p = 0;
    return 1;
}
