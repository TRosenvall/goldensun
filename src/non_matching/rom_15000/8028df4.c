/* YesNoMenu -- NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_15000/rom_23178_a_a_c_c_a.s
 * Best screen: 43 instructions against the ROM's 45.
 *
 * BLOCKER CLASS: which parameter lands in which callee-saved register.
 *
 * The logic screens as correct -- the two guard rewrites, the two menu-bar
 * options, and the `-1` normalisation at the end all appear in the right
 * places. What differs is the prologue's assignment of four parameters to four
 * high registers, and every later instruction that names one of them:
 *
 *     rom    r5 = c, r6 = d, r7 = a, r10 = b, r8 = 0
 *     ours   r6 = a, r8 = b, r10 = d, r7 = 0    (and c stays in r5)
 *
 * Two instructions short, because the ROM materialises the zero into r3 and
 * copies it to r8, then reuses r3 for 0x11 and copies again; we build each
 * where it is needed.
 *
 * TRIED: naming the fourth parameter into a local immediately after the third,
 * so the source's order of first use matches the ROM's register order.
 * Byte-identical at 39 differing.
 *
 * This is the same kind of thing as the r2/r3 exchange class in
 * docs/elevation.md but on the high registers, where there is no operand-order
 * or type lever to reach for -- the parameters arrive where they arrive. The
 * one thing NOT yet tried is reordering the parameter list itself, which would
 * change the function's ABI and cannot be right.
 */
extern void Func_80284dc(void);
extern void AddMenuBarOption(int n);
extern void Func_8028808(int a, int b, int c);
extern int Func_8028574(int a);
extern void Func_802851c(void);

int YesNoMenu(int a, int b, int c, int d)
{
    int x;
    int y;
    int res;

    x = c;
    y = 0;
    Func_80284dc();
    if (x == 0)
        x = 3;
    if (a != 0)
        y = 0x11;
    AddMenuBarOption(5);
    AddMenuBarOption(6);
    Func_8028808(y, x, b);
    res = Func_8028574(d);
    Func_802851c();
    if (res == -1)
        res = 1;
    return res;
}
