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
 */
void OvlFunc_899_2008428(void)
{
    __CutsceneStart();
    __MessageID(0x1253);
    OvlFunc_899_20083bc(0xf);
    __Func_8092adc(0xf, 0x80 << 8, 0);
    __CutsceneEnd();
}
