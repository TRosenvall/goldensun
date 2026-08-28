/* OvlFunc_932_200abe0 -- 0x0200abe0, asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_c_a.s
 *
 * 103 of 103 lines, FIVE differing -- and only THREE are real.
 * Candidate: scratch/Dabe0_best.c.  Came down from 51 on the first screen.
 *
 * TWO OF THE FIVE ARE A MISSING LINKER ALIAS, not C.  Both `%` operations emit
 * `bl __umodsi3` where the ROM calls `bl _umodsi3_RAM`.  Other overlays carry
 * `__umodsi3 = _umodsi3_RAM;` in overlay.ld -- rom_7c7b9c does, and
 * src/overlays/rom_7a5214/ovl_17ec_c_b.c documents the pattern for __divsi3 --
 * but rom_7b9cb4's script has NO such alias.  Adding one is safe (each overlay
 * links separately) and would close those two lines.  It is NOT added here,
 * because an alias that does not complete a match is a link-script change that
 * does not pay for itself; add it in the round that closes the other three.
 *
 * FOUR FINDINGS GOT IT FROM 51 TO 5, all reusable:
 *
 *   1. THE MODULO HELPER SAYS UNSIGNED.  `_umodsi3_RAM`, not `_modsi3_RAM`, so
 *      iwram_3001e40 and __Random are `unsigned`.  The signedness oracle again.
 *   2. THE FIRST ARGUMENT MUST BE INLINE.  Naming the shifted __Random
 *      expression before __CreateActor costs a pseudo and turns the ROM's
 *      `add r1, r2` into `add r1, r0, r2`.  51 -> 44.
 *   3. STATEMENT ORDER IS THE STORE ORDER.  The ROM writes +0x1c and +0x18
 *      BEFORE the byte at +0x61; writing the byte first costs four positions.
 *   4. THE NEGATED MASK NEEDS AN INT INTERMEDIATE, and this one was worth 33
 *      lines by itself.  `p[9] = (p[9] & -13) | 4` truncates to a byte and gcc
 *      emits a single `mov r3, #0xf3`; the ROM builds -13 in SImode with
 *      `mov r3, #0xd / neg r3, r3`.  Through an int local it builds it, and
 *      because that is TWO instructions rather than one it also fixed the
 *      length -- the whole tail was a one-instruction shift.  38 -> 5.
 *
 * BLOCKER: the commutative register-role swap, the same wall as 200a5c0.c.
 *      rom   mov r3, #0xd / ldrb r2, [r1, #9] / neg r3, r3
 *      ours  mov r2, #0xd / ldrb r3, [r1, #9] / neg r2, r2
 * The `and` and the `strb` are identical; only which register holds the constant
 * and which the loaded byte differs.  Both assignment orders were screened
 * (constant first, load first) and both give the identical 5, which is the
 * documented boundary -- source order picks registers for two INDEPENDENT
 * values, not for the operands of a commutative operator.
 */
