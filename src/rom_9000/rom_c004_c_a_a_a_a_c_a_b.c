/* Cluster Actor_SetAnimSpeed..Actor_SetAnimSpeed extracted from goldensun/asm/rom_9000/rom_c004_c_a_a_a_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/rom_9000/rom_c004_c_a_a_a_a_c_a_a.o and
 * asm/rom_9000/rom_c004_c_a_a_a_a_c_a_c.o in goldensun/stage1.ld.
 *
 * The speed-setting sibling of src/rom_9000/rom_c004_c_a_a_a_a_b.c: identical
 * control flow, calling Sprite_SetAnimSpeed instead of Sprite_SetAnim. Found by
 * tools/match_shapes.py, which collapses callee names -- find_twins.py cannot
 * group these two for exactly that reason.
 *
 * The draw-kind dispatch (`*(p + 0x54) & 0xf`, cases 1 and 2) is the shape
 * documented on the exemplar; the array case walks four slots downward with a
 * post-increment load so the ROM's `ldmia r5!, {r0}` falls out.
 */
extern void Sprite_SetAnimSpeed(unsigned int sprite, unsigned int speed);

void Actor_SetAnimSpeed(unsigned char *p, unsigned int speed) {
    unsigned int v;
    unsigned int *q;
    int i;

    if (p == (unsigned char *)0)
        return;

    switch (*(unsigned char *)(p + 0x54) & 0xf) {
    case 1:
        Sprite_SetAnimSpeed(*(unsigned int *)(p + 0x50), speed);
        break;
    case 2:
        q = *(unsigned int **)(p + 0x50);
        for (i = 3; i >= 0; i--) {
            v = *q++;
            if (v != 0)
                Sprite_SetAnimSpeed(v, speed);
        }
        break;
    }
}
