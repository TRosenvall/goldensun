/* SOLVED FOR FUNCTIONS WITH A BRANCH. OvlFunc_969_2009280 was unparked in
 * batch 37 by the basic-block lever -- assign the pooled values to named locals
 * in a DIFFERENT BASIC BLOCK from the call and gcc rematerialises them there,
 * after `mov r0` instead of before it. See reports/arg-interleave.md.
 *
 * That also settled something this file got wrong: this is NOT a separate class
 * from arg-interleave. Both are gcc emitting an expensive operand in one piece
 * where the ROM emits it in two with another argument in the gap; one displaces
 * a shift and the other a pool load, and the same lever moves both.
 *
 * THE THREE REMAINING MEMBERS ARE STRAIGHT-LINE and stay parked. There is no
 * block boundary to put between the assignment and the call. OvlFunc_882_20083cc
 * written with a named local goes from 2 differing instructions to 4.
 *
 * The note below is kept as written, including its speculation about
 * fakematches, because the negative results in it are still valid -- they are
 * what ruled out the whole call-site axis and made it clear the answer had to be
 * somewhere else.
 *
 * THREE overlay functions and one now elevated, one shared blocker. An eighth class.
 *
 *   OvlFunc_882_2008398  asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a.s
 *   OvlFunc_882_20083cc  asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a.s
 *   OvlFunc_882_2008400  asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a.s
 *   OvlFunc_969_2009280  asm/overlays/rom_7f6e64/ovl_314_c_a_c_c_a_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_a.s
 *
 * POOL LOADS COME FIRST. Within one argument block gcc-2.96 emits every
 * literal-pool load before any `mov`, whatever order the arguments are written
 * in. The ROM emits them in source order. Two positions differ and nothing
 * else does:
 *
 *   OvlFunc_882_20083cc, 17 against 17
 *     rom    mov r1, #0x66 / ldr r2, =0x4b6 / mov r0, #0
 *     ours   ldr r2, =0x4b6 / mov r1, #0x66 / mov r0, #0
 *
 *   OvlFunc_969_2009280, 27 against 27
 *     rom    mov r0, r5 / ldr r1, =0xcccc / ldr r2, =0x6666
 *     ours   ldr r1, =0xcccc / ldr r2, =0x6666 / mov r0, r5
 *
 * The two look different -- one is a small immediate displaced, the other is
 * `r0` displaced -- and they are the same rule seen twice. In both, every
 * pooled operand moves ahead of every non-pooled one and their relative orders
 * are otherwise preserved.
 *
 * WHY THIS IS NOT THE DECLARATION LEVER'S CLASS, which is what it looks like.
 * That lever reorders argument construction (batch 31 widened it from "where
 * r0 lands" to "the order of the whole block), and it does not reach this:
 *
 *   * the mismatching callee declared, undeclared, and with the third
 *     parameter widened to `unsigned` -- all identical output
 *   * the PRECEDING callee undeclared, and with its return type swept through
 *     char, short, unsigned, void *, unsigned char *, long long, float and
 *     double -- every non-void return goes from 2 differing positions to 5,
 *     which is the return value being kept live, not a reordering
 *   * `__PlaySound` given a return type as well -- no change
 *
 * AND IT IS NOT SCHEDULING. -fno-schedule-insns, -fno-schedule-insns2,
 * -fno-peephole, -fno-force-mem, -fno-caller-saves, -fno-expensive-optimizations
 * and -fno-cse-follow-jumps all leave the output byte-identical. Naming the
 * values as locals immediately before the call, in the ROM's order -- the
 * stack-arg-pair lever's trick -- does not move them either, for either
 * function, whether one value is named or all three.
 *
 * THE PRECEDENT SAYS THIS IS A WALL. `OvlFunc_883_2008fbc`
 * (src/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_b.c) has the FIRST of
 * these two shapes exactly -- the same callee, the same 0x66, the same 0x4b6 --
 * and it is in fakematch.txt, matched by pinning r0 and r1 with
 * `register ... __asm__("r0")` and an empty `__asm__ volatile` barrier. So the
 * shape was reached once before and only that way.
 *
 * These four are left as assembly rather than adding four more fakematches.
 * That is a judgement call and it should be reviewed: the inline-asm form does
 * produce the right bytes, and if the maintainer would rather have C with a
 * barrier than assembly, the change is mechanical and this note lists every
 * member. The argument for leaving them is that a fakematch records "we could
 * not find the source construct" in a form that compiles, and four more of
 * them in one family makes the family look solved when it is not.
 *
 * WHAT WOULD ACTUALLY SOLVE IT is a construct that makes gcc treat the pooled
 * value as available LATER than the register one -- the reverse of every lever
 * in docs/elevation.md, all of which make something live EARLIER. Nothing in
 * the tree does that yet.
 *
 * The body below is OvlFunc_882_20083cc, the smallest member, so that
 * tools/audit_parks.py screens the class against a real reference.
 */

void OvlFunc_882_200815c(int);
void __CutsceneWait(int);
void __Func_8010560(void *, int, int);
void __Func_809218c(int, int, int);
extern void __PlaySound(int a);
extern unsigned char L578a[] __asm__(".L578a");

void OvlFunc_882_20083cc(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L578a, 0x23, 0x49);
    __Func_809218c(0, 0x66, 0x4b6);
    __CutsceneWait(3);
    OvlFunc_882_200815c(0xc);
}
