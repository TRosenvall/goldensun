// fakematch
/* OvlFunc_949_200828c  --  0x0200828c
 *
 * From goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_a_c_c.s, which held this function alone, so no split was
 * needed.
 *
 * Member of shape group 1 (flat, {add, bl, bx, mov, neg, pop, push, str, sub})
 * at the widened 45-instruction cut.
 *
 * TWO KNOWN SHAPES, NOTHING NEW.
 *
 * 1. A STACK-ARGUMENT PAIR. The six-argument call passes its last two on the
 *    stack, and the ROM builds BOTH before storing either --
 *    `mov r3 / mov r2 / str r3, [sp] / str r2, [sp, #4]` -- where gcc reuses
 *    one register for both. Two named locals assigned before the call restore
 *    it; same instruction count, different registers, so the length never moves.
 *
 * 2. THE `-1` PAIR, grouped rather than interleaved:
 *
 *        mov r1, #1 / mov r2, #1 / mov r0, #0x66 / neg r1, r1 / neg r2, r2
 *
 *    Both movs, then the third argument, then both negations. Batch 192 found
 *    that a `-1` TRIPLE needs its assignments and negations INTERLEAVED,
 *    because three registers receiving the same value have nothing to order
 *    them. This is a pair, not a triple, and it wants the opposite: written
 *    grouped in the ROM's order with the registers pinned, it matches. The
 *    batch-192 rule is about what to do when ordering fails, not a shape to
 *    apply everywhere.
 */

extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);

void OvlFunc_949_200828c(void)
{
    int e;
    int g;

    e = 3;
    g = 0x1a;
    __Func_8010704(2, 0x19, 1, 1, e, g);
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        q1 = 1;
        q2 = 1;
        q0 = 0x66;
        q1 = -q1;
        q2 = -q2;
        __Func_808edac(q0, q1, q2);
    }
}
