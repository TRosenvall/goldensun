extern void *__GetFieldActor(int id);

int OvlFunc_899_200c7bc(int x, int y, int id)
{
    int *a;
    int ax;
    int ay;
    int m;
    int r;
    int p;
    int q;

    a = (int *)__GetFieldActor(id);
    m = 0x80 << 24;
    ax = a[0xe];
    if (ax == m)
        ax = a[2];
    ay = a[0x10];
    if (ay == m)
        ay = a[4];
    ax = (ax - x) >> 16;
    ay = (ay - y) >> 16;
    q = ay * ay;
    p = ax * ax;
    r = 1;
    if (p + q > 0x40)
        r = 0;
    return r;
}
