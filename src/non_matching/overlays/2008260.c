/* OvlFunc_949_2008260 -- asm/overlays/rom_7d4af4/ovl_30_c_c_a_a_c.s
 *
 * BLOCKER: ARG-INTERLEAVE with no basic-block boundary to use. 3 of 20,
 * LENGTH EXACT.
 *
 * Two calls: a six-argument one whose last two arguments go on the stack, then
 * a three-argument one taking two shifted constants.
 *
 * ONE LEVER LANDED -- STACK ARGUMENTS NAMED PER CALL SITE, 8 differing to 3.
 * The ROM materialises BOTH stack constants into two registers and then stores
 * both (`mov r3,#3 / mov r2,#0x1a / str r3 / str r2`); written as literals gcc
 * builds and stores each in turn, reusing one register. Naming them as two
 * locals is docs/elevation.md's stack-argument rule, and it fixed the whole
 * first cluster.
 *
 * WHAT REMAINS is the arg-interleave at the second call:
 *
 *     rom    mov r1,#0xe0 / mov r2,#0xd4 / mov r0,#0x66 / lsl r1,#14 / lsl r2,#17
 *     ours   mov r1,#0xe0 / mov r2,#0xd4 / lsl r1,#14 / lsl r2,#17 / mov r0,#0x66
 *
 * THIS FUNCTION'S KIN DOCUMENTS THE LEVER FOR EXACTLY THIS SHAPE.
 * src/overlays/rom_7d4af4/ovl_30_c_c_a_a_b.c -- the other half of the same
 * split, and the first function ever matched through this blocker -- records
 * that the trigger is WHERE THE VALUE IS ASSIGNED:
 *
 *     literal at the call site                          contiguous
 *     named local, SAME basic block as the call         held in a register
 *     named local, DIFFERENT basic block                *** INTERLEAVED ***
 *
 * THE LEVER CANNOT BE APPLIED HERE, because this function has no branch. It is
 * eight instructions of straight-line code and two calls; there is no second
 * basic block to assign the constants in. Both available placements measured:
 *
 *   shifted constants as locals, assigned before the FIRST call
 *                                                   23 lines, 20 differ
 *   shifted constants as locals, same block as the call
 *                                                   20 lines,  3 differ
 *                                                   (byte-identical to literals)
 *
 * The first is the useful measurement: a CALL DOES NOT SUBSTITUTE FOR A BRANCH.
 * Assigning the constants before the first call does not put them in a
 * different basic block -- it makes them LIVE ACROSS a call, so gcc keeps them
 * in callee-saved registers, pushes r4 and r5 that the ROM never pushes, and
 * the function grows three lines. That confirms from the other direction what
 * docs/elevation.md already says about calls not creating block boundaries.
 *
 * So this is the same boundary as ovl_7cb2c0/200dca4 and 200bdec: a
 * straight-line function whose only defect is an arg-interleave is unreachable,
 * and the kin's lever needs a branch this function does not have.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, unsigned int b, unsigned int c);

void OvlFunc_949_2008260(void)
{
    int s1;
    int s2;

    s1 = 3;
    s2 = 0x1a;
    __Func_8010704(3, 0x20, 1, 1, s1, s2);
    __Func_808edac(0x66, 0xe0 << 14, 0xd4 << 17);
}
