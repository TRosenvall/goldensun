extern void *_GetUnit(int id);
extern int Func_80bf208(int id, int n, int k);

int Func_80bf37c(int id)
{
    unsigned char *p;
    int v;
    p = (unsigned char *)_GetUnit(id) + (0x9c << 1);
    v = *p;
    if (!v)
        return 0;
    v += 0xff;
    *p = v;
    if (!(unsigned char)v)
        return 1;
    if (!Func_80bf208(id, *p, 0x1e))
        return 0;
    *p = 0;
    return 1;
}
