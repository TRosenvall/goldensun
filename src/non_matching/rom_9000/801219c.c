/* Func_801219c (IsPositionOnMap) -- NON-MATCHING.
 * Blocker class: REGISTER CHOICE for a value held across a branch.
 * 54 lines against the ROM's 53, 17 differing.
 *
 * The ROM keeps the divided x in r4 across the null-map test; gcc puts it in
 * r1 and the choice propagates through everything after. Note r4 is FREE here
 * without saving -- GCC296_CFLAGS carries -fcall-used-r4 -- so both compilers
 * could use it and only one does.
 *
 * THREE IDIOMS WERE SOLVED and the first 28 lines are exact because of them:
 *
 *   1. SIGNED DIVIDE, not a shift. `cmp / bge / add #0xffff / asr #16` is
 *      gcc's expansion of `/ 0x10000` on a signed int, and `cmp / bge /
 *      add #0xf / asr #4` is `/ 16`. Writing the shifts gives neither bias.
 *      This function uses both, four times between them.
 *   2. THE BRANCHLESS TAIL. `eor #0xff / neg / orr / lsr #31 / sub #1` is
 *      `(t != 0) - 1` where `t = b ^ 0xff` -- zero when the tile byte is 0xFF
 *      and -1 otherwise, with no compare. Written as a conditional it
 *      branches instead.
 *   3. OPERAND ORDER IN THE INDEX. `x / 16 + (z / 16) * 128` emits the two
 *      divides in the ROM's order; `(z / 16) * 128 + x / 16` reverses them,
 *      which was worth 23 differing down to 17.
 *
 * What remains is which register holds x. No source form selects that, and it
 * is the register-pressure category HANDOFF.md already describes.
 */
extern char *iwram_3001e70;

int Func_801219c(int *pos)
{
    int x, z;
    char *m;
    unsigned char *layer;
    int t;

    x = pos[0] / 0x10000;
    z = (pos[2] - pos[1]) / 0x10000;
    m = iwram_3001e70;
    if (m == 0)
        return 0;
    layer = *(unsigned char **)(m + (0xc8 << 1));
    t = layer[(x / 16 + (z / 16) * 128) * 4 + 2] ^ 0xff;
    return (t != 0) - 1;
}
