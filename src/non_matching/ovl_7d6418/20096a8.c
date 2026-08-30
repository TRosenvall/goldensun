/* OvlFunc_951_20096a8 -- 0x020096a8,
 * asm/overlays/rom_7d6418/ovl_30_c_c_c_c.s
 *
 * 53 lines against the ROM's 54, 34 differing.  Candidate at
 * scratch/N96a8_best.c.  TWO residues, both in the allocator.
 *
 * 1. A FOUR-WAY REGISTER ROTATION.  Four pointers and a counter are live across
 *    the table loop.  The ROM assigns them r6, r5, r4, r7; we assign r7, r6, r5,
 *    r4 -- the same registers, each shifted by one, with the same prologue.  The
 *    materialisation ORDER is already identical to the ROM's, so there is
 *    nothing left in the source to reorder: three declaration permutations
 *    (counter first, pointers reversed, counter mid-list) are byte-identical to
 *    each other and to the original.  This is the class of
 *    src/non_matching/ovl_7ced6c/2009c84.c and .../200a16c.c one dimension
 *    wider, and those two already establish that the source has no handle on it.
 *
 * 2. THE POST-LOOP ZEROS ARE A SECOND VARIABLE.  The ROM materialises a fresh
 *    `mov r2, #0` after the loop for the six header stores, keeping the loop's
 *    own zero in r1; we reuse the loop's zero for both, which is the one missing
 *    line.  Declaring a separate `z2 = 0;` beside the header stores does NOT
 *    separate them -- gcc CSEs on the value, not the spelling, exactly as
 *    src/non_matching/ovl_7d30e0/20091d8.c records for the same question.
 *
 * SOLVED and worth keeping: the table walk itself screens clean.  The 0x18-byte
 * entry as a struct with three int fields and five shorts, the three source
 * cursors advancing by 1, 1 and 2, and the `*a << 16` byte-to-word widening all
 * reproduce; the five `.LNNNN` data labels are four hex digits so they need no
 * _TBL_ alias.
 *
 * WHY IT IS HERE AT ALL: this function has FIVE calls and the candidate filter
 * required eight until this round.  It is a fair test of the relaxed floor and
 * it failed on a wall that has nothing to do with call density.
 */
