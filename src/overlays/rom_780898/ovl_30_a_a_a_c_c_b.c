/* Cluster OvlFunc_883_2008244..OvlFunc_883_2008244 extracted from goldensun/asm/overlays/rom_780898/ovl_30_a_a_a_c_c.s.
 *
 * Split out of that .s; the _a part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_780898/overlay.ld.
 *
 * FillMapRectCollisionByte. Writes one byte into every cell of a rectangle of
 * the map's cell array. Rows are 0x200 bytes apart, cells are 4 bytes, and the
 * byte lands at +2 of the cell -- the collision/attribute byte, not the
 * metatile index. A null [iwram_3001e70] makes it a no-op. Always returns 0.
 *
 * HEAD OF AN 18-MEMBER FAMILY, the largest in the overlays.
 *
 * THREE THINGS ARE LOAD-BEARING, and all three are invisible in the C:
 *
 * 1. THE BRANCH POLARITY. The ROM branches AWAY for layer > 2, so the layer
 *    lookup is the fall-through and therefore the `if` body. Written as
 *    `if (layer > 2) p = gBuffer; else ...` gcc inverts the test and the two
 *    arms swap. docs/elevation.md records this as a general rule and it is
 *    the second time it has decided a function.
 *
 * 2. THE LAYER OFFSET IS A NAMED LOCAL. The ROM loads with a register offset,
 *    `ldr r0, [r2, r3]`, where r3 holds layer*0x30 + 0x130. Written inline,
 *    gcc folds the 0x130 into the address and adds twice. Same lever as the
 *    byte offset in batch 12 -- naming an intermediate stops gcc folding it
 *    into its consumer.
 *
 * 3. THE ADDEND ORDER. The ROM computes `add r3, r6, r3` -- x plus the shifted
 *    z. Written `(z << 7) + x` gcc emits the destructive `add r3, r6`, which
 *    is the same arithmetic and different bytes. Written `x + (z << 7)` it
 *    matches. This was the last instruction to differ and is the kind of thing
 *    that reads as a compiler quirk rather than as something the source said.
 */
extern unsigned char iwram_3001e70[];
extern unsigned char gBuffer[];

int OvlFunc_883_2008244(unsigned int layer, int x, int z, unsigned int width,
                        unsigned int height, int val)
{
    unsigned char *base;
    unsigned char *p;
    unsigned char *q;
    unsigned int row;
    unsigned int col;
    unsigned int off;

    base = *(unsigned char **)iwram_3001e70;
    if (base == 0)
        return 0;
    if (layer <= 2) {
        off = layer * 0x30 + 0x130;
        p = *(unsigned char **)(base + off);
    } else {
        p = gBuffer;
    }
    p += (x + (z << 7)) << 2;
    for (row = 0; row < height; row++) {
        q = p + (row << 9);
        for (col = 0; col < width; col++) {
            q[2] = val;
            q += 4;
        }
    }
    return 0;
}
