/* Cluster OvlFunc_927_20089dc..OvlFunc_927_20089dc extracted from goldensun/asm/overlays/rom_7b4558/ovl_30_a_a_c_c_c_c_a.s.
 *
 * Total .text for this TU = 22 bytes (= 0x16).
 * Appended after the _a piece in goldensun/overlays/rom_7b4558/overlay.ld.
 *
 * Writes a two-bit selector into bits 2-3 of the sprite flag byte at +9 of the
 * sprite record hanging off the actor at +0x50. FIVE BYTE-IDENTICAL COPIES of
 * this function exist -- one in overlays/common and four in per-area overlays;
 * they are listed in reports/batch-70.md and share this C verbatim.
 *
 * IT IS A BITFIELD STORE, AND THAT IS THE WHOLE POINT. Written by hand as
 *
 *      old = s->flags;
 *      v &= 3;
 *      m = 0xd; m = -m;      (the K; -K form, to avoid a pooled negative)
 *      v <<= 2;
 *      m &= old;
 *      s->flags = m | v;
 *
 * the first four instructions come out exactly right and then gcc notices that
 * 3 is still live in r3 and builds -13 as `sub r3, #0x10` -- one instruction
 * where the ROM spends two. Ten lines against the ROM's eleven, with gcc
 * strictly ahead.
 *
 * Moving the negation ahead of the mask does stop the derivation, but then the
 * two constants need separate registers and the sprite pointer is pushed out of
 * r0 into r4, so eleven lines match in count and seven differ in content.
 * Neither ordering can have both.
 *
 * Declared as a bitfield, gcc's store_bit_field expands the mask, the shift and
 * the merge itself, and the two constants are generated as independent RTL that
 * CSE never gets to relate. Exact on the first screen.
 *
 * PROBED AND NEGATIVE before the bitfield reading: -fno-gcse,
 * -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-expensive-optimizations,
 * -fno-strict-aliasing, -fno-rerun-cse-after-loop, -O1. None of them stops the
 * constant derivation. It is not a flag question -- the two spellings simply
 * give gcc different material.
 */

struct Sprite {
    unsigned char pad[9];
    unsigned char lo : 2;
    unsigned char sel : 2;
    unsigned char hi : 4;
};

void OvlFunc_927_20089dc(void *a, int v)
{
    struct Sprite *s;

    s = *(struct Sprite **)((char *)a + 0x50);
    s->sel = v;
}
