/* OvlFunc_945_20080fc  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7cb2c0/ovl_30_a_c_c_a.s
 * Best screen: 19 instructions in disagreeing regions, of 28 (rom 28, ours 27).
 *
 * BLOCKER CLASS: basic-block placement.
 *
 * The ROM puts the `return 1` for the counter-expired case BETWEEN the
 * decrement and the rest of the function:
 *
 *      cmp r3, #0 / beq .L10a
 *      sub r3, #1 / str r3, [r0, #0x4c] / b .L10e
 *      .L10a: mov r0, #1 / b .L128
 *      .L10e: <the three comparisons>
 *
 * gcc sinks that block to the end, past the comparisons, and every label after
 * the first shifts.
 *
 * WHAT WAS TRIED
 *   1. A plain early `return 1;` (19 of 28).
 *   2. The layout written out explicitly with gotos --
 *      `if (v == 0) goto one; ... goto rest; one: return 1; rest: ...` --
 *      which is the ROM's block order stated in the source. BYTE-IDENTICAL.
 *
 * (2) is the informative one. Block reordering happens after the source has had
 * its say, so naming the order in C does not reach it. This is distinct from
 * the branch-SENSE levers (arm inversion, do/while back edge), which do work,
 * because those change which test gcc emits rather than where it puts the
 * resulting blocks.
 *
 * The comparison chain itself is right: each test compares against the value
 * loaded by the PREVIOUS test (`x != w`, then `y != x`), which is what gives
 * the ROM's `cmp r2, r3` and `cmp r3, r2` rather than three compares against
 * the constant.
 */
int OvlFunc_945_20080fc(unsigned char *a)
{
    int v;
    int k;
    int w;
    int x;
    int y;

    v = *(int *)(a + 0x4c);
    if (v == 0)
        return 1;
    v = v - 1;
    *(int *)(a + 0x4c) = v;
    k = 0x80 << 24;
    w = *(int *)(a + 0x38);
    if (w != k)
        return 0;
    x = *(int *)(a + 0x3c);
    if (x != w)
        return 0;
    y = *(int *)(a + 0x40);
    if (y != x)
        return 0;
    return 1;
}
