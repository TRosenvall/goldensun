extern char gOvl_0200e3f4[];

void OvlFunc_881_200a7dc(void)
{
    char *base;
    char *w;
    char *p;
    int off;
    int k, one, neg1;

    base = gOvl_0200e3f4;
    k = 0x21;
    neg1 = 1;
    w = base;
    one = 1;
    p = base + 4;
    off = 0;
    neg1 = -neg1;
    for (;;) {
        if (*(int *)(base + off) == 2 && *(short *)p == 0x8a) {
            *(int *)(w + off) = one;
            *(int *)(p + 4) = k;
            return;
        }
        if (*(int *)(base + off) == neg1)
            return;
        p += 0xc;
        off += 0xc;
    }
}
