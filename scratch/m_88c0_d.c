extern unsigned char *iwram_3001e70;
extern unsigned char L61d0[] __asm__(".L61d0");
extern unsigned char L61e8[] __asm__(".L61e8");
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_883_2008244(int a, int b, int c, int d, int e, int f);

int OvlFunc_883_20088c0(int slot)
{
    int pad[2];
    int v[6];
    unsigned char *base;
    unsigned char *e;
    unsigned char *tp;
    int off;
    unsigned int i;
    unsigned int n;
    int m;
    int idx;
    int a, b, w, h;
    int px, pz;
    int sx, sz;

    base = iwram_3001e70;
    e = __MapActor_GetActor(slot);
    m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
    i = 0;
    tp = L61d0;
    off = 0;
    if (m == *(int *)(tp + off)) {
        v[0] = i;
    } else {
        for (;;) {
            v[0] = 7;
            i++;
            if (i > 5)
                goto done;
            off += 4;
            m = **(short **)(*(unsigned char **)(e + 0x50) + 0x28);
            if (m == *(int *)(tp + off)) {
                v[0] = i;
                break;
            }
        }
    }
done:
    n = v[0];
    if (n > 6)
        return 0;
    px = *(int *)(e + 8);
    v[2] = px;
    v[3] = *(int *)(e + 0xc);
    pz = *(int *)(e + 0x10);
    v[4] = pz;
    idx = n << 4;
    a = *(int *)(L61e8 + idx + 4);
    if (a < 0)
        a = -a;
    b = *(int *)(L61e8 + idx + 0xc);
    if (b < 0)
        b = -b;
    h = (a + b) >> 4;
    a = *(int *)(L61e8 + idx);
    w = a;
    if (a < 0)
        w = -a;
    b = *(int *)(L61e8 + idx + 8);
    if (b < 0)
        b = -b;
    sx = ((a << 16) + px) >> 20;
    sz = ((*(int *)(L61e8 + idx + 4) << 16) + pz) >> 20;
    w = (w + b) >> 4;
    v[2] = sx;
    v[4] = sz;
    __Func_8010704(sx, sz,
                   w, h,
                   (*(int *)(base + (0x9e << 1)) >> 20) + sx,
                   (*(int *)(base + (0xa0 << 1)) >> 20) + sz);
    OvlFunc_883_2008244(0, v[2], v[4], w, h, 0xff);
    OvlFunc_883_2008244(2, v[2], v[4], w, h, 0xff);
    return 1;
}
