extern unsigned char gState[];
extern int _CONST_1;
extern void _DeleteActor(unsigned char *a);

void Func_809b364(unsigned char *a)
{
    unsigned char *g;
    int base;
    int lim;
    unsigned char *h;
    int v;
    int t;
    int cap;
    int d;

    g = gState;
    base = *(int *)(a + 0x14);
    lim = base + (0xa0 << 12);
    h = *(unsigned char **)(a + 0x68);
    if (*(short *)(g + (0xed << 1)) == (int)&_CONST_1)
        lim = base + (0x80 << 11);
    v = *(int *)(a + 0xc);
    if (v <= lim) {
        _DeleteActor(a);
        return;
    }
    t = *(int *)(a + 0x18) + (0xc0 << 4);
    cap = 0x80 << 9;
    if (t > cap)
        t = cap;
    *(int *)(a + 0x18) = t;
    *(int *)(a + 0x1c) = t;
    *(int *)(a + 8) = *(int *)(h + 8);
    *(int *)(a + 0xc) = v - 0x20000;
    d = cap - t;
    *(int *)(a + 0x10) = *(int *)(h + 0x10) + d * 5 + (0x90 << 12);
}
