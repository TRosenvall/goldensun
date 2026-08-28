extern unsigned char *_GetUnit(int id);
extern int Func_80bf208(int id, int a, int b);

int Func_80bf250(int id)
{
    unsigned char *u;
    unsigned char *p;
    unsigned char *q;
    int n;
    int z;

    u = _GetUnit(id);
    p = u + (0x99 << 1);
    if (*p == 0)
        goto fail;
    n = *p + 0xff;
    *p = n;
    z = 0;
    if ((signed char)n == 0) {
        q = u + 0x133;
        *q = z;
        return 1;
    }
    q = u + 0x133;
    if (*(signed char *)q >= 0)
        goto fail;
    if (Func_80bf208(id, *p, 0x1e) == 0)
        goto fail;
    *q = z;
    *p = z;
    return 1;
fail:
    return 0;
}
