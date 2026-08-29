/* OvlFunc_968_20098f8
 *
 * Cut out of goldensun/asm//overlays/rom_7f2f14/ovl_30_c_a_c_a_c_c_a_b.s.
 *
 * Repaints two tile blocks, then walks three actors and repaints under each.
 *
 * THIS TU NEEDS AN EXPLICIT -O2 RULE. It matches a
 * `rom_7f2f14/ovl_30_c_a_c_a_c_c%` wildcard that applies O1_CFLAGS, and at -O1
 * this function is 20 differing; at -O2 it is exact. An explicit rule beats a
 * pattern rule in make, so the wildcard is left alone for the files it is
 * right about.
 *
 * Screened by a parallel agent; re-verified here before wiring.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[4];
    int f10;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_968_2008910(int a, int b);

void OvlFunc_968_20098f8(void)
{
    struct A *a;
    unsigned int i;
    int m;
    int n;
    int p;
    int q;
    int x;
    int y;

    __MapActor_GetActor(8);
    __CutsceneStart();
    m = 0xc;
    n = 0x2c;
    __Func_8010704(0x13, 0x2c, 4, 1, m, n);
    p = 0xb;
    q = 0x33;
    __Func_8010704(0x11, 0x33, 2, 2, p, q);
    for (i = 0; i <= 2; i++) {
        a = __MapActor_GetActor(i + 8);
        x = a->f8 >> 20;
        y = a->f10 >> 20;
        __Func_8010704(0xc, 0x32, 1, 1, x, y);
    }
    OvlFunc_968_2008910(0xa, 9);
    __CutsceneEnd();
}
