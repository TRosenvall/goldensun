// fakematch
/* Cluster OvlFunc_882_2008400..OvlFunc_882_2008400 extracted from goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a.s.
 *
 * FAKEMATCH -- matched by pinning a register with inline asm, not by finding the
 * construct. Authorised as an interim measure; every one of these is on the
 * worklist in reports/fakematch-worklist.md for a later pass.
 *
 * THE REAL BLOCKER is the straight-line half of the arg-interleave class. The
 * ROM materialises an expensive operand in TWO PIECES with another argument
 * scheduled into the gap, and gcc emits it in one piece. Batch 37 found the
 * lever for functions WITH a branch -- assign the value in a different basic
 * block -- and it needs a block boundary, which a straight-line function does
 * not have. See reports/arg-interleave.md.
 *
 * WHAT A FUTURE PASS SHOULD KNOW, so it does not start over:
 *
 *   * `volatile` on the local produces the RIGHT ORDERING with no inline asm.
 *     It is not usable because it also forces a stack slot -- `sub sp,#4 / str
 *     r1,[sp] / add sp,#4`, three instructions the ROM does not have. That is
 *     worth knowing precisely: the ordering is reachable in plain C, and the
 *     only thing wrong with `volatile` is the memory traffic.
 *   * So what is wanted is a REGISTER-LEVEL volatile -- exactly what the
 *     `__asm__ volatile ("" : : "r" (x))` barrier below is standing in for.
 *   * Ruled out by direct experiment: the literal at the call site, a named
 *     local assigned at its declaration or as a separate statement, both
 *     operands as locals in either order, a nested block, a comma expression,
 *     `const`, `* 2` instead of `<< 1`, an extern, and a parameter. Twelve
 *     formulations, all contiguous. Eight more are recorded in
 *     src/non_matching/overlays/interleaved_arg_setup.c.
 */
void OvlFunc_882_200815c(int);
void __CutsceneWait(int);
void __Func_8010560(void *, int, int);
extern void __PlaySound(int a);
extern unsigned char L57a0[] __asm__(".L57a0");

void OvlFunc_882_2008400(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L57a0, 0x26, 0x48);
    {
        register unsigned int r1v __asm__("r1") = 0x92;
        __asm__ volatile ("" : : "r" (r1v));
        __Func_809218c(0, r1v, 0x49e);
    }
    __CutsceneWait(3);
    OvlFunc_882_200815c(0xd);
}
