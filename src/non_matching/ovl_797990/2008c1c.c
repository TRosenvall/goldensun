/* BATCH 207 -- THE BARRIER LEVER IS RULED OUT BY MEASUREMENT, not by omission.
 * This park closes by saying "source position does not reach post-reload
 * scheduling of a copy", and it is right. Batch 206 found a lever that DOES
 * reach post-reload scheduling -- a volatile asm consuming the value, which
 * forces it materialised at that point -- so this residue, a `mov r2, r8` landing
 * one instruction late, was the obvious place to spend it.
 *
 * MEASURED:
 *
 *     q = s; (a copy born in the gap), no barrier          2   (as before)
 *     volatile asm on s directly, before the mask store   10
 *     q = s; then a volatile asm on q, before the store   65   and 74 lines
 *
 * The copy alone is inert, exactly as this park already recorded. The barrier is
 * active and it destroys the function.
 *
 * WHY, and it is the same cause as in src/non_matching/ovl_7ebdfc/2008120.c,
 * measured in the same round: the barrier shortens every live range crossing the
 * split, and THIS FUNCTION'S CORRECTNESS DEPENDS ON A LONG ONE. The sprite
 * pointer lives in r8 across four calls -- that is what `mov r2, r8` IS -- and
 * once the range is cut, gcc stops using r8 at all and the allocation is
 * renumbered from instruction 6 onward.
 *
 * The rule, measured across five functions and written up in docs/elevation.md:
 * THE BARRIER IS ONLY AVAILABLE WHERE THE ROM DOES NOT USE r8-r11. Five
 * instructions here. Note the irony worth keeping: the residue is a copy OUT OF
 * a high register, so the very thing that makes the site interesting is the
 * thing that makes the lever unusable.
 */
/* OvlFunc_901_2008c1c -- asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_c_c_a_a.s
 * OvlFunc_898_2009090 -- asm/overlays/rom_793768/ovl_314_c_c_c_a_c_a_c_c_a.s
 *
 * TWO-MEMBER FAMILY (cross-overlay duplicates).  2 of 75, same length.
 *
 * 73 of 75 exact, including four __CopyMapTiles sites sharing one carried `2`,
 * the stack-arg pair for __Func_8010704, and both bit-twiddles.  The residue
 * is one scheduling transposition:
 *
 *     rom  and r3, r2 / mov r2, r8      / strb r3, [r6]
 *     ours and r3, r2 / strb r3, [r6]   / mov r2, r8
 *
 * `mov r2, r8` is gcc copying the sprite pointer out of a high register for
 * the next statement's `ldrb [r2, #9]`.  The ROM issues it before the mask
 * store; gcc after.
 *
 * THE USEFUL FINDING IS THE TYPE ASYMMETRY, and it contradicts the sharpening
 * made the same day for `orr`:
 *
 *     orr, constant as destination   ->  needs `unsigned char m`
 *                                        (`int m` is folded, byte-identical
 *                                        to the plain literal)
 *     and, constant as destination   ->  needs `int m`
 *                                        (`unsigned char m` puts the LOADED
 *                                        value in the destination instead)
 *
 * Measured here: `unsigned char m = 0xfe;` gives 4 differing;
 * `int m = 0xfe;` gives 2 and fixes the operand roles.  Both spellings of
 * "make the constant the destination" (`*p = m & *p;` and
 * `m = m & *p; *p = m;`) are byte-identical to each other under either type,
 * so the type is the whole lever and the statement form is inert.
 *
 * ALSO MEASURED, all 2 differing at position 57:
 *   `p = a + 0x23;` moved before the __Func_8010704 call    2
 *   the two independent stores swapped in source           16 (much worse)
 *
 * BATCH 167 -- the address-local BIRTH-STATEMENT lever does NOT reach this.
 * That lever (docs/elevation.md) says the statement gap a pointer is born in
 * decides its register and its placement, and it closed OvlFunc_886_20090c0
 * from 4 differing to 1. It is the obvious thing to try here, because the
 * residue IS a pointer copy landing one statement late. It does not work:
 *   a copy `q = s;` born between `m = 0xfe;` and the mask store     2
 *   the same copy born before `m = 0xfe;`                          2
 *   `s` re-derived from `a` at the use site      69 lines, 74 differing
 * Flags are inert too: -fno-schedule-insns and -fno-strict-aliasing both give
 * 2, and -fno-schedule-insns2 gives 41 at position 3 -- sched2 is doing
 * correct work everywhere else in this function.
 *
 * The distinction that explains it: in OvlFunc_886_20090c0 the local was born
 * from a BASE POINTER and the birth statement decided which register it got.
 * Here the value is already live in r8 across the calls and every use
 * rematerialises a copy out of it; what differs is where sched2 puts a
 * register-to-register move, and source position does not reach post-reload
 * scheduling of a copy. So: the birth-statement lever governs a pointer that is
 * COMPUTED, not one that is COPIED out of a high register.
 *
 * Best C: scratch/J8c1c.c.
 */
