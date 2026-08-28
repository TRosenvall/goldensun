/* Func_80160fc -- asm/rom_15000/rom_15e8c_a_c_a_a.s
 *
 * BLOCKER: gcc will NOT derive one constant from another here, and the ROM does
 * -- the inverse of the usual constant-CSE problem.  44 of 54, ours 53 lines.
 *
 *     rom  ldr r2, =0xea6 / ... / sub r2, #0x3 / add r3, r7, r2 / ldrb
 *     ours ldr r3, =0xea6 / ... / ldr r3, =0xea3               / ldrb
 *
 * Every other blocker in the corpus is gcc commoning two constants the ROM
 * rebuilds.  This one is the reverse: the ROM's compiler related 0xea6 and
 * 0xea3 with a `sub`, and ours emits two independent pool loads.  Writing the
 * offset as one variable mutated in place (`off = 0xea6; ... off -= 3;`) does
 * not reach it -- gcc constant-folds `off` at each use, because both values
 * are compile-time constants and there is nothing to stop it.
 *
 * WHAT DID WORK, and is reusable:
 *   * The `goto` loop is right.  Both `ldr r3, =REG_DMA3SAD` and
 *     `ldr r2, =0x84000040` sit INSIDE the loop body in the ROM, which is the
 *     documented signature; a `do/while` hoists them.
 *   * `one = 1;` and `step = 0x80 << 1;` before the loop are source variables,
 *     not hoists -- the corollary to the goto rule.
 *   * A NAMED destination pointer per access (`q = (unsigned char *)(b + off);`)
 *     gets the ROM's `add r3, r7, r2 / ldrb r3, [r3]` instead of a reg+reg
 *     `ldrb r3, [r6, r3]`.  Worth 8 instructions: 52 differing at 51 lines
 *     becomes 44 at 53.
 *
 * MEASURED:
 *   offset inline, reg+reg addressing        52 of 54, ours 51 lines
 *   offset named and mutated, named pointer  44 of 54, ours 53 lines
 *
 * Best C: scratch/L60fcb.c.
 */
