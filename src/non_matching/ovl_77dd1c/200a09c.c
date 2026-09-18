/* ==================== SOLVED IN BATCH 271, NOT YET LANDED ====================
 *
 *   OK OvlFunc_882_200a09c -- 96 bytes, 46 encodings and 2 relocations identical
 *
 * objcmp is REQUIRED here -- tryc warns that the reference keeps its literal pool
 * inside the function. Verified candidate: scratch_elev/b271/regalloc/b3.c.
 *
 * TO LAND: asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a_c_c.s holds
 * OvlFunc_882_2009b18 (a ~525-instruction cutscene) ahead of this one, so it needs
 * a two-way text split. No data section. `.L48bc` is already `.global` in
 * asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_c_c_c_c_c.s, so the cross-object
 * reference links.
 *
 * THIS PARK CARRIED NO C BODY -- only a comment header, which is the failure
 * docs/elevation.md records as "a park should carry its candidate C". The body was
 * reconstructed from the asm before anything could be measured.
 *
 * WHY THE PARK'S DOUBLE-READ LEVER WAS THE WRONG CURE FOR THE RIGHT SYMPTOM. The
 * ROM needs `ldrb r3, [r3]` -- address and loaded value in one register -- plus a
 * surviving `mov r1, r3`. The double read (guard on the field, body on a local)
 * does produce the copy, but it also gives the TWO load insns ONE SHARED ADDRESS
 * PSEUDO SPANNING TWO BLOCKS. That pseudo becomes a global allocno, conflicts with
 * the block-local loaded value already holding r3, and is pushed to r2 -- exactly
 * the parked 3.
 *
 * THE CURE IS ONE LOAD, TWO VARIABLES:
 *
 *     c = o->f27;
 *     if (c != 0) { i = c; ... do { } while (--i != 0); }
 *
 * The address is referenced once, dies at the load, stays block-local and reuses
 * r3; `c` spans into the loop-setup block; and the copy `mov r1, r3` survives on
 * its own because `i` is multi-block, so combine_regs refuses to tie it
 * (reg_qty[sreg] == -1). The park's `n = p->f27; if (n != 0)` at 23 differing
 * failed only because it used ONE variable for both roles, which deletes the copy.
 *
 * TWO MORE THINGS WERE NEEDED, both ordinary:
 *   - `.L48bc` through the tree's existing idiom,
 *     `extern unsigned char L48bc[] __asm__(".L48bc");` (as in
 *     src/rom_9000/rom_11ce0_a_c_c_a_a_b.c).
 *   - iwram_3001e40 shifted UNSIGNED (`(unsigned int)x >> 1`), or you get `asr`
 *     where the ROM has `lsr`. And the `& 1` must be a literal 1: cse's
 *     record_jump_equiv knows r1 == 1 inside the `== 1` branch and reuses it,
 *     which is the ROM's `and r3, r1`.
 */

/* OvlFunc_882_200a09c -- asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a.s
 *
 * BLOCKER: REGISTER ALLOCATION (which register holds a computed address)
 *
 * 3 of 49 differing.
 *
 * Identical instructions in identical order; the only difference is which register
 * holds (char*)p + 0x27:
 *   rom  mov r3,r12 / add r3,#0x27 / ldrb r3,[r3]
 *   ours mov r2,r12 / add r2,#0x27 / ldrb r3,[r2]
 * * Got here from 23 differing via the double-read lever (see below), which is the
 * part worth keeping: the ROM has `ldrb r3,[r3] / cmp r3,#0 / ... / mov r1,r3`.
 * `n = p->f27; if (n != 0) {...}` coalesces the copy away; writing the guard on the
 * field and the body on a local -- two textual reads that gcc CSEs -- reproduces it.
 * * MEASURED at 3: unsigned char / cast / signed char field and counter types;
 * ((unsigned char*)p)[0x27]; a named unsigned char *cp; statement-order swaps.
 * -fno-schedule-insns2 12; -O1 worse; -fno-rerun-cse-after-loop 3.
 */
