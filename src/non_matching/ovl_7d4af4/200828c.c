/* OvlFunc_949_200828c -- NOT MATCHING. 6 instructions in disagreeing regions, of 20.
 *
 * Source asm: goldensun/asm/overlays/rom_7d4af4/ovl_30_c_c_a_a_c.s
 *
 * Blocker classes: constant-CSE and arg-interleave, both straight-line.
 *
 * The second call passes -1 TWICE, and the ROM builds each one separately with
 * its own mov/neg pair, with r0 landing between the movs and the negs:
 *
 *     mov r1, #1 / mov r2, #1 / mov r0, #0x66 / neg r1, r1 / neg r2, r2
 *
 * TWO SEPARATE LOCALS DO NOT REACH IT. `p = -1; q = -1;` is the documented
 * first move and it produces
 *
 *     ... neg r2, r2 / mov r1, r2
 *
 * -- gcc materialises -1 once and COPIES it, which is the "separate variables
 * do not defeat a copy" result in docs/elevation.md, confirmed again here. Our
 * stream is 19 instructions against the ROM's 20 for exactly that reason.
 *
 * The r0 displacement is the second, independent problem: r0 sits inside the
 * other arguments' construction, and this function has no branches, so the
 * basic-block lever cannot reach it either (batch 42, local-alloc.c).
 *
 * The first call is fine and matches: a stack-arg pair named as two locals,
 * __Func_8010704 left undeclared because its r0 comes late.
 *
 * NEXT: nothing until the straight-line side of the basic-block lever is
 * retired. Both defects need the same thing and neither is reachable in plain
 * C as it stands.
 */
extern void __Func_808edac(int a, int b, int c);

void OvlFunc_949_200828c(void)
{
    int m;
    int n;

    m = 3;
    n = 0x1a;
    __Func_8010704(2, 0x19, 1, 1, m, n);
    __Func_808edac(0x66, -1, -1);
}
