/* Cluster Func_801a7c0..Func_801a7c0 extracted from goldensun/asm/rom_15000/rom_1a66c_a.s.
 *
 * Total .text for this TU = 52 bytes (= 0x34).
 * Slotted between the _a and _c pieces in goldensun/stage1.ld.
 *
 * PushScreenEntry -- appends one (a, b) pair to two parallel sixteen-entry
 * short arrays in the party/status block, with the shared count at +0x394.
 * A full array silently drops the value.
 *
 * WHAT MADE THIS MATCH WAS DESCRIBING THE LAYOUT, NOT THE ADDRESSES. Written as
 * raw offsets --
 *
 *      *(unsigned short *)(blk + (0x354 + n)) = a;
 *
 * -- gcc reassociates to `(blk + n) + 0x354`, folds the index into the base and
 * emits `strh r0, [r3]` off a rewritten pointer, needing an extra callee-saved
 * register (25 lines, r5 pushed). Declared as two `short [16]` members at
 * their real offsets, gcc keeps the block pointer as the base, computes `n * 2`
 * once and adds each member's constant to it -- which is the ROM's
 * `mov r12, r3` plus register-offset `strh r0, [r2, r3]`, and pushes only lr.
 *
 * The trailing `p->n = p->n + 1` reloads the count rather than reusing the
 * value in hand, and that is correct rather than sloppy: the two `short` stores
 * may alias the `unsigned short` count, so gcc-2.96 has to re-read it. The ROM
 * re-reads it too.
 *
 * NOTE ON r4: this function writes r4 without saving it, and so does our
 * output -- it shows up identically in the two accessors in rom_11ce0_c_a_b.c.
 * The reason is -fcall-used-r4 in GCC296_CFLAGS, not a disassembly artifact and
 * not something innate to gcc-2.96.
 */

struct Blk {
    unsigned char pad[0x354];
    short xs[16];
    short ys[16];
    unsigned short n;
};

extern struct Blk *iwram_3001e98;

void Func_801a7c0(int a, int b)
{
    struct Blk *p;
    int n;

    p = iwram_3001e98;
    n = p->n;
    if (n != 0x10) {
        p->xs[n] = a;
        p->ys[n] = b;
        p->n = p->n + 1;
    }
}
