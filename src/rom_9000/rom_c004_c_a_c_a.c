/* Func_800c548 and Func_800c570, the whole of goldensun/asm/rom_9000/rom_c004_c_a_c_a.s.
 *
 * Total .text for this TU = 88 bytes (= 0x58). The .s is replaced outright, so
 * no linker-script change was needed.
 *
 * Two guarded bit-setters on the sprite record hanging off an actor at +0x50:
 * a two-bit field at bits 2-3 of byte +5, and a one-bit flag at bit 1 of byte
 * +0x1d. Both do nothing unless the actor exists and its kind byte at +0x54 is
 * 1.
 *
 * THESE ARE TWO OF THE "34-FUNCTION BLOCKER" NAMED IN docs/elevation.md as the
 * single highest-value problem in the project, and the fix is to stop writing
 * the masking by hand.
 *
 *     rom    mov r3, #0xd / neg r3, r3      (~0xc built at 32-bit width)
 *     ours   mov r3, #0xf3                  (~0xc narrowed to a byte)
 *
 * The old reading was that gcc proves the loaded value is 0..255 and picks the
 * cheaper 8-bit immediate, so the job was to hide the width. Eleven statement
 * orders, six width-hiding tricks and four mask spellings were tried against
 * that reading and none of them worked.
 *
 * Declared as a bitfield and assigned directly --
 *
 *     unsigned char lo : 2, sel : 2, hi : 4;
 *     ...
 *     a->spr->sel = v;
 *
 * -- gcc's store_bit_field builds the mask, the shift and the merge itself, at
 * int width, and the whole function matches on the first screen. The width was
 * never the thing to fix; writing the merge by hand was.
 */

struct Sprite {
    unsigned char pad05[5];
    unsigned char f5_lo : 2;
    unsigned char f5_sel : 2;
    unsigned char f5_hi : 4;
    unsigned char pad06[0x17];
    unsigned char f1d_lo : 1;
    unsigned char f1d_bit : 1;
    unsigned char f1d_hi : 6;
};

struct Actor {
    unsigned char pad[0x50];
    struct Sprite *spr;
    unsigned char kind;
};

void Func_800c548(struct Actor *a, int v)
{
    if (a != 0) {
        if (a->kind == 1)
            a->spr->f5_sel = v;
    }
}

void Func_800c570(struct Actor *a, int v)
{
    if (a != 0) {
        if (a->kind == 1)
            a->spr->f1d_bit = v;
    }
}
