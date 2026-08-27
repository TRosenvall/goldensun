extern void *_GetUnit(int id);
extern int Func_80bf208(int id, int n, int k);

int Func_80bf37c(int id)
{
    unsigned char *p;
    int t;
    int v;

    p = (unsigned char *)_GetUnit(id) + (0x9c << 1);
    t = *p;
    v = t;
    if (v != 0) {
        v = v + 0xff;
        *p = v;
        if ((unsigned char)v == 0)
            return 1;
        if (Func_80bf208(id, *p, 0x1e) != 0) {
            *p = 0;
            return 1;
        }
    }
    return 0;
}
