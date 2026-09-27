/* Func_80933f8  --  0x080933f8, was asm/rom_8a000/rom_93304_a_a_a_a_c.s (this
 * function alone), so it converts whole. Matched from scratch; iwram_3001e70 is
 * used as the pointer itself (one load), as in SetCameraTarget.
 */
struct C {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
};

extern unsigned char *iwram_3001e70;
extern unsigned char *galloc_ewram(int slot, int size);
extern void _Actor_Stop(struct C *c);
extern void _Actor_TravelTo(struct C *c, int x, int y, int z);
extern void WaitFrames(int n);
extern void _Func_800fe9c(void);

void Func_80933f8(int x, int y, int z, int flags)
{
    unsigned char *m;
    struct C *c;
    unsigned char *g;
    int *t;
    int xlo, zlo, xhi, zhi;

    m = galloc_ewram(0x1b, 0xccc);
    c = *(struct C **)(m + (0xf0 << 1));
    g = iwram_3001e70;
    xlo = *(int *)(g + 0xec) + 0x780000;
    zlo = *(int *)(g + 0xf0) + c->fc + 0x600000;
    xhi = *(int *)(g + 0xf4) - 0x780000;
    zhi = *(int *)(g + 0xf8) + c->fc - 0x400000;
    t = &c->f8;
    *(int **)g = t;
    _Actor_Stop(c);
    if (x == -1)
        x = *t;
    if (y == -1)
        y = c->fc;
    if (z == -1)
        z = c->f10;
    if (x < xlo)
        x = xlo;
    if (z < zlo)
        z = zlo;
    if (x > xhi)
        x = xhi;
    if (z > zhi)
        z = zhi;
    if (flags == 0) {
        *t = x;
        c->fc = y;
        c->f10 = z;
        WaitFrames(1);
        if (*(short *)(m + (0xcf << 1)) != 3)
            _Func_800fe9c();
    } else {
        _Actor_TravelTo(c, x, y, z);
    }
}
