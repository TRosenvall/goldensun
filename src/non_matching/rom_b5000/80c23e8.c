/* Func_80c23e8  --  0x080c23e8, asm/rom_b5000/rom_c1a34_a_a_c_c_a_a.s
 *
 * BLOCKER CLASS: gcc merges two `return 1` paths that the ROM keeps separate.
 * Status: 16 lines against the ROM's 19 -- gcc is THREE instructions ahead.
 *
 * WHAT IT DOES
 * Looks a four-bit field out of an 8-byte table record and returns it, or 1
 * both when the index is out of range and when the field is zero.
 *
 * THE BITFIELD READ IS RIGHT AND SHOULD BE KEPT. The ROM's
 * `lsl r3, #27 / lsr r3, #28` is exactly what gcc emits for an unsigned 4-bit
 * bitfield at bit 1, and declaring the record as
 * `unsigned char b0 : 1, mid : 4, hi : 3` reproduces it with no shifting
 * written by hand.
 *
 * THE DIFFERENCE IS BLOCK STRUCTURE. The ROM has TWO separate `mov r0, #1`
 * blocks -- one for the range failure, one for the zero field -- and jumps to
 * the epilogue from each. gcc notices they return the same value and merges
 * them into one, saving a `mov` and a `b`.
 *
 * WHAT WAS TRIED
 *   - `if (id > 0xab) return 1;` then the body, which is the ROM's own reading:
 *     gcc still merges and inverts the branch to `bhi`
 *   - the same with a `goto` to a single trailing `return 1`, the lever that
 *     fixed OvlFunc_937_20080e4: byte-identical -- because it asks for exactly
 *     the merge gcc is already doing
 *
 * The second attempt is the informative one. Guard inversion moves a block; it
 * cannot UN-merge two blocks that compute the same value, and no spelling of
 * "return 1 twice" survives, because the two returns are indistinguishable in
 * the source. This is the optimiser-proved class, not a placement one.
 */

struct E {
    unsigned char pad[2];
    unsigned char b0 : 1;
    unsigned char mid : 4;
    unsigned char hi : 3;
    unsigned char pad3[5];
};

extern struct E Lc7420[] __asm__(".Lc7420");

int Func_80c23e8(unsigned int id)
{
    int v;

    if (id > 0xab)
        return 1;
    v = Lc7420[id].mid;
    if (v != 0)
        return v;
    return 1;
}
