/* OvlFunc_891_20095d4  [ovl_78c76c] and one sibling
 * Source asm: goldensun/asm/overlays/rom_78c76c/ovl_30_c_c_a_c_c_c_c_a_a.s
 *
 * Seventeen against seventeen, diverging at instruction 3 in the set-up for
 * the first call: the ROM interleaves TWO shifted constants and a plain
 * argument as
 *
 *     mov r1, #0xd0 / mov r2, #0xe0 / mov r0, #2 / lsl r1, #16 / lsl r2, #15
 *
 * This is the two-shifted-constants case that gcc DOES produce elsewhere --
 * probe q8 in the session notes emitted exactly that pattern from
 * f3(0xe, 0x102, 0x204). Here it does not, and the plain `mov r0, #2` in the
 * middle is the difference: gcc groups the two shift pairs and puts the
 * unrelated move outside them.
 *
 * So this is the arg-interleave class with a third argument involved, and
 * worth retrying if that class ever falls -- it is not a separate problem.
 *
 * RE-TESTED after the arg-fill-order fix (2026-08-03). Four formulations, all
 * producing the identical diff above:
 *
 *   1. Full prototypes on every callee, with return types. That is what fixed
 *      arg-fill-order -- an implicitly declared callee returns int, so gcc
 *      keeps r0 live across the call and fills the next call's r0 last. It
 *      does not move r0 here.
 *   2. The reverse: __Func_8012078 implicitly declared.
 *   3. The shifted values built into named locals before the call, so the
 *      shifts are statements rather than argument expressions.
 *   4. A narrower first parameter (unsigned char), so r0's argument needs a
 *      conversion the others do not.
 *
 * So r0 placement here is NOT driven by declaration state, which is what
 * separates arg-interleave from arg-fill-order. They looked like one class.
 *
 * The filter still does not catch this one: tools/elevation_candidates.py
 * looks two lines back from an `lsl` for the `mov` that starts it, and here
 * the pair is split by three. Same deliberate gap as OvlFunc_933_2009874 --
 * but that is now two functions it has cost.
 */
extern void __Func_8012078(int a, int b, int c, int d);
extern int  OvlFunc_891_2009be8(int a, int b, int c);
extern void OvlFunc_891_200a244(void);

void OvlFunc_891_20095d4(void)
{
    __Func_8012078(2, 0xd0 << 16, 0xe0 << 15, 0);
    if (OvlFunc_891_2009be8(0xa, 0xe, 7))
        OvlFunc_891_200a244();
}
