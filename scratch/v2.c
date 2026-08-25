extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(void *a, int n);
extern void __Func_80929d8(void *a, int n);
extern void __Func_800c548(void *a, int n);

void *OvlFunc_946_20089f4(int a, int b, int c, int d)
{
    unsigned char *p;
    unsigned char *q;
    unsigned char *r;
    int n;
    int v;
    int z;
    int w;
    unsigned char *ret;

    p = __CreateActor(d, a, b, c);
    ret = 0;
    if (p == 0)
        goto out;
    q = *(unsigned char **)(p + 0x50);
    n = 0xd;
    v = q[9];
    n = -n;
    n &= v;
    r = p;
    q[9] = n;
    r += 0x55;
    z = 0;
    *r = z;
    r += 4;
    w = 8;
    *r = w;
    __Actor_SetSpriteFlags(p, 0);
    __Func_80929d8(p, 0xe);
    __Func_800c548(p, 1);
    ret = p;
out:
    return ret;
}
