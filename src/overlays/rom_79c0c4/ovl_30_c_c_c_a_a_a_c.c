// fakematch
/* Cluster OvlFunc_908_20081a8..OvlFunc_908_20081a8 extracted from goldensun/asm/overlays/rom_79c0c4/ovl_30_c_c_c_a_a_a.s.
 *
 * The .s held ONLY this function and no data after the earlier split, so no
 * further split was needed.
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
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __Func_809280c(int a, int b, int c);
extern void __ActorMessage(int a, int b);
extern void __Func_8092adc(unsigned int a, unsigned int b, int c);

void OvlFunc_908_20081a8(void)
{
    unsigned int w;

    __CutsceneStart();
    __MessageID(0x13ed);
    __Func_809280c(0x15, 0, 0);
    __ActorMessage(0x15, 0);
    w = 0xc0;
    {
        register unsigned int rq __asm__("r0") = 0x15;
        __asm__ volatile ("" : : "r" (rq));
        w <<= 8;
        __Func_8092adc(rq, w, 0xa);
    }
    __CutsceneEnd();
}
