/* OvlFunc_933_2009054  --  0x02009054, split out of
 * asm/overlays/rom_7bc690/ovl_4e4_c_a.s; OvlFunc_933_2008e2c stays in _a.s.
 * Matched from scratch. `g = gState` as a local; the bare `iwram_3001e40;`
 * statement reproduces the ROM's discarded volatile re-read.
 */
struct P {
    unsigned char pad00[8];
    int f8;
    int fc;
    unsigned char pad10[0x22 - 0x10];
    unsigned short f22;
    unsigned char pad24[4];
};

struct Actor {
    unsigned char pad00[6];
    unsigned short f6;
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x30 - 0x14];
    int f30;
    unsigned char pad34[4];
    int f38;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001ebc;
extern volatile int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_933_2009054(void)
{
    struct P t;
    unsigned char *g;
    unsigned char *e;
    struct Actor *a;
    int off;
    int v;
    int r;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + (0xfa << 1)));
    e = iwram_3001ebc;
    if (a->f38 == (int)0x80000000)
        return;
    *(unsigned short *)(g + 0x232) += 1;
    if (*(short *)(e + (0xb6 << 1)) == 0x1e)
        return;
    if (a->f30 <= 0x80 << 9) {
        if ((iwram_3001e40 & 0xf) != 0)
            return;
        t.f8 = 0x80 << 8;
        t.fc = 0x80 << 8;
        off = 0;
        t.f22 = (__Random() << 12 >> 16) + (0xf8 << 8);
        if (a->f6 != 0 && a->f6 != 0x80 << 8)
            off = (0x80 << 10) - ((((a->f10 >> 20) & 1) * 5) << 16);
        OvlFunc_common0_10c(a->f8 + off, a->fc, a->f10, 0, 0, 0, 0x880001, &t);
    } else {
        v = iwram_3001e40 & 7;
        if (v != 0)
            return;
        iwram_3001e40;
        t.f8 = 0xcccc;
        t.fc = 0xcccc;
        t.f22 = (__Random() << 12 >> 16) + (0xf8 << 8);
        r = ((__Random() * 5) >> 16) * 0x1999;
        OvlFunc_common0_10c(a->f8, a->fc + (0x80 << 10), a->f10, 0, r, v, 0x880001, &t);
    }
}
