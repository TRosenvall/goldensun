/* OvlFunc_924_20098f8  --  0x020098f8, split out of asm/overlays/rom_7ac2d8/ovl_f84_c_a.s;
 * OvlFunc_924_20097a8 stays in _a.s and OvlFunc_924_20099b8 in _c.s. Matched
 * from scratch.
 *
 * The ROM's `-(i << 17)` counter is a strength-reduced induction variable, so
 * the inner loop is a real `for` -- the `goto` form its sibling 200a030 uses is
 * wrong here. `k = 0;` assigned before `one = 1;` closed the last two.
 */
struct P {
    int f0;
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x28 - 0x10];
};

extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern unsigned int __Random(void);
extern void __CutsceneWait(int n);
extern void __StopTask(void (*f)(void));
extern void OvlFunc_924_2009790(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a,
                                int b, int c, int d, struct P *p);

void OvlFunc_924_20098f8(void)
{
    struct P p;
    int one;
    unsigned int k, i;

    __CopyMapTiles(0x4e, 0x3a, 0x6e, 0x24, 1, 1);
    p.f4 = 5;
    p.f8 = 0x80 << 8;
    p.fc = 0x80 << 8;
    k = 0;
    one = 1;
    do {
        for (i = 1; i <= 7; i++) {
            if (i & one) {
                OvlFunc_common0_10c(0xb6 * 0x40000 - (i << 17) - (k << 19), 0,
                                    (0x248 - (__Random() * 5 >> 16)) << 16,
                                    -0x4000, 0, 0, 0x90 << 12, &p);
                __CutsceneWait(1);
            }
        }
        __CopyMapTiles(0x6f, 0x23, 0x6d - k, 0x24, one, one);
        k++;
    } while (k <= 2);
    __StopTask(OvlFunc_924_2009790);
}
