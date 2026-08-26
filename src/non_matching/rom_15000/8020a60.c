/* Func_8020a60  --  0x08020a60, asm/rom_15000/rom_20198_c_a.s
 * and its byte-identical twin Func_80a2268  --  0x080a2268,
 * asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a.s
 *
 * Source asm: goldensun/asm/rom_15000/rom_20198_c_a.s
 *
 * BLOCKER CLASS: two-operand accumulate where the ROM keeps three operands.
 * Status: 72 lines against 72, SIXTY-FIVE identical, and the seven that differ
 * come from two commutative-add decisions.
 *
 * Clips a rectangle to the 0x1e x 0x14 tilemap and then flips bit 12 of every
 * halfword inside it, one row at a time, finally setting the dirty flag at
 * +0xea3. The clamp constants really are asymmetric -- both tests compare
 * against 0x1d while the corrections are `0x1e - x1` and `0x14 - y1` -- and
 * that is transcribed as found, not tidied.
 *
 * WHAT WAS SOLVED, because it is reusable. The inner loop first came out five
 * instructions longer than the ROM's:
 *
 *     rom    ldrh r3, [r4] / and r3, r2 / orr r3, r7 / strh r3, [r4]
 *     ours   ldrh r2, [r4] / mov r3, r6 / and r3, r2 / mov r2, r12 /
 *            orr r3, r2 / strh r3, [r4]
 *
 * `*p = (*p & ~0x1000) | flip;` narrows the whole expression to HImode, so the
 * mask is pooled as `0xefff` where the ROM has `0xffffefff`, and the extra
 * width forced the mask and the attribute into high registers that need a `mov`
 * apiece inside the loop. Loading through a named `unsigned int` and writing
 * the three steps as separate statements keeps it SImode and the loop collapses
 * to the ROM's four instructions. Same lesson as batch 79's Func_800c5b4: it is
 * the OPERATION that narrows, and an intermediate of the right width is what
 * stops it.
 *
 * WHAT IS LEFT, and it is one class seen twice:
 *
 *     rom    add r3, r1, r3 / add r1, r3, #1      three-operand, x survives
 *     ours   add r1, r3     / add r1, #1          two-operand, accumulates
 *
 *     rom    add r4, r1, r3     (offset + base)
 *     ours   add r4, r3, r1     (base + offset)
 *
 * The first accounts for six of the seven -- the two-operand form shifts the
 * whole block by one slot. gcc coalesces the intermediate `x + b->w` with `x1`
 * because the intermediate dies immediately; the ROM's compiler gave them
 * separate registers. The second is gcc canonicalising a commutative PLUS.
 *
 * TRIED AND MEASURED, all 72 lines against 72:
 *
 *   x + b->w + 1                                  7
 *   x + 1 + b->w                                  7
 *   1 + (x + b->w)                                7
 *   x + (b->w + 1)                                7
 *   b->w + x + 1                                 21   worse
 *   one named temp reused for both axes          19   worse
 *   two named temps, assigned in ROM order        7
 *   `off + (char *)base` instead of base + off    7   (gcc canonicalises)
 *   `int v` rather than `unsigned int v`         13   worse
 *   the mask written 0xffffefff inline           76   worse (still narrows)
 *
 * Flags: -fno-gcse, -fno-rerun-cse-after-loop, -fno-strict-aliasing,
 * -fno-schedule-insns and -fno-strength-reduce all leave it at 7;
 * -fno-schedule-insns2 gives 17 and -O1 gives 31.
 *
 * NOTHING IN THE SOURCE SEPARATES THE INTERMEDIATE FROM x1 -- it would have to
 * be live where x1 is not, and it is used exactly once. This belongs with the
 * register-allocation class rather than with anything spellable. Two functions
 * come with it.
 */

struct Box {
    unsigned char pad00[0xc];
    unsigned short w;
    unsigned short h;
};

extern unsigned char iwram_3001e8c[];

void Func_8020a60(struct Box *b, int x, int y, int w, int h, int flip)
{
    unsigned short *base;
    unsigned short *p;
    int x1, y1, off, n;

    base = *(unsigned short **)iwram_3001e8c;
    x1 = x + b->w + 1;
    y1 = y + b->h + 1;
    flip <<= 12;
    if (x1 < 0) {
        w += x1;
        x1 = 0;
    }
    if (x1 + w > 0x1d)
        w = 0x1e - x1;
    if (y1 < 0) {
        h += y1;
        y1 = 0;
    }
    if (y1 + h > 0x1d)
        h = 0x14 - y1;
    if (w > 0 && h > 0) {
        off = (y1 << 6) + (x1 << 1);
        do {
            p = (unsigned short *)((char *)base + off);
            n = w;
            while (n != 0) {
                unsigned int v = *p;
                v &= ~0x1000;
                v |= flip;
                *p = v;
                n--;
                p++;
            }
            h--;
            off += 0x40;
        } while (h != 0);
        *((char *)base + 0xea3) = 1;
    }
}
