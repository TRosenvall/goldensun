extern unsigned char *_GetUnit(int id);

int Func_80bf54c(int id)
{
    unsigned char *p;
    int off;
    unsigned char w;
    int v;

    off = 0x13f;
    p = _GetUnit(id) + off;
    w = *p;
    if (w != 0) {
        v = w + 0xff;
        *p = v;
        if ((unsigned char)v == 0)
            return 1;
    }
    return 0;
}
