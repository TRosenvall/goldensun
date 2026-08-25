/* THE FIVE-TO-TEN-INSTRUCTION FUNCTIONS, and why they are HARDER than the
 * thirty-instruction ones. Read this before spending a round on "the small
 * ones first".
 *
 * The candidate tools rank by size, so these float to the top of every list and
 * look like free wins. They are not. At five to ten instructions there is no
 * structure left to get right -- no control flow, no field layout, no
 * expression shape. Everything that is still wrong is the compiler's
 * ARRANGEMENT: which register a value is born in, and whether an address load
 * is emitted before or after the arithmetic that indexes it. Those are exactly
 * the two residues that park a thirty-instruction function after the real work
 * is done, except here they are the whole function.
 *
 * The levers that carry a 30-line function -- statement order, named locals,
 * assign-back-into-the-parameter, constant-as-destination -- have nothing to
 * bite on. Each was tried on each of these and none moved a single one.
 *
 * FIVE FUNCTIONS, all screened, all close, none matching:
 *
 *   GetFlagByte / SetFlagByte   asm/rom_77000/rom_79338_c_a.s     3 of 5
 *       rom   lsl r3, r0, #0x14 / lsr r0, r3, #0x17 / ldr r3, =gFlags / ldrb
 *       ours  ldr r3, =gFlags / lsl r0, #0x14 / lsr r0, #0x17 / ldrb
 *       gcc materialises the array address before the index; the ROM after.
 *       Tried: index in a named local; the shift split into two locals so the
 *       three-operand `lsl r3, r0, #20` form appears; the array reached through
 *       a pointer local assigned last. --no-sched2 gets it to 2 of 5 and no
 *       further, which says the scheduler is only part of it.
 *
 *   SetTextColor                asm/rom_15000/rom_1de5c_c_a.s     4 of 8
 *       The ROM masks first and computes the address second; gcc does the
 *       reverse. AND ITS 0xf IS A POOL TELL -- `ldr r2, =0xf` where
 *       `mov r2, #0xf` would do, so that operand was a symbol. No namespace
 *       fits a text-colour mask, so it has NOT been named. Masking through an
 *       int local does fix the width (gcc otherwise emits `ldrh` for it, the
 *       batch-71 narrow-constant symptom) but not the order.
 *
 *   OvlFunc_971_2008128         asm/overlays/rom_7fb4a8/...       6 of 9
 *       Every instruction present, six registers permuted. The ROM's store is
 *       `str r2, [r3, r4]` with the SCALED INDEX as the base and the array
 *       address as the offset -- the reverse of what the pointer-typed-operand
 *       lever produces. Writing the store through explicit char* arithmetic in
 *       either direction does not swap them.
 *
 *   Func_8019d0c                asm/rom_15000/rom_1908c_c_c.s     8 of 10
 *       Two halfword stores of one value at +0x12ec and +0x12ee. The ROM
 *       computes an address per store (`add r1, r3, r0` twice, with
 *       `add r0, #2` between); gcc keeps one base and uses register-offset
 *       stores, two instructions shorter. Naming the address in a pointer local
 *       per store does not force the recompute.
 *
 *   OvlFunc_954_2008158         asm/overlays/rom_7db0c8/...       5 of 10
 *       Argument precompute: `__StartTask(fn, 0xc8 << 4)` has TWO expensive
 *       arguments -- a pool-loaded function address and a synthesised shift --
 *       so the shift and the address load come out in the other order. This is
 *       the documented calls.c:805 class; the two-expensive-arguments rule
 *       predicts it exactly.
 *
 * A SIXTH, Func_80a3ce4, is already recorded in docs/elevation.md as sitting on
 * the signed lower-bound floor. It is ten instructions and two of them are
 * unreachable.
 *
 * THE POINT FOR PLANNING. "Clear the smallest band first" is a reasonable
 * instinct and it is wrong here. The 21-40 instruction band is where the
 * hit rate is, because those functions still contain decisions that C can
 * express. Below twenty, the ratio inverts.
 */
