/* Cluster HasMove..HasMove extracted from goldensun/asm/rom_77000/rom_78b9c_a_c.s.
 *
 * Slotted between rom_78b9c_a_c_a.o and the rest of stage1.ld.
 *
 * Scans 32 move slots for one whose low 14 bits match.
 *
 * THE MASK IS A NAMED LOCAL AND THE LOADED VALUE IS THE AND'S DESTINATION.
 * Written as `v = *(unsigned short *)p & 0x3fff;` the mask ends up in a
 * callee-saved register and gcc copies it into place every iteration --
 * `ldrh r2 / mov r3, r4 / and r3, r2` against the ROM's `ldrh r3 / and r3, r1`.
 * 8 of 23. Splitting the load and the mask into two statements, so `v` is what
 * the `and` writes, matches.
 *
 * The pointer advances BEFORE the comparison -- `p += 4;` sits between the mask
 * and the `if` -- because the ROM does `add r0, #4` there.
 */
extern void *GetUnit(void);

int HasMove(int unused, int want)
{
    unsigned char *p;
    int i;
    int v;
    int mask;

    p = (unsigned char *)GetUnit();
    mask = 0x3fff;
    i = 0;
    p += 0x58;
    do {
        v = *(unsigned short *)p;
        v &= mask;
        p += 4;
        if (v == want)
            return 1;
        i++;
    } while (i <= 0x1f);
    return 0;
}
