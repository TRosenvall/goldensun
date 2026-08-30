/* UploadIcon -- 0x0801bc34, asm/rom_15000/rom_1aeec_a_a_c_c.s
 *
 * 69 lines against the ROM's 71.  Candidate at scratch/Nicon_best.c.
 * The 38 differing is a two-line shift cascading; the real residue is TWO
 * INSTRUCTIONS, one at each of two sites.
 *
 * SOLVED, and this is the part worth keeping: THE JUMP TABLE REPRODUCES FROM A
 * PLAIN SWITCH.  The ROM dispatches with `sub r0, #1 / cmp r0, #8 / bhi / lsl
 * r3, r0, #2 / ldr r3, [r3, r2] / mov pc, r3` over a nine-entry table, five of
 * whose slots point at the default.  Written as `switch (id)` with cases 1, 2,
 * 4, 6 and 9 -- gcc does the biasing itself -- the table, its entry order and
 * the whole dispatch come out exact, and so do the first 32 lines.  Four
 * elevated functions in the tree already contain a jump table, so the shape was
 * known reachable; this confirms the ordinary spelling is what reaches it.
 *
 * BLOCKER: the ROM copies the AllocSpriteSlot result into a register before
 * using it, and we use it in place.
 *      rom   bl AllocSpriteSlot / mov r2, r0 / str r2, [sp, #8] / cmp r2, #0x60
 *      ours  bl AllocSpriteSlot /              str r0, [sp, #8] / cmp r0, #0x60
 * Ours is one instruction SHORTER at each of the two sites.  `v8` has its
 * address taken -- the other two arms pass `&v8` -- so it lives in the frame,
 * and gcc stores the return value and compares it straight out of r0.
 *
 * TRIED: a separate `t = AllocSpriteSlot(); v8 = t; if (t == 0x60)` at both
 * sites, which is the obvious way to ask for the register copy.  Byte-identical
 * to the original -- gcc knows t and v8 hold the same value and folds the copy
 * away.  Nothing in the source distinguishes "store it" from "keep a copy and
 * store that" when the copy has no other use.
 *
 * Also settled: the two `return -1` paths cross-jump into one shared exit that
 * skips the trailing `ldr r0, [sp, #8]`, which is what writing them as two
 * ordinary `return -1;` statements produces.
 */
