/* OvlFunc_965_20089dc
 *
 * Source asm: goldensun/asm/overlays/rom_7ef4f4/ovl_30_a_a_a_c_c_c_c.s
 *
 * NOT SPLIT.
 *
 * Writes a two-bit field into bits 2-3 of the byte at sprite offset 9.
 *
 * IMPROVED FROM 9 DIFFERING POSITIONS TO 4, and the instruction count now
 * matches at 11 against 11. Three separate rules applied, all of them already
 * in the tree and none of them applied here before:
 *
 *  1. NARROW_CONSTANT. The ROM synthesises ~0xc as `mov r3, #0xd / neg r3, r3`
 *     -- a full 32-bit -13 -- where a literal `& ~0xc` on a byte value narrows
 *     to `mov r3, #0xf3`. A NAMED `int` mask fixes it. This is the sub-case
 *     that HAS a lever: a narrow value in a wide expression. (The sub-case that
 *     does not is a narrow expression end to end -- see
 *     src/non_matching/rom_9000/rom_c5b4.c.) The park's own TODO proposed this
 *     and it was never tried.
 *
 *  2. THE FIELD READ NEEDS ITS OWN `int` LOCAL, for the same reason -- read
 *     directly out of `q[9]` into the expression, gcc keeps it byte-wide.
 *
 *  3. OPERAND ORDER OF THE `&` DECIDES THE DESTINATION REGISTER. `(m & v)`
 *     gives the ROM's `and r3, r2` (result in the mask's register); `(v & m)`
 *     gives `and r2, r3`. Same fact as the addition in HeightTile_7 -- gcc
 *     writes the result into whichever operand it evaluated first.
 *
 * WHAT REMAINS is scheduling: the ROM interleaves the `ldrb` of the field
 * between building the 3-mask and applying it, and gcc emits the load either
 * before or after the whole `val & 3` sequence. Moving the read earlier in the
 * source overshoots -- gcc then merges it and the function comes out 10
 * instructions instead of 11.
 *
 * THIS IS A FIVE-MEMBER FAMILY: OvlFunc_common0_0 and OvlFunc_{927,946,964,965}
 * _20089dc are clones. Whatever closes the last four positions closes all five.
 */
void OvlFunc_965_20089dc(unsigned char *p, int val)
{
    unsigned char *q = *(unsigned char **)((char *)p + 0x50);
    int v;
    int m;
    int t;

    t = val & 3;
    v = q[9];
    m = ~0xc;
    t <<= 2;
    q[9] = (m & v) | t;
}
