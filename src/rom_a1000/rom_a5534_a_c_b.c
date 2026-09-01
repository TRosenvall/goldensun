extern unsigned int iwram_3001f2c;
extern int _GetUnit(int id);
extern int Func_80a3d6c(int id);

int Func_80a5578(int *dest, int cursor)
{
    unsigned char *state;
    unsigned char *p;
    int base;
    int off;
    int unit;
    int total;
    int k;
    int idx;
    int page;
    int row;
    int pages;

    base = 0x86 << 2;
    state = *(unsigned char **)&iwram_3001f2c;
    p = state + 2;
    off = cursor + base;
    total = Func_80a3d6c(p[off]);
    unit = _GetUnit(p[off]);
    k = p[off] + (0x98 << 2);
    idx = *(signed char *)(state + k);
    if (idx + 1 > total)
        idx = total - 1;
    page = idx / 5;
    row = idx % 5;
    pages = total / 5;
    if (total % 5 != 0)
        pages++;
    dest[0] = unit;
    dest[2] = page;
    dest[3] = pages;
    dest[4] = row;
    dest[5] = total;
    dest[6] = idx;
    return 1;
}
