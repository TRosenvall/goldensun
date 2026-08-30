/*
 * Func_80a3ce4 (IsSpecialItemId) -- asm/rom_a1000/rom_a1814_c_a_c_c_c_c_a_c_b.s
 * SPLIT OUT this round; byte-neutral, verified.
 *
 * BLOCKER: gcc folds the range test; the ROM does not. 11 lines against 12.
 *
 *      rom   cmp r0, #0xc4 / bgt / cmp r0, #0xc1 / blt
 *      ours  sub r0, #0xc1 / cmp r0, #0x3 / bhi
 *
 * gcc-2.96 rewrites `0xc1 <= id && id <= 0xc4` into an unsigned range check --
 * subtract the low bound, compare against the width. The ROM keeps two signed
 * comparisons.
 *
 * TRIED AND REJECTED, measured:
 *   `id <= 0xc4 && id >= 0xc1` in one condition      -- 11 lines, 11 differing
 *   two separate `if (...) return 0;` statements     -- 13 lines, 10 differing
 *   nested ifs writing a result variable             -- 11 lines, 10 differing
 *
 * None prevents the fold. Note the ROM's `cmp #0xc1 / blt` is the same family as
 * the `cmp #K / bge` shape batch 145 corrected, where naming the bound in a
 * local was the fix -- but here the problem is one step earlier: the two
 * comparisons never survive as two comparisons.
 */
int Func_80a3ce4(int id)
{
    int r;

    r = 0;
    if (id <= 0xc4) {
        if (id >= 0xc1)
            r = 1;
    }
    return r;
}
