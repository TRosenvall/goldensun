/* OvlFunc_common1_4cc -- asm/overlays/common/common1_a_a_a_a_c_a_a.s
 *
 * REFERENCE .s REPOINTED (batch 337).  The split that landed its piece-mate
 * OvlFunc_common1_588 consumed common1_a_a_a_a_c_a.s; this function is now in
 * common1_a_a_a_a_c_a_a.s.  install_batch.py flagged this park for manual
 * handling because it names the .s without a --func, so the automatic repoint
 * could not act on it.
 *
 * THE SUBJECT LINE MUST STAY FIRST.  I originally recorded this note by
 * PREPENDING a block above it, which moved the subject off line 1 and broke
 * tools/piece.py's bodyless-park lookup -- this park has no body, so the
 * subject line is the only thing identifying it.  Notes go UNDER the subject.
 *
 * AND IT IS THE OBVIOUS NEXT TARGET.  Brief D landed OvlFunc_common1_588 at
 * -O1 on the SPECULATIVE LITERAL HOIST blocker (find_if_case_1,
 * ifcvt.c:1651).  This park carries the SAME blocker with the same message
 * ids, so -O1 is the indicated remedy here too; see the Makefile rule beside
 * the mis-scoped-wildcard note, and keep any new rule EXPLICIT, not a pattern.
 *
 * The 'Candidate at scratch/N04cc_best.c' below DOES still exist -- checked,
 * not assumed.
 *
 * 78 lines against the ROM's 76.  Candidate at scratch/N04cc_best.c.
 *
 * BLOCKER: A CALL TAIL THE ROM SHARES AND GCC WILL NOT.  Three arms pick one of
 * three message ids and pass it to __MessageID.  The ROM loads the id in each
 * arm and BRANCHES TO ONE SHARED CALL:
 *
 *      cmp r2, r3 / bne .L4f4 / ldr r0, =0x2076 / b .L500
 *      .L4f4: ... / ldr r0, =0x2078 / b .L500
 *      .L4fe: ldr r0, =0x207a
 *      .L500: bl __MessageID
 *
 * Written as three `__MessageID(...)` calls gcc emits three `bl`s and no shared
 * tail: 78 lines, two too many.  Written as an id assigned in each arm and one
 * call after the join -- which is the shape the ROM has -- gcc IF-CONVERTS it,
 * hoisting each `ldr r0, =id` ABOVE its own compare and using `beq` to fall
 * into the call: 72 lines, four too few.  Neither count is the ROM's and the
 * chain shape differs in both.
 *
 * SCREENED: three separate calls (78/61); a shared `int m` assigned per arm
 * (72/63); the same written with explicit gotos to a `msg:` label, which is the
 * ROM's control flow spelled out and gcc collapses it identically (72/63); and
 * each of those with __Func_8019908's prototype dropped, which is a real fix
 * for a separate residue -- it moves the first difference from line 4 to line
 * 14 by putting `mov r0` last in that call's setup -- but does not touch this.
 *
 * THIS IS THE SECOND FUNCTION WITH THIS EXACT RESIDUE.
 * src/non_matching/ovl_7fb4a8/2008e10.c parks on it too: there the ROM merges
 * two __MessageID calls one instruction further back than we do, sharing `bl
 * __MessageID` with the id already in r0, and forcing the merge with an
 * explicit shared local made it worse for the same reason it does here.  Two
 * independent functions, same shape, same failure: gcc-2.96 as invoked here
 * does not cross-jump identical call tails that differ only in a pooled
 * constant, and the original build did.  That is a class, not a coincidence,
 * and it is worth a flag sweep by whoever picks it up next.
 *
 * SOLVED and not to be re-derived: the two area comparisons are SYMBOLS.
 * `ldr r3, =0x8f` for a value `cmp Rn, #imm8` covers is the pooled-constant
 * tell, the halfword at gState+0x1c0 is the area id by area.sym's own header,
 * and _AREA_8f / _AREA_90 were already in the file -- adding them again is the
 * mistake I made first.  Also: the gState base needs a named pointer or the
 * +0x1c0 folds into the pool constant.
 */
