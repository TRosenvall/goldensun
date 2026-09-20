/* Cluster OvlFunc_882_200a09c..OvlFunc_882_200a09c extracted from goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_c_c.s.
 *
 * Total .text for this TU = 96 bytes (= 0x60).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_c_c_a.o and asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_c_a.o in
 * goldensun/overlays/rom_77dd1c/overlay.ld.
 *
 * Solved in batch 271 and landed in 272. No pins. objcmp rather than tryc is the
 * screen here: the reference keeps its literal pool inside the function, which
 * tryc warns about.
 *
 * THE PARK CARRIED NO C BODY AT ALL -- only a comment header, which is the failure
 * docs/elevation.md names ("a park should carry its candidate C"). The body had to
 * be reconstructed from the asm before anything could be measured.
 *
 * WHY THE PARK'S DOUBLE-READ LEVER WAS THE WRONG CURE FOR THE RIGHT SYMPTOM. The
 * ROM needs `ldrb r3, [r3]` -- address and loaded value in one register -- AND a
 * surviving `mov r1, r3`. A double read (guard on the field, body on a local) does
 * produce the copy, but it gives the TWO load insns ONE SHARED ADDRESS PSEUDO
 * SPANNING TWO BLOCKS. That pseudo becomes a global allocno, conflicts with the
 * block-local loaded value already holding r3, and is pushed to r2 -- exactly the
 * parked 3.
 *
 * THE CURE IS ONE LOAD, TWO VARIABLES. `c = o->f27; if (c != 0) { i = c; ... }`:
 * the address is referenced once, dies at the load, stays block-local and reuses
 * r3; `c` spans into the loop-setup block; and the copy survives on its own
 * because `i` is multi-block, so combine_regs refuses to tie it
 * (reg_qty[sreg] == -1). Using ONE variable for both roles deletes the copy, which
 * is why the park's `n = p->f27; if (n != 0)` measured 23.
 *
 * TWO ORDINARY THINGS WERE ALSO NEEDED:
 *   - `.L48bc` through the tree's existing idiom,
 *     `extern unsigned char L48bc[] __asm__(".L48bc");` (as in
 *     src/rom_9000/rom_11ce0_a_c_c_a_a_b.c). It is already `.global` in
 *     asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_c_c.s, so the cross-object
 *     reference links.
 *   - iwram_3001e40 shifted UNSIGNED, or you get `asr` where the ROM has `lsr`.
 *     And the `& 1` must be a literal 1: cse's record_jump_equiv knows r1 == 1
 *     inside the `== 1` branch and reuses it, which is the ROM's `and r3, r1`.
 */
extern int iwram_3001e40;
extern unsigned char L48bc[] __asm__(".L48bc");

void OvlFunc_882_200a09c(unsigned char *p, int n)
{
    unsigned char *o;
    unsigned char **e;
    int v;
    int c;
    int i;

    if ((p[0x54] & 0xf) == 1) {
        o = *(unsigned char **)(p + 0x50);
        v = n - 1;
        if (n == 0)
            v = L48bc[((unsigned int)iwram_3001e40 >> 1) & 1];
        c = o[0x27];
        if (c != 0) {
            e = (unsigned char **)(o + 0x28);
            i = c;
            do {
                unsigned char *x;
                x = *e++;
                if (x != 0 && *(int *)(x + 0x10) != 0)
                    x[5] = v;
            } while (--i != 0);
        }
        o[0x25] = 1;
    }
}
