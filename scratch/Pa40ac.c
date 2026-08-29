extern char *_GetUnit(int id);
extern int _Func_80788c4(int a, int b);

int Func_80a40ac(int who)
{
    char *u;
    unsigned short *p;
    int off;
    unsigned short v;
    int i, r, q, n;

    u = _GetUnit(who);
    off = 0xd8;
    v = *(unsigned short *)(u + off);
    r = 0;
    i = 0;
    p = (unsigned short *)(u + 0xd8);
    goto test;
body:
    v = *p;
    if ((v & 0x200) != 0)
        goto next;
    q = v >> 11;
    n = q + 1;
    if (q == 0)
        n = 1;
    if (n == 0)
        goto done;
    do {
        r = _Func_80788c4(who, i);
        n--;
    } while (n != 0);
done:
    if (r != 2)
        return 0;
    goto one;
next:
    i++;
    p++;
    if (i > 0xe)
        goto ret;
    v = *p;
test:
    if (v != 0)
        goto body;
one:
    r = 1;
ret:
    return r;
}
