extern unsigned char *_GetUnit(int id);

int Func_80bf54c(int id)
{
    unsigned char *p;
    int off;
    int v;

    off = 0x13f;
    p = _GetUnit(id) + off;
    if (*p != 0) {
        v = *p + 0xff;
        *p = v;
        if ((unsigned char)v == 0)
            return 1;
    }
    return 0;
}
