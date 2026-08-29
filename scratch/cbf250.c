extern unsigned char *_GetUnit(int id);
extern int Func_80bf208(int id, int n, int k);

int Func_80bf250(int id)
{
    unsigned char *u;
    unsigned char *p;
    signed char *q;
    int v;
    int z;

    u = _GetUnit(id);
    p = u + (0x99 << 1);
    v = *p;
    if (v == 0)
        return 0;
    v = v + 0xff;
    *p = v;
    z = 0;
    if ((unsigned char)v == 0) {
        *(u + 0x133) = z;
        return 1;
    }
    q = (signed char *)(u + 0x133);
    if (*q < 0)
        return 0;
    if (Func_80bf208(id, *p, 0x1e) != 0) {
        *q = z;
        *p = z;
        return 1;
    }
    return 0;
}
