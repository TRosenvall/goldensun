/* Func_80a4db4  --  0x080a4db4
 *
 * Cut out of goldensun/asm/rom_a1000/rom_a47b4_a_c.s.
 *
 * Draws a signed number and then its sign glyph, right-aligning the pair: the
 * glyph's x is pulled left by eight pixels per digit, so the digit count is
 * worked out from the magnitude first.
 *
 * THE MAGNITUDE IS COMPUTED TWICE, NOT STORED. The ROM has the
 * `mov r3, r5 / cmp r5, #0 / bge / neg r3, r5` sequence in full, twice, once
 * before each threshold test. A local holding the absolute value would compute
 * it once; writing the conditional expression at both sites is what produces
 * the duplicate.
 *
 * THE POSITION IS COMPUTED INSIDE EACH ARM. Both branches end in the same call
 * with the same three trailing arguments and differ only in the glyph, so the
 * obvious spelling hoists the arithmetic above the `if`. The ROM repeats
 * `lsl r2, #3 / sub r2, r6, r2 / add r2, #0x10` in both arms, which is what
 * writing the whole call twice gives -- gcc does not cross-jump these because
 * the differing argument is the FIRST one, not the last.
 *
 * Matched on the first screen.
 */
extern unsigned char Laf224[] __asm__(".Laf224");
extern unsigned char Laf228[] __asm__(".Laf228");
extern void _Func_801ea08(int val, int base, int y, int w, int e);
extern void _UIDrawText(unsigned char *s, int y, int x, int e);

void Func_80a4db4(int val, int a, int y, int w, int e)
{
    int d;

    _Func_801ea08(val, 3, y, w, e);
    d = 1;
    if ((val < 0 ? -val : val) > 9)
        d = 2;
    if ((val < 0 ? -val : val) > 0x63)
        d = 3;
    if (val > 0)
        _UIDrawText(Laf224, y, w - (d << 3) + 0x10, e);
    else
        _UIDrawText(Laf228, y, w - (d << 3) + 0x10, e);
}
