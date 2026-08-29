typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _GetFlag(int id);
extern void *Func_808d394(int i);
extern void *GetFieldActor(int i);
extern void _Actor_SetPos(void *a, int x, int y, int z);
extern void _Actor_SetAnim(void *a, int n);

void Func_8095680(void)
{
    unsigned int base;
    unsigned int off;
    unsigned char *p;
    int hi;
    unsigned int lo;
    int i;
    int *slot;
    unsigned char *a;
    unsigned char *b;
    unsigned char *c;

    base = (unsigned int)&gState;
    off = 0x8d;
    off <<= 2;
    p = (unsigned char *)(base + off);
    hi = *(short *)p & 0xf000;
    lo = *(unsigned short *)p & 0xfff;
    if (_GetFlag(0x109) == 0)
        return;
    if (hi != 0)
        return;
    hi = lo & 0x800;
    lo &= 0x7ff;
    if (lo - 0x12c > 0x50)
        return;
    off = 0x236;
    p = (unsigned char *)(base + off);
    off -= 0x42;
    if (*(short *)p <= 0)
        return;
    i = 8;
    slot = (int *)(base + off);
    for (; i <= 0x41; i++) {
        a = (unsigned char *)Func_808d394(i);
        if (a == 0)
            continue;
        if (*(short *)(a + 2) - 0x30 != (int)(lo - 0x12c))
            continue;
        b = (unsigned char *)GetFieldActor(i);
        if (b == 0)
            continue;
        if (hi == 0) {
            *(int *)(b + 0x14) = hi;
            b[0x55] = 3;
            _Actor_SetPos(b, *(int *)(a + 8), *(int *)(a + 0xc),
                          *(int *)(a + 0x10));
        } else {
            c = (unsigned char *)GetFieldActor(*slot);
            _Actor_SetPos(b, *(int *)(c + 8), *(int *)(c + 0xc),
                          *(int *)(c + 0x10) + 0xffe00000);
        }
        _Actor_SetAnim(b, 1);
    }
}
