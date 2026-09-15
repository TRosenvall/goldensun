/* Func_80a3ce4 @ 0x080a3ce4  --  IsSpecialItemId
 *
 * Returns 1 for 0xc1..0xc4 inclusive, 0 otherwise.  No pins, no flags.
 *
 * THE RANGE TEST COMES FROM A switch, NOT FROM A COMPARISON.
 *
 * The ROM's second test is `cmp r0, #0xc1 / blt`.  NO spelling of the condition
 * reaches that -- every one of them canonicalises to `cmp r0, #0xc0 / ble`:
 *
 *     if (id < 0xc1)        if (id <= 0xc0)       if (0xc1 > id)
 *     if (!(id >= 0xc1))    if (id >= 0xc1) ...   all -> cmp #0xc0 / ble
 *
 * With the goto form the structure was otherwise instruction-identical -- the
 * push, both branches, both movs, the join -- and only that one `cmp`/branch
 * pair differed, 2 of 10.  Writing the range as a switch is EXACT:
 *
 *     switch (id) { case 0xc1: case 0xc2: case 0xc3: case 0xc4: return 1; }
 *     return 0;
 *
 * expand_switch emits the bounds check itself rather than folding a user
 * comparison, so the low bound survives as 0xc1 with a `blt`.  The `||` form is
 * 9 of 10 and the two-arm if/else forms are 2, 5 or 6 depending on polarity.
 *
 *   A `cmp #C / blt` that resists every </<= spelling of the same bound is the
 *   tell for a switch range test.  Four consecutive cases cost nothing extra --
 *   gcc emits the same two compares, not a table.
 */
int Func_80a3ce4(int id)
{
    switch (id) {
    case 0xc1:
    case 0xc2:
    case 0xc3:
    case 0xc4:
        return 1;
    }
    return 0;
}
