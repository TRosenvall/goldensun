/*
 * OvlFunc_952_200be40 -- asm/overlays/rom_7d768c/ovl_30_c_a_c_a_a.s
 *
 * BLOCKER: gcc HOISTS a repeated two-instruction constant to the common
 * dominator. 119 lines against 121 -- two short, and the two are the extra
 * `push {r6}` and its pop.
 *
 * The function passes `0xa0 << 7` twice and `0xc0 << 6` three times, in two
 * different branches. The ROM rebuilds `mov r1,#0xa0 / lsl r1,#7` at each use.
 * gcc computes both once at the very top of the function -- BEFORE the first
 * call, ahead of even __SetFlag -- and holds them in r5 and r6.
 *
 * That is partial redundancy elimination hoisting to the dominator, not the
 * ordinary CSE recorded elsewhere in this file, and it is why the usual lever
 * fails: the per-use-site named locals that closed OvlFunc_948_20095f0 are
 * byte-identical here, whether all five are assigned at the top of the function
 * or each immediately before its own call. gcc folds the initialisers to the
 * same rtx and hoists once regardless.
 *
 * WHY THIS FUNCTION WAS PICKED, since the shape looked ideal and still failed:
 * 119 instructions, 38 calls, no high registers used, single-function file --
 * the profile the doc now recommends. It confirms that the 40-120 band helps
 * with ALLOCATION residues but does nothing for redundancy elimination, which
 * is a separate pass and fires on constants regardless of pressure.
 *
 * The message base 0x2233 with `add r0, r5, #1` and `#2` derivations IS
 * reproduced correctly by a plain `base = 0x2233;` local -- that part works.
  *
 * PROBED, five variants, and the rule is dominance -- not distance, not the
 * number of intervening calls:
 *
 *     both uses in mutually exclusive branches    -> rebuilds, push {lr}
 *     three uses, all in branches                 -> rebuilds, push {lr}
 *     one dominating use + one in a branch        -> HOISTS, push {r5,lr}
 *     the same with eight calls in between        -> HOISTS
 *     the same with the second use deep in an arm -> HOISTS
 *
 * THIS FUNCTION CONTRADICTS THAT. Its ROM rebuilds 0xa0<<7 at a dominating use
 * in the prologue and again inside the else branch, which under our flags gcc
 * will not do. There is no branch before the first use -- the listing is
 * push / SetFlag / CutsceneStart / Func_808e118 / the first 8092adc -- so the
 * dominance is not something I have mis-structured.
 *
 * The likeliest reading is that the two constants are DIFFERENT SYMBOLS in the
 * original and coincide only in value. That is inference from a contradiction
 * rather than a measurement, and this tree has no symbol space to write it
 * with, so it is recorded and not acted on.
*/
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __Func_808e118(void);
extern void __Func_8092adc(int a, int b, int c);
extern void __Func_80921c4(int a, int b, int c);
extern void __MessageID(int id);
extern void __Func_8092c40(int a, int b);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __MapActor_DoAnim(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);

void OvlFunc_952_200be40(void)
{
    int base;

    __SetFlag(0x96c);
    __CutsceneStart();
    __Func_808e118();
    __Func_8092adc(8, 0xa0 << 7, 0);
    __Func_8092adc(9, 0xc0 << 6, 0);
    __Func_80921c4(0, 0xc8, 0x88 << 1);
    __Func_8092adc(0, 0xc0 << 8, 0);
    __CutsceneWait(0x14);
    base = 0x2233;
    __MessageID(base);
    __Func_8092c40(8, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __CutsceneWait(0x14);
        __MessageID(base + 1);
        __ActorMessage(8, 0);
    } else {
        __CutsceneWait(0x14);
        __MessageID(base + 2);
        __ActorMessage(8, 0);
        __CutsceneWait(0x14);
        __Func_8092848(8, 9, 0x3c);
        __Func_8092adc(9, 0xc0 << 6, 0);
        __CutsceneWait(0x28);
        __Func_80925cc(9, 2);
        __CutsceneWait(0x1e);
        __Func_8092848(8, 9, 0x1e);
        __MapActor_DoAnim(9, 3);
        __CutsceneWait(0x1e);
        __MapActor_Emote(8, 0x81 << 1, 0x32);
        __Func_8092adc(8, 0xa0 << 7, 0);
        __Func_8092adc(9, 0xc0 << 6, 0);
        __CutsceneWait(0x14);
        __MapActor_DoAnim(8, 4);
        __CutsceneWait(0x14);
        __ActorMessage(8, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(8, 2);
        __CutsceneWait(0x14);
        __ActorMessage(8, 0);
    }
    __CutsceneEnd();
}
