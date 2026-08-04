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
 */
void OvlFunc_899_2008428(void)
{
    __CutsceneStart();
    __MessageID(0x1253);
    OvlFunc_899_20083bc(0xf);
    __Func_8092adc(0xf, 0x80 << 8, 0);
    __CutsceneEnd();
}
