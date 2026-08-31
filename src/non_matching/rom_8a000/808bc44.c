/* Func_808bc44 -- asm/rom_8a000/rom_8ba38_a_a_a.s
 *
 * BLOCKER: a TWO-REGISTER SWAP, repeated twelve times. 38 of 41, LENGTH EXACT,
 * and the instruction sequence is otherwise identical line for line.
 *
 * A leaf function -- no prologue, no epilogue, just `bx lr` -- that zeroes
 * twelve consecutive halfwords starting at offset 0x16c in the block at
 * iwram_3001ebc.
 *
 * THREE LEVERS WERE NEEDED TO GET THE SHAPE, and they are the value here:
 *
 *   1. UNROLL IT IN THE SOURCE.        loop form: 17 lines against 41.
 *      gcc-2.96 does not unroll at -O2, so a `for` loop cannot produce this.
 *      Twelve explicit statements are required.
 *
 *   2. NAME THE ADDRESS, not just the offset.        31 -> 41 lines.
 *      Written as `*(unsigned short *)(p + off) = 0`, gcc emits a
 *      register-offset store `strh r2, [r1, r3]` -- ONE instruction, ten
 *      shorter overall. The ROM computes the address into a register first
 *      (`add r3, r1, r0 / strh r2, [r3]`). Assigning the address to its own
 *      pointer local reproduces that.
 *
 *      Note this is the INVERSE of the lever that closed Func_8011b00 and
 *      Sprite_DeleteLayer, where naming the OFFSET restored register-offset
 *      addressing. Both directions exist and the ROM decides which is wanted:
 *      name the offset to get `[base, index]`, name the address to get a
 *      separate `add`.
 *
 *   3. THE ZERO THROUGH AN INT LOCAL. Otherwise `ldr r2, =0x0` -- a pooled
 *      constant -- because a bare literal into a halfword store is HImode.
 *      Same operand-mode rule that closed Func_8011b00.
 *
 * WHAT REMAINS is which register holds which value:
 *
 *     rom    offset -> r0    address -> r3
 *     ours   offset -> r3    address -> r0
 *
 * repeated at all twelve stores, which is the entire count. FIVE orderings
 * measured, none flips it:
 *
 *   baseline (p, off, zero)                     41 lines, 38 differ
 *   `off` assigned before `p`                   41 lines, 40 differ
 *   `zero` assigned first                       41 lines, 39 differ
 *   `q` declared before `off`                   41 lines, 38 differ
 *   `off` declared first of all locals          41 lines, 38 differ
 *
 * The assignment-position lever decided exactly this kind of r0/r3 pair on
 * Func_80d66cc. It does not reach it here, and the three byte-identical
 * results say the source cannot express the difference.
 */
extern int iwram_3001ebc;

void Func_808bc44(void)
{
    char *p;
    unsigned short *q;
    int off;
    int zero;

    p = (char *)iwram_3001ebc;
    off = 0xb6 * 2;
    zero = 0;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    off += 2;
    *q = zero;
    q = (unsigned short *)(p + off);
    *q = zero;
}
