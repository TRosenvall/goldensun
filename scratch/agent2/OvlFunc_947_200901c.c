/* OvlFunc_947_200901c  --  asm/overlays/rom_7d0e88/ovl_314_c_a_a.s
 *
 * Copies two bitfields and two bytes of one 4-byte map cell onto another.
 * Same layer-table lookup as OvlFunc_883_2008244 (the 18-member family head):
 * `off = layer * 0x30 + 0x130` must be a NAMED LOCAL or gcc folds 0x130 into
 * the address and adds twice, and the cell offset is `(x + (z << 7)) << 2`
 * with x first (`add r3, r6, r3`, not the destructive form).
 *
 * The two field copies are BITFIELD assignments, not masking: the ROM's
 * `mov r2, #0x31 / neg r2` is a 32-bit ~0x30, and gcc merges the second write
 * into the value already in r2 (one ldrb, two strb).  The reads come out at
 * different widths -- `ldr` + lsl18/lsr30 for bits 12-13, `ldrb` + lsr6 for
 * bits 14-15 -- from a single `unsigned char` bitfield declaration.
 * No --cflags.
 */
extern unsigned char iwram_3001e70[];

struct Cell {
    unsigned char b0;
    unsigned char lo : 4;
    unsigned char mid : 2;
    unsigned char hi : 2;
    unsigned char b2;
    unsigned char b3;
};

void OvlFunc_947_200901c(unsigned int layer, int x, int z, struct Cell *s)
{
    unsigned char *base;
    struct Cell *p;
    unsigned int off;

    base = *(unsigned char **)iwram_3001e70;
    if (base == 0)
        return;
    off = layer * 0x30 + 0x130;
    p = *(struct Cell **)(base + off);
    p += x + (z << 7);
    p->mid = s->mid;
    p->hi = s->hi;
    p->b2 = s->b2;
    p->b3 = s->b3;
}
