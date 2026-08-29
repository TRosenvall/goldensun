/* Func_8079bf8  --  0x08079bf8, asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a.s
 *
 * BLOCKER CLASS: argument setup order plus a two-operand subtract.
 * Status: 27 lines against 27. The whole tail -- the multiply, the signed
 * divide and both clamps -- is exact; the first five instructions are shuffled.
 *
 *      rom    mov r5, r0 / sub r5, r1 / mov r6, r2 / mov r0, r3 / cmp r5, #0
 *      ours   sub r5, r0, r1 / mov r6, r2 / cmp r5, #0 ... mov r0, r3 (after)
 *
 * TWO SEPARATE THINGS, and neither is reachable:
 *
 *   The ROM copies then subtracts in place; gcc uses the three-operand form.
 *   `x = a; x -= b;` -- the usual way to ask for that -- is byte-identical,
 *   because gcc folds the copy and the subtract back together.
 *
 *   The ROM moves argument 3 into r0 for the upcoming call BEFORE the clamp
 *   test; gcc does it after. Nothing in C orders an argument load against an
 *   unrelated comparison.
 *
 * SETTLED AND WORTH KEEPING. `r / 512` on a signed int produces the ROM's
 * `cmp #0 / bge / add #0x1ff / asr #9` exactly -- the round-toward-zero
 * correction. An arithmetic shift would be one instruction and is not what the
 * ROM has. And the multiply is `r * (x + c * 2)` with r first, because Thumb's
 * two-operand `mul` puts the result in the first operand's register.
 */

extern int Func_8079b24(int a, int b);

int Func_8079bf8(int a, int b, int c, int d)
{
    int x;
    int r;

    x = a - b;
    if (x < 0)
        x = 0;
    r = Func_8079b24(d, 1);
    r = r * (x + c * 2);
    r = r / 512;
    if (r < 0)
        r = 0;
    return r;
}
