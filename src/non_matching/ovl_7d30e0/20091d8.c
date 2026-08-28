/* OvlFunc_948_20091d8 -- NON-MATCHING.  Blocker class: CONSTANT CSE, NO BOUNDARY.
 *
 * 19 lines against the ROM's 20, 8 differing, and the whole cause is one
 * instruction the ROM has and we do not:
 *
 *     rom   mov r1, #0x80 / mov r2, #0x80 / mov r0, #0xc / lsl r1, #12 / lsl r2, #12
 *     ours  mov r2, #0x80 / lsl r2, #12   / mov r0, #0xc / mov r1, r2
 *
 * __MapActor_SetPos is called with the SAME value, 0x80 << 12, for both x and
 * y.  The ROM builds it twice.  gcc-2.96 builds it once and copies the
 * register, which is one instruction shorter -- hence 19 lines against 20.
 *
 * This is the class tools/blocked_cse.py measures and docs/elevation.md
 * describes under "Pool-constant CSE: the complete rule": a constant costing
 * two instructions to build, used twice with the first use dominating the
 * second, is hoisted by gcc-2.96.  Recovering the rebuild needs BOTH a
 * control-flow boundary between the uses AND -fno-rerun-cse-after-loop.  Here
 * the two uses are adjacent arguments of ONE call, so there is no boundary to
 * be had and the flag alone cannot reach it.
 *
 * That is 585 functions, 27% of the remaining band and 51% of its
 * instructions, so this park is a representative rather than a special case.
 *
 * Tried:
 *   - both arguments as the inline expression `0x80 << 12`:  8 differing
 *   - each argument in its own named local, assigned separately: 8 differing,
 *     byte-identical output.  Two names do not make two values; gcc CSEs on
 *     the value, not the spelling.
 *
 * The stack-argument half of the function is already exact -- the two named
 * locals for the fifth and sixth arguments of __Func_80105d4 reproduce the
 * `str r3, [sp] / str r2, [sp, #4]` pair -- so the only defect is the CSE.
 */
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_948_20091d8(void)
{
    int s1, s2;

    s1 = 0x19;
    s2 = 0x30;
    __Func_80105d4(0x18, 0x30, 1, 2, s1, s2);
    __MapActor_SetPos(0xc, 0x80 << 12, 0x80 << 12);
}
