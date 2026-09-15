/* Func_8079c5c -- 0x08079c5c, split out of
 * asm/rom_77000/rom_79460_c_c_c_c_a_c_c_a_a.s.
 *
 * Scales a value by a table lookup and a multiplier, then divides by 65536.
 *
 * THE DIVISION IS A PLAIN `/ 0x10000` ON A SIGNED int. The ROM's
 * `cmp r0, #0 / bge / ldr r3, =0xffff / add r0, r3 / asr r0, #16` is gcc's
 * expansion of a signed power-of-two divide -- bias by 2^n - 1 when negative,
 * then arithmetic shift. Reading it as a shift, or as a conditional in the
 * source, would both be wrong: there is nothing to write but the division.
 *
 * THE RESIDUE WAS MULTIPLY OPERAND ORDER, and it is worth two lines because the
 * fix is not where it looks. The call's result must be the LEFT operand:
 *
 *     rom    mov r3, r6 / mul r3, r0        (a moved first, result multiplied in)
 *     ours   mov r3, r0 / mul r3, r3, r6    (result moved first)
 *
 * `a * r * c` gives the second; `r * a * c` gives the ROM's. gcc evaluates the
 * call first regardless -- `r` is already in r0 -- so which operand it COPIES
 * is decided by the source's operand order, not by evaluation order.
 *
 * Folding the multiplications into statements is WORSE and in a way worth
 * recording: `r *= a; return c * r / 0x10000;` and `r = a * r; return r * c
 * / 0x10000;` both come out 44 bytes against 48 -- two instructions SHORT --
 * because gcc then commutes the second multiply into the first's register and
 * drops a copy. The single expression is what keeps both copies.
 *
 * EXACT: 48 bytes, 22 encodings, 1 relocation, measured three times, clean on
 * tools/tryc.py.
 */
extern int Func_8079b24(int a, int b);

int Func_8079c5c(int a, int b, int c)
{
    int r;

    r = Func_8079b24(b * 2 - 0xc8, 0);
    return r * a * c / 0x10000;
}
