/* Cluster OvlFunc_927_2008244..OvlFunc_927_2008244 extracted from goldensun/asm/overlays/rom_7b4558/ovl_30_a_a_c_a_c_b.s.
 *
 * Split out of that .s; the sibling part stays as assembly and keeps its slot
 * in the overlay's linker script.
 *
 * FillMapRectCollisionByte, one of eighteen identical copies -- one per
 * overlay, byte-for-byte the same body. See
 * src/overlays/rom_780898/ovl_30_a_a_a_c_c_b.c for the three things that are
 * load-bearing; all three are invisible in the C and one of them looks like a
 * compiler quirk.
 */
extern unsigned char iwram_3001e70[];
extern unsigned char gBuffer[];

int OvlFunc_927_2008244(unsigned int layer, int x, int z, unsigned int width,
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
