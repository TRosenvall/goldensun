extern void *_GetUnit(int id);
extern int Func_80bf208(int id, int n, int k);

int Func_80bf37c(int id)
{
    unsigned char *p;
    unsigned char v;

    p = (unsigned char *)_GetUnit(id) + (0x9c << 1);
    v = *p;
    if (v == 0)
        return 0;
    v--;
    *p = v;
    if (v == 0)
        return 1;
    if (Func_80bf208(id, *p, 0x1e) == 0)
        return 0;
    *p = 0;
    return 1;
}
