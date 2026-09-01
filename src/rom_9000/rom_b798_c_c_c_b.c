extern unsigned char *_GetSpriteInfo(int id);

int Func_800be20(int id, unsigned int idx, int count)
{
    unsigned char *info;
    unsigned char *ip;
    unsigned char *q;
    int tbl;
    int total;
    unsigned int a;
    int b;

    total = 0;
    info = _GetSpriteInfo(id);
    if (idx >= info[5])
        return 0;
    tbl = *(int *)(info + 0x10);
    ip = (unsigned char *)(idx << 2);
    q = *(unsigned char **)(ip + tbl);
loop:
    a = q[0];
    b = q[1];
    q += 2;
    if (a == 0xfe)
        goto done;
    if (a == 0xf1)
        goto done;
    if (a == 0xfd)
        goto done;
    if (a == 0xef)
        goto done;
    if (a != 0xf5 && a != 0xff && a > 0xee)
        goto loop;
    count--;
    total += b;
    if (count != 0)
        goto loop;
done:
    return total;
}
