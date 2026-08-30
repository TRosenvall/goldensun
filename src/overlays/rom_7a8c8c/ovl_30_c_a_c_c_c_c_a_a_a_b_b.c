/* Cluster OvlFunc_922_2009050..OvlFunc_922_2009050 extracted from
 * goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_a_a_b.s.
 *
 * Three save-bit tests, each choosing between two six-argument calls.  Matched
 * on the first screen by reading the `str` operands to decide which stack
 * arguments get names -- the same discriminator as
 * src/overlays/rom_7c5efc/ovl_30_c_a_c_c_a_c_a_b.c, elevated alongside it.
 *
 *   `n = 8` is NAMED: the ROM holds it in r5 and stores it with `str r5, [sp]`
 *   at two of the seven call sites.  A callee-saved register spent on a
 *   constant is the ROM asking for a local.
 *
 *   THE OTHER FIVE SITES GET THEIR OWN PAIR.  At each of them the ROM builds
 *   both stack arguments into separate registers before storing either --
 *   `mov r3, #0xb / mov r2, #0x21 / str r3, [sp] / str r2, [sp, #4]` -- which is
 *   what a fresh pair of locals produces.  Written as literals gcc reuses one
 *   register and interleaves the stores.
 *
 * The pairs are deliberately NOT shared between sites even where the values
 * repeat (0xb appears twice, 0x11 twice, 0x1d and 0x21 several times).  Sharing
 * them is the batch-144 failure: one pair across three calls scored 20
 * differing where separate pairs scored 2.
 */
extern int __GetFlag(int id);
extern void OvlFunc_922_2009004(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_922_2009050(void)
{
    int n;
    int p1, q1, p2, q2, p3, q3, p4, q4, p5, q5;

    n = 8;
    __Func_8010704(8, 0x2a, 0xf, 5, n, 0x1d);
    if (__GetFlag(0x301)) {
        OvlFunc_922_2009004(8, 0x16, 0x1f);
        __Func_8010704(9, 0x1e, 1, 3, n, 0x1e);
    } else {
        OvlFunc_922_2009004(8, 8, 0x1f);
        p1 = 0x16;
        q1 = 0x1e;
        __Func_8010704(9, 0x1e, 1, 3, p1, q1);
    }
    if (__GetFlag(0x302)) {
        OvlFunc_922_2009004(9, 0xc, 0x1d);
        p2 = 0xb;
        q2 = 0x21;
        __Func_8010704(0xe, 0x21, 3, 1, p2, q2);
    } else {
        OvlFunc_922_2009004(9, 0xc, 0x21);
        p3 = 0xb;
        q3 = 0x1d;
        __Func_8010704(0xe, 0x1d, 3, 1, p3, q3);
    }
    if (__GetFlag(0x303)) {
        OvlFunc_922_2009004(0xa, 0x12, 0x1d);
        p4 = 0x11;
        q4 = 0x21;
        __Func_8010704(0xe, 0x21, 3, 1, p4, q4);
    } else {
        OvlFunc_922_2009004(0xa, 0x12, 0x21);
        p5 = 0x11;
        q5 = 0x1d;
        __Func_8010704(0xe, 0x1d, 3, 1, p5, q5);
    }
}
