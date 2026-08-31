/* OvlFunc_938_200940c -- 0x0200940c  (asm/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_c_c.s)
 *
 * BLOCKER: argument interleave at STRAIGHT-LINE sites. 6 of 30, exact length,
 * and the six are three identical two-line transpositions.
 *
 *     rom   mov r1, #0xe0 / mov r0, #0x1 / lsl r1, #0x8
 *     ours  mov r1, #0xe0 / lsl r1, #0x8 / mov r0, #0x1
 *
 * Four calls to __Func_8092adc with a shifted constant each; the function's
 * only conditional branch is AFTER all four, so the entry block does not
 * dominate anything and the naming lever has no guard to cross.
 *
 * MEASURED: naming the four shifted constants in the entry block is 37
 * differing and EIGHT lines long -- the documented failure for straight-line
 * functions, where the named values stay live instead of being rematerialised.
 * Naming the three slot arguments instead is inert at 6.
 *
 * THE DETAIL WORTH KEEPING: the FIRST of the four calls matches exactly, and it
 * is the only one with anything pending to put in the gap --
 *
 *     mov r1, #0xc0 / mov r5, r0 / lsl r1, #0x7
 *
 * where `mov r5, r0` is the PARAMETER SAVE, not an argument at all. So the gap
 * in a split constant build is filled by whatever single-instruction work gcc
 * happens to have outstanding, and an argument is only the usual candidate.
 * That explains why a run of near-identical call sites can match at one and
 * fail at the rest, which otherwise looks like noise: the first site had a
 * spare instruction and the others did not.
 *
 * It also says what would close this: something the source genuinely computes
 * between the calls. There is nothing, and inventing one would change the
 * program.
 */
extern void __Func_8092adc(int a, int b, int c);
extern void __CutsceneWait(int n);

void OvlFunc_938_200940c(int t)
{
    __Func_8092adc(0, 0xc0 << 7, 0);
    __Func_8092adc(1, 0xe0 << 8, 0);
    __Func_8092adc(2, 0x80 << 6, 0);
    __Func_8092adc(3, 0xa0 << 8, 0);
    if (t != 0)
        __CutsceneWait(t);
}
