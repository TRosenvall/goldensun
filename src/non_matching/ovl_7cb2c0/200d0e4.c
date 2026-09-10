/* OvlFunc_945_200d0e4 -- 0x0200d0e4,
 * asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_c_a_a_c_a.s
 *
 * FOUR differing of 206, at EXACT size, exact encoding count and IDENTICAL
 * relocations. Candidate and sweep in scratch_elev/b257/a2/d0e4/. Its file-mate
 * OvlFunc_945_200d2f4 is ELEVATED, so this .s is the _a remainder of that split.
 *
 * BLOCKER CLASS: sched2 HOISTS A CONSTANT ACROSS TWO CALLS THAT THE REFERENCE'S
 * sched2 LEFT IN PLACE -- a `(set (reg) (const_int 0x1e46))` moved across two
 * `bl`s.
 *
 * THE DISCRIMINATOR IS A PAIR OF DUMPS, not a source experiment: the placement
 * is CORRECT in .17.lreg and WRONG in .23.sched2. That localises it to sched2
 * with no spelling search at all.
 *
 * AND THERE IS NO LEVER FROM C. `-fno-schedule-insns2` REGRESSES 4 -> 65, so
 * the flag is not the answer either, and C has no barrier that reaches a
 * post-reload scheduling decision inside one block. Not reached by: a drop-one
 * over all 37 pins; a FULL order-permutation pass over all 37 sites; eight
 * statement placements.
 *
 * TWO REUSABLE THINGS RECOVERED ON THE WAY, both already load-bearing here:
 *
 *   `adds r5, #1` IS A LIVE-ACROSS-A-CALL TELL, NOT A use_related_value ONE.
 *   The recorded reading of that shape is the related-value rule; here the
 *   increment simply has to happen before the intervening call. Moving the
 *   `+= 1` before it is 45 -> 6, and moving it inside the pin block is 6 -> 4.
 *
 *   THREE INDIVIDUALLY INERT FILL ORDERS WERE WORTH 7 TOGETHER. That is the
 *   ordering analogue of the recorded "eviction pins must be added as a set",
 *   and it means a fill-order sweep that changes one site at a time can report
 *   every site inert while the set is load-bearing.
 *
 * LANDING IF CLOSED: this is the _a half of an already-split file, so closing it
 * lets the two halves collapse back toward one TU. The overlay.ld lines MUST
 * KEEP their asm/ paths. makefile_flags() is empty.
 */
