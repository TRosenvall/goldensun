/* Cluster HeightTile_3..HeightTile_3 extracted from
 * goldensun/asm/rom_9000/rom_11ce0_a_c_c_a_a_a.s.
 *
 * Total .text for this TU = 48 bytes (= 0x30).
 * Placed in the run in goldensun/stage1.ld.
 *
 * One of the height-tile corner resolvers. Reads the two signed corner heights
 * out of a tile record, scales both by 1 << 19, and returns the higher of the
 * two when the tile shape is 0xf, the second corner when the shape is above
 * 0xe, and the first otherwise.
 *
 * THE SHAPE INDEX IS UNSIGNED, and that was the whole difference -- one
 * instruction out of twenty-four:
 *
 *      rom    cmp r1, #0xe / bhi
 *      ours   cmp r1, #0xe / bgt
 *
 * `int i` gives the signed `bgt`. This is the second function in two rounds to
 * come down to exactly this (see OvlFunc_911_20080a0's loop bound), so it is
 * worth checking the branch SUFFIX before diagnosing anything else. The rule is
 * in docs/elevation.md.
 *
 * The corner reads use an index register -- `mov r3, #0 / ldrsb r3, [r0, r3]`
 * -- because Thumb `ldrsb`, like `ldrsh`, has no immediate-offset form. That
 * falls out of ordinary array indexing and needed no help.
 */

int HeightTile_3(signed char *p, int x, int y)
{
    int a;
    int b;
    int m;
    unsigned int i;

    a = p[0] << 19;
    b = p[1] << 19;
    m = a;
    if (b > a)
        m = b;
    i = x + y;
    if (i == 0xf)
        return m;
    if (i > 0xe)
        return b;
    return a;
}
