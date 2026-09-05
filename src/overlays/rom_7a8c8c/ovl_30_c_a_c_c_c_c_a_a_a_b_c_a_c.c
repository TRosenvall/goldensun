/* OvlFunc_922_20092cc  --  0x020092cc
 *
 * Cut out of goldensun/asm/overlays/rom_7a8c8c/ovl_30_c_a_c_c_c_c_a_a_a_b_c_a_c.s,
 * which holds this function alone.
 *
 * Queues a fixed set of dialogue lines, choosing between alternatives on ten
 * save flags. Nineteen calls to the same six-argument helper, six to a
 * three-argument one, and nothing else. Structurally the twin of its immediate
 * sibling OvlFunc_922_2009154 (ovl_30_c_a_c_c_c_c_a_a_a_b_c_a_b.c), which is
 * where the reading of the stack-argument pairs came from.
 *
 * NO PINS. Three named locals and the right NAME on each is the whole job.
 *
 * The `str` operands say which values get locals, as usual:
 *
 *     mov r3, #0xc / mov r2, #0x15 / str r3, [sp] / str r2, [sp, #4]
 *         -- both stack arguments built fresh into their own register: this
 *            site wants its own pair of locals.
 *     mov r3, #0x14 / mov r5, #0x18 / str r3, [sp] / ... / str r5, [sp, #4]
 *         -- r5 is callee-saved and survives three calls: one local assigned
 *            once for the held value, a fresh one per site for the other.
 *
 * WHAT WAS NEW HERE: WHICH LOCAL, NOT HOW MANY. A local gets ONE pseudo for
 * the whole function, so its hard register is chosen once and every use in the
 * body inherits it. This function needs r3 for the value stored to [sp] in its
 * first half and r3 for the value stored to [sp, #4] in its second -- the two
 * halves put the FRESH value in opposite argument slots, because the ROM holds
 * a different one of the pair in r5 on each side.
 *
 * So the split is not by argument position, it is by rebuilt-vs-held:
 *
 *     g   every value the ROM REBUILDS at each site          -> r3
 *     e   the second of a fresh pair                         -> r2
 *     a   every value the ROM HOLDS in r5 across calls       -> r5
 *
 * Naming by argument position instead -- e for the fifth argument everywhere,
 * g for the sixth -- keeps the length exact and costs 26 differing, every one
 * of them an r2/r3 swap, and all 26 in the first half. The second half was
 * already exact because there the position split and the rebuilt/held split
 * happen to coincide.
 *
 * The three locals were checked one at a time; each is load-bearing (see the
 * measured table in the batch report) and the surviving set was re-confirmed
 * byte-identical under objcmp.
 */

extern int __GetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_922_2009004(int a, int b, int c);

void OvlFunc_922_20092cc(void)
{
    int a;
    int e;
    int g;

    g = 0xc;
    e = 0x15;
    __Func_8010704(0xc, 3, 9, 0x10, g, e);
    if (__GetFlag(0xc2 << 2) != 0) {
        OvlFunc_922_2009004(8, 0xe, 0x19);
        g = 0x14;
        e = 0x18;
        __Func_8010704(0x10, 0x18, 1, 3, g, e);
    } else if (__GetFlag(0x309) != 0) {
        OvlFunc_922_2009004(8, 0x11, 0x19);
        g = 0x14;
        a = 0x18;
        __Func_8010704(0x12, 0x18, 1, 3, g, a);
        g = 0xe;
        __Func_8010704(0x12, 0x18, 1, 3, g, a);
        g = 0x11;
        __Func_8010704(8, 0x29, 1, 3, g, a);
    } else {
        OvlFunc_922_2009004(8, 0x14, 0x19);
        g = 0xe;
        e = 0x18;
        __Func_8010704(0x10, 0x18, 1, 3, g, e);
    }
    if (__GetFlag(0x30a) != 0) {
        OvlFunc_922_2009004(9, 0xd, 0x23);
        g = 0xf;
        e = 0x22;
        __Func_8010704(0xe, 0x22, 1, 3, g, e);
    } else {
        OvlFunc_922_2009004(9, 0xf, 0x23);
        g = 0xd;
        e = 0x22;
        __Func_8010704(0xe, 0x22, 1, 3, g, e);
    }
    if (__GetFlag(0x30b) != 0) {
        OvlFunc_922_2009004(0xa, 0xf, 0x16);
        a = 0xe;
        g = 0x1e;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x16;
        __Func_8010704(5, 0x29, 3, 1, a, g);
    } else if (__GetFlag(0xc3 << 2) != 0) {
        OvlFunc_922_2009004(0xa, 0xf, 0x17);
        a = 0xe;
        g = 0x17;
        __Func_8010704(5, 0x2a, 3, 1, a, g);
        g = 0x1e;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x15;
        __Func_8010704(0xa, 0x2c, 3, 1, a, g);
    } else if (__GetFlag(0x30d) != 0) {
        OvlFunc_922_2009004(0xa, 0xf, 0x1a);
        a = 0xe;
        g = 0x16;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x1a;
        __Func_8010704(5, 0x2b, 3, 1, a, g);
        g = 0x1e;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
    } else if (__GetFlag(0x30e) != 0) {
        OvlFunc_922_2009004(0xa, 0xf, 0x1b);
        a = 0xe;
        g = 0x16;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x1e;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x1b;
        __Func_8010704(5, 0x2c, 3, 1, a, g);
    } else {
        OvlFunc_922_2009004(0xa, 0xf, 0x1e);
    }
    if (__GetFlag(0x30f) != 0) {
        OvlFunc_922_2009004(0xb, 0xf, 0x17);
        a = 0xe;
        g = 0x1f;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x17;
        __Func_8010704(0xa, 0x28, 3, 1, a, g);
    } else if (__GetFlag(0xc4 << 2) != 0) {
        OvlFunc_922_2009004(0xb, 0xf, 0x18);
        a = 0xe;
        g = 0x1f;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x18;
        __Func_8010704(0xa, 0x29, 3, 1, a, g);
    } else if (__GetFlag(0x311) != 0) {
        OvlFunc_922_2009004(0xb, 0xf, 0x1b);
        a = 0xe;
        g = 0x1f;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x1b;
        __Func_8010704(0xa, 0x2a, 3, 1, a, g);
    } else if (__GetFlag(0x312) != 0) {
        OvlFunc_922_2009004(0xb, 0xf, 0x1c);
        a = 0xe;
        g = 0x1f;
        __Func_8010704(0xe, 0x1d, 3, 1, a, g);
        g = 0x1c;
        __Func_8010704(0xa, 0x2b, 3, 1, a, g);
    } else {
        OvlFunc_922_2009004(0xb, 0xf, 0x1f);
    }
}
