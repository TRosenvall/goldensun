/* OvlFunc_899_2008428  [ovl_794ac0]
 *
 * Source asm: goldensun/asm/overlays/rom_794ac0/ovl_30_a_c_a_a_c_c_c_a_a.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function and
 * no data. It is left in place because it does not match.
 *
 * Fourteen instructions, thirteen of them right, and the whole diff is where
 * ONE `mov r0` sits.
 *
 * Blocker: ARG-INTERLEAVE. The ROM puts r0 BETWEEN the two halves of building
 * r1, and gcc puts it after both:
 *
 *     rom    mov r1, #0x80 / mov r0, #0xf / lsl r1, #0x8 / mov r2, #0x0
 *     ours   mov r1, #0x80 / lsl r1, #0x8 / mov r2, #0x0 / mov r0, #0xf
 *
 * THIS IS NOT THE SIMPLE FILL-ORDER CLASS, and the distinction is the reason
 * this note exists. Both declaration levers were tried and NEITHER moves it:
 *
 *   1. `extern void __Func_8092adc(int, int, int);` -- declaring the
 *      mismatching callee, which is what fixed the two functions elevated
 *      alongside this one in the same round
 *   2. `extern void OvlFunc_899_20083bc(int);` -- declaring the PRECEDING
 *      call, so r0 is not held live across it
 *   3. both together
 *   4. the shifted argument in a named local, to change what gcc is scheduling
 *
 * All four are byte-identical to the form below. The levers move r0 to the
 * FRONT or the BACK of an argument block; they do not place it in the MIDDLE
 * of another argument's construction. That is a scheduling decision, not a
 * declaration one, and nothing in the tree reaches it.
 *
 * Worth recording because the symptom -- "one mov r0 in the wrong place" --
 * looks identical to the fill-order class that the declaration lever retires,
 * and reaching for that lever here costs four screens. The tell is whether the
 * misplaced mov is OUTSIDE the other arguments' setup (fill order, fixable) or
 * INSIDE it (interleave, not).
 *
 * LATER (this round): a twin was found that MATCHES with the identical rom
 * sequence, which narrows the precondition rather than the lever.
 *
 * OvlFunc_921_20087a4 emits exactly `mov r1, #0x80 / mov r0, #0xc / lsl r1,
 * #7 / mov r2, #0xa` -- r0 in the middle of r1's construction, the same shape
 * called impossible above -- and it matches with the shifted value in a named
 * local assigned at the TOP of the function.  Attempt 4 above tried that here
 * and it did not work.
 *
 * The difference is what else is live.  OvlFunc_921_20087a4 holds an actor
 * pointer and a facing value across its calls, so it already pushes r5; the
 * named constant costs nothing extra and gcc is free to schedule its
 * materialisation late.  This function holds nothing across a call, so naming
 * the constant at the top makes it the ONLY long-lived value, gcc puts it in
 * r5, and the prologue grows to `push {r5, lr}` -- 15 differing, far worse.
 *
 * So the lever is not "name the shifted value"; it is "name the shifted value
 * WHEN a callee-saved register is already committed".  Nothing the C can do
 * creates that precondition here without inventing a live value the ROM does
 * not have.  Two further attempts this round, the plain constant 0x8000 and
 * dropping the prototype on __Func_8092adc, both stay at 2 differing.
 *
 * CORRECTED by src/non_matching/ovl_7b2078/200a68c.c.  The precondition stated
 * above -- that naming the shifted value works when a callee-saved register is
 * already committed -- is NOT sufficient.  OvlFunc_926_200a68c holds two
 * arguments across all eight of its calls, so it saves r5 and r6 before any of
 * this is decided, and naming its two shifted values STILL fails: 36 differing
 * and eight lines long, because the named locals want registers beyond the two
 * already committed and gcc spills.
 *
 * The narrower reading that survives both cases: the named constant has to fit
 * in registers the function was going to save anyway, with nothing else
 * competing for them.  One held value plus one named constant fits, which is
 * why OvlFunc_921_20087a4 matches.  Two plus two does not.  That is a property
 * of how many values are live at the call rather than something the C selects,
 * so it is a coincidence to check for, not a lever to reach for.
 *
 * READ THIS FIRST, AND BEFORE ANY OTHER FUNCTION IN THIS CLASS.
 *
 * This is the ARGUMENT PRECOMPUTE class, and it was diagnosed out of the
 * compiler sources long before this park was written.  See HANDOFF.md,
 * "Argument precompute: DIAGNOSED, and it is a compiler difference", and the
 * full derivation in src/non_matching/ovl_780898/2008dc0.c:
 *
 *     calls.c:805  precompute_register_parameters() copies any argument whose
 *                  rtx_cost > 2 into a pseudo BEFORE any hard register is
 *                  loaded, under SMALL_REGISTER_CLASSES && reg_parm_seen.
 *     arm.h:1061   SMALL_REGISTER_CLASSES is TARGET_THUMB -- always 1 here.
 *     arm.c:2042   In Thumb, ASHIFT costs 4, so a shifted constant is
 *                  "expensive" and gets hoisted; a cheap `mov` is emitted
 *                  afterwards and lands last.
 *
 * The ROM's compiler did not precompute.  That is the whole difference, and
 * HANDOFF.md states plainly that it is NOT FIXABLE FROM C -- eight source
 * spellings and eight flags are byte-identical to the default.
 *
 * The "callee-saved register already committed" theory recorded elsewhere in
 * these notes was an invented explanation for this same behaviour, arrived at
 * without reading the existing diagnosis.  It is not a lever and should not be
 * treated as one.  Anything above that reasons about which registers are
 * committed is describing a symptom of the hoist, not its cause.
 *
 * The predictive rule from HANDOFF.md, which is the useful part: a call
 * misorders when its argument list mixes cheap constants with expensive values
 * and a cheap one is not last.  A call whose arguments are ALL cheap constants
 * matches.  Check that before screening, not after.
 */
void OvlFunc_899_2008428(void)
{
    __CutsceneStart();
    __MessageID(0x1253);
    OvlFunc_899_20083bc(0xf);
    __Func_8092adc(0xf, 0x80 << 8, 0);
    __CutsceneEnd();
}
