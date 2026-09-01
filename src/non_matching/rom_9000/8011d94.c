/* HeightTile_5 -- 0x08011d94  (asm/rom_9000/rom_11ce0_a_c_c_a_a_a_b.s)
 *
 * BLOCKER: register assignment and scheduling in the three-read prologue.
 * 29 of 36, exact length. docs/elevation.md already records this family as
 * "three-for-three on register assignment with interpolation, divisions and
 * branches all exact", and this member behaves the same way.
 *
 * The arithmetic and control flow reproduce: three signed-byte reads shifted
 * left 19, the `a + b` index, the `== 0xf` early exit, the unsigned `> 0xe`
 * split, and both interpolations with their `__divsi3` by 15. What differs is
 * the ORDER of the reads against their shifts --
 *
 *     rom   ldrsb / add r0,#1 / lsl r6 / ldrsb / lsl r5 / ldrsb / add r1,r2 / lsl r3
 *     ours  ldrsb / add r0,#1 / ldrsb  / lsl r6 / ldrsb / add / lsl / lsl
 *
 * -- and which register each height lands in, which follows from it.
 *
 * MEASURED: merging the index into the `a` parameter, which the ROM's
 * destructive `add r1, r2` suggests, gives 23 differing but grows the function
 * to 38 lines. Fewer differences and a worse structure, so the version kept
 * below is the exact-length one. That trade is worth noting -- the merge lever
 * has been reliable and here it buys count at the cost of length, which for a
 * function this short is the wrong direction.
 */
int HeightTile_5(signed char *p, int a, int b)
{
    int h0;
    int h1;
    int h2;
    int t;
    int r;

    h0 = p[(unsigned int)0] << 19;
    p += 1;
    h1 = p[(unsigned int)0] << 19;
    h2 = p[1] << 19;
    t = a + b;
    r = h1;
    if (t == 0xf)
        goto out;
    if ((unsigned int)t > 0xe)
        goto high;
    r = h0 + t * (h1 - h0) / 0xf;
    goto out;
high:
    r = h1 + (t - 0xf) * (h2 - h1) / 0xf;
out:
    return r;
}
