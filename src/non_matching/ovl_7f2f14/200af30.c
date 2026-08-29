/* OvlFunc_968_200af30 -- NON-MATCHING.
 * Blocker class: SHARED-BASE CONSTANT CSE, not argument precompute.
 * 38 lines against the ROM's 37, 21 differing, and the very first line is the
 * tell: our prologue is `push {r5, lr}` where the ROM's is `push {lr}`.
 *
 * The function uses 0x80 << 7 twice -- once as the y speed and again as the
 * walk distance -- and 0x80 << 8 once. gcc builds 0x4000 into r5 BEFORE the
 * first call and keeps it there across three calls; the ROM rebuilds it each
 * time from `mov rN, #0x80 / lsl rN, #7`.
 *
 * WHY THE CENSUS MISCLASSIFIES THIS. tools/census.py checks const_remat before
 * precompute, but its const_remat test only recognises the `neg` form -- the
 * same value negated into two argument registers. It does not see two argument
 * values that SHARE A BASE and are reached by different shifts, which is what
 * happens here, so the function falls through to precompute. The class label
 * on this function in any census output is wrong, and the same is likely true
 * of others.
 *
 * Tried:
 *   - `0x80 << 8` and `0x80 << 7` written as the folded constants 0x8000 and
 *     0x4000: identical, 21 differing. gcc still relates them -- it emits
 *     `lsl r5, #7` and derives the other, so writing the result rather than
 *     the shift does not hide the shared base.
 *
 * This is the same family as ovl_7b2078/2008388.c and rom_15000/801c954.c, the
 * two recorded counterexamples to the pool-constant CSE remedy, and it has the
 * same shape: a boundary exists (three calls), and nothing reaches the rebuild.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_968_2008058(unsigned int, unsigned int, unsigned int, unsigned int);
extern void __Func_8092708(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_968_200af30(void)
{
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __Func_80921c4(0, 0x82 << 2, 0xb2 << 2);
    __Func_8092adc(0, 0x80 << 7, 0xa);
    OvlFunc_968_2008058(0x82 << 18, 0, 0xc4 << 18, 0xdf);
    __Func_8092708(0, 6, 0);
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x14);
    __CutsceneEnd();
}
