/* Func_8078aa0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_77000/rom_78a8c_c_a.s
 * Best screen: 30 instructions in disagreeing regions, of 25 (rom 25, ours 28).
 *
 * BLOCKER CLASS: guard inversion plus three instructions of extra control flow.
 *
 * The ROM's bounds guard branches OUT and falls through into the body:
 *
 *      cmp r2, #0x7f / bgt .Lexit
 *
 * gcc inverts it, branching INTO the body and falling through to the early
 * return, which costs an extra `b`. Written the other way round -- the body in
 * the `if` and the return in the `else` -- gcc inverts that too; the
 * arm-inversion lever does not reach a guard whose arms are a whole function
 * body against a single return.
 *
 * WHAT WAS TRIED
 *   1. `if (i > 0x7f) return r;` with the clamp chain following (33 of 25).
 *   2. The table pointer hoisted above the guard, matching the ROM's
 *      `ldr r4, =0x2000380` before the compare. 30 of 25 -- a genuine
 *      improvement of three, and kept.
 *
 * The clamp chain itself is right: `if (v < 0) ... else if (v > 0x63) ... else`
 * reproduces the ROM's two compares and three arms, including the arm that sets
 * BOTH the stored value and the return value to 0x63.
 *
 * The residue is that our stream is three longer, so this is not a naming or
 * spelling question -- gcc is laying the guard out differently and every label
 * after it shifts.
 */
extern unsigned char ewram_2000380[];

int Func_8078aa0(int idx, int d)
{
    unsigned char *tb;
    int i;
    int v;
    int r;

    i = idx;
    tb = ewram_2000380;
    r = 0;
    if (i > 0x7f)
        return r;
    v = tb[i];
    v = v + d;
    if (v < 0) {
        v = 0;
    } else if (v > 0x63) {
        v = 0x63;
        r = 0x63;
    } else {
        r = v;
    }
    tb[i] = v;
    return r;
}
