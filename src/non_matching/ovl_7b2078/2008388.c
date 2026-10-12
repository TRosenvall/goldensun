/* OvlFunc_926_2008388 -- NON-MATCHING.  Blocker class: CONSTANT CSE ACROSS A
 *
 * NON-MATCHING, 19 of 62 encodings  (RE-MEASURED, batch 333 brief B).
 *   COUNTS AGREE: ref 62, ours 62, instructions fifty-three against
 *   fifty-three, so this figure IS a distance.  The earlier twenty-six was
 *   measured on a stream one instruction long.
 *   THE RELOCATION WARNING THAT USED TO STAND HERE IS WITHDRAWN: the only
 *   symbol difference is `_AREA_3c`, and area.sym line seventy-five already
 *   carries `_AREA_3c = 0x3c`.  An absolute assignment resolves at link time
 *   and emits no bytes, so objcmp on a single function always shows it as a
 *   relocation against a zero word.  That is the same artefact Func_8091eb0's
 *   park documents for its own device symbol, not a blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b2078/2008388.c \
 *     asm/overlays/rom_7b2078/ovl_314_c_c_a_a_a.s --func OvlFunc_926_2008388
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * CALL -- and it is a COUNTEREXAMPLE to the documented remedy, which is the
 * reason this park is worth reading.
 *
 * 60 lines against the ROM's 59, 25 differing.  The first 23 lines are exact,
 * including the area comparison, so the `_AREA_3c` half of this function is
 * finished -- this came out of the "209 functions are elevatable NOW" pool in
 * HANDOFF.md and that part of it worked.
 *
 * THE ONE CAUSE
 *
 *     rom   ldr r0, =0x895 / bl __GetFlag   ...   ldr r0, =L4998 / ldr r1, =0x895
 *     ours  ldr r5, =0x895 / mov r0, r5 / bl __GetFlag  ...  (r5 reused)
 *
 * 0x895 is a save-bit id: tested with __GetFlag, and then -- only if the test
 * passes -- stored into four halfword fields of the table at .L4998.  The ROM
 * takes TWO pool entries for it.  gcc-2.96 hoists it into r5, keeps it live
 * across the call, and every later register assignment shifts by one.
 *
 * WHY THIS IS A COUNTEREXAMPLE
 *
 * docs/elevation.md, "Pool-constant CSE: the complete rule", says recovering
 * the ROM's reload needs BOTH a control-flow boundary between the two uses AND
 * -fno-rerun-cse-after-loop, and that with no boundary nothing reaches it.
 *
 * Here BOTH preconditions are met.  The first use dominates the second, and
 * there is a real boundary between them -- the `beq` around the whole store
 * block.  The flag still does not move it.  So the rule states a necessary
 * condition, not a sufficient one, and a boundary plus the flag is not a
 * recipe.  tools/blocked_cse.py counts 585 functions in this class on the
 * strength of the boundary test; that count is a population, not a worklist.
 *
 * Tried, all 25 differing and most byte-identical to each other:
 *   - the literal 0x895 in all five places
 *   - the stored value in a named local inside the guarded block, which is the
 *     lever that took ovl_7b8cb0/200807c from 15 differing to 6
 *   - --no-rerun-cse, and --cflags with each of -fno-gcse,
 *     -fno-cse-follow-jumps, -fno-expensive-optimizations, -fno-force-mem
 *
 * CHECKED AND RULED OUT: that the stored 0x895 is a symbol rather than a
 * literal, which would explain two pool entries naturally.  It is in none of
 * area.sym, message.sym, const.sym or file_table.sym.  If a save-bit namespace
 * is ever given a .sym file, re-try this first -- a `_FLAG_895` address would
 * pool separately from the integer and the CSE could not fire.

 * ========================================================================
 * BATCH 333 BRIEF B -- TWENTY-SIX DOWN TO NINETEEN, LENGTHS NOW EQUAL, AND
 * THE PARK'S "TWO POOL ENTRIES" OBSERVATION IS A MISREADING OF THE .s
 * ========================================================================
 *
 * DIRECTION, STATED BACK: this park was OURS LONGER, by one instruction.  So
 * the search was for a value we keep that the original build did not, never
 * for a statement to add.
 *
 * THE LEVER IS ONE MOVED STATEMENT.  `p = L4998;` hoisted ABOVE the
 * `if (__GetFlag(...))` aligns the lengths and takes the figure to nineteen.
 * Mechanism, and it is already written down in this tree:
 * src/rom_c9000/rom_d82b0_b.c's header records "A POOL LOAD HOISTED ABOVE A
 * CALL IS A REGISTER-ROTATION LEVER", because lengthening the pool pseudo's
 * live range changes which pseudo wins the local-alloc race.  Same effect
 * here, in a different overlay, on a different constant.
 *
 * REFUTED, AND IT IS THE REASON THIS PARK READ AS A COUNTEREXAMPLE.  The park
 * above says "The ROM takes TWO pool entries for it".  It does not.
 * `ldr r0, =0x895` and `ldr r1, =0x895` in a HAND-WRITTEN .s are two loads of
 * ONE pool word -- the assembler's `=` syntax deduplicates identical literals.
 * Counted both ways: the reference and our candidate each have exactly SIX
 * pool words (gState, the area value, the two returned tables, the save-bit id
 * and the stored-to table) and each shows eight thirty-two-bit stream entries,
 * of which two are the calls.  Corpus check, over all four and a half thousand
 * generated .s files in asm/: NOT ONE has a duplicated numeric literal-pool
 * word.  The single apparent hit is a `.rodata` array, not a pool.
 *   >> So there is no "two pool entries" phenomenon to reproduce.  The ROM
 *   >> simply RELOADS one pool word twice, which is ordinary pool-constant
 *   >> reload, and the park's claim that this function refutes the documented
 *   >> remedy rests on the misreading.  The boundary-plus-flag observation
 *   >> still stands on its own evidence; the counterexample framing does not.
 *
 * WHY THE EXTRA INSTRUCTION WAS THREE STATEMENTS AWAY FROM ITS CAUSE.  The
 * save-bit id hoisted into a callee-saved register costs a copy into the
 * argument register, and saves the reference's reload inside the guarded block
 * -- those cancel.  The surplus showed up at the LAST TWO STORES instead: the
 * reference holds the table base in the FIRST argument register, so the only
 * free low registers at that point are the two the offsets need, and
 * reload_cse_move2add is forced to increment the offset register in place
 * (`add <off>, #0x18`).  In ours the first argument register is free, the
 * second offset is born in it, and a different register cannot be incremented
 * from the first -- so the offset is rebuilt with a shift pair.  One cause,
 * surfacing as a lost move2add well downstream of the register it came from.
 *
 * MEASURED EXACTLY INERT -- all nineteen with the counts equal, CROSSED WITH
 * THE HOIST (which is why they are listed again; the park's rejected rows had
 * been measured without it): the stored value in a named local inside the
 * guarded block; a second pointer local advanced in place for the last pair;
 * the flag-test argument passed through a named local; the tail call and the
 * return taken through the hoisted pointer rather than the table name.
 * MEASURED AS A DIFFERENT PROGRAM, recorded so it is not re-found and
 * believed: ANY running-offset spelling (`off = 0x85 << 1; ... off += 0x18;`)
 * OVERSHOOTS to fifty or fifty-one instructions, because `*(short *)(p + off)`
 * with a VARIABLE offset is a legal Thumb register-plus-register address, so
 * gcc drops the separate address computation the reference performs.  A
 * CONSTANT offset is what makes gcc materialise the address first.  That is a
 * real asymmetry between a named offset and a literal one, and it is the
 * opposite of the usual direction.
 * MEASURED WORSE: the tail taken through a second, separate pointer local
 * (twenty-six, one instruction long again).
 *
 * NEXT: the residue is the extra callee-saved register plus the lost move2add,
 * and they are ONE question -- get the table base into the first argument
 * register inside the guarded block and both follow.  Do not vary the stores.
 *
 * THIS PIECE HOLDS ONLY THIS FUNCTION, so a landing here is installable alone.
 *
 * ========================================================================
 * BATCH 341 BRIEF B -- FIGURE REPRODUCED AT NINETEEN, THE RESIDUE REDUCED TO
 * ONE CAUSE WITH A QUANTITATIVE BOUND, AND THIS PARK'S OWN "NEXT" REFUTED
 * ========================================================================
 *
 * RE-DERIVED: **19 of 62**, ref 53 instructions against ours 53, 9 against 9
 * thirty-two-bit entries, 140 bytes against 140.  A TRUE DISTANCE.  The
 * relocation line is one extra R_ARM_ABS32 against `_AREA_3c` at 0x78 where
 * the reference has a plain `0x3c` literal in the same pool slot; area.sym
 * line 75 defines `_AREA_3c = 0x3c` by value, so it links to the literal and
 * emits no bytes.  The park's withdrawal of that warning is correct.
 *
 * autosweep.py: BOTH CHEAP AXES CLOSED.  Flags that TIE at 19: -fno-gcse,
 * -ffixed-r7, -fno-strict-aliasing, -fno-strength-reduce,
 * -fno-rerun-cse-after-loop, -fno-rerun-loop-opt.  -O1 is 33,
 * -fno-schedule-insns2 is 25.  All 13 anchor placements inert.
 *
 * ---- THE RESIDUE IS ONE CAUSE, AND IT IS NOT THREE ----
 *
 * The park lists its residue as the extra callee-saved register plus a lost
 * reload_cse_move2add, and says they are one question.  They are, and so is a
 * third thing the park does not mention (the in-block table base landing in the
 * WRONG LOW REGISTER).  All three are one cause, PROVED by a diagnostic:
 *
 *   A VARIANT IDENTICAL IN EVERY WAY EXCEPT THAT THE STORED HALFWORD IS 0x95
 *   INSTEAD OF 0x895 PRODUCES THE REFERENCE'S GUARDED BLOCK INSTRUCTION FOR
 *   INSTRUCTION -- the base in r0, the value in r1, and `add r2, #0x18`, the
 *   move2add the park says is lost:
 *
 *       ldr r0, <base>   /  ldrh r1, <value>  /  mov r3, r0  /  add r3, #122
 *       strh r1, [r3]    /  add r3, #48       /  strh r1, [r3]
 *       mov r2, r0       /  mov r3, #144      /  add r2, #200 / lsl r3, #17
 *       str r3, [r2]     /  mov r3, #248      /  add r2, #8   / lsl r3, #16
 *       str r3, [r2]     /  mov r2, #133      /  lsl r2, #1
 *       add r3, r0, r2   /  add r2, #24       /  strh r1, [r3]
 *       add r3, r0, r2   /  strh r1, [r3]
 *
 *   (It is NOT a candidate -- it is a different program, and it also drags in
 *   the mid-function HImode minipool that Func_8091eb0's park documents, so it
 *   reads 49 of 62 at 54 instructions.  Kept as the diagnostic it is.)
 *
 * ---- THE CAUSE, WITH THE COST ARITHMETIC ----
 *
 * `cse.c` unifies the pre-call argument constant and the in-block stored
 * constant into ONE pseudo, which then crosses the call and must take a
 * callee-saved register.  Read off the dumps of the park body: .00.rtl has
 * FIVE separate sets of `(const_int 2197)`; by .03.cse there is ONE, and every
 * store's source has become `(subreg:HI (reg:SI 50) 0)` -- reg 50 being the
 * pseudo that feeds r0 for `__GetFlag`.
 *
 * TWO PLACES IN cse.c DO IT and both decide on COST, not on control flow:
 *   * cse.c:4977-5002, "See if we have a CONST_INT that is already in a
 *     register in a wider mode" -- its gate is
 *     `GET_MODE_BITSIZE (mode) < BITS_PER_WORD`, true for an HImode store;
 *   * failing that, the ordinary cheapest-equivalent loop at cse.c:5195-5240,
 *     which documents its own tie-break: "For items of equal cost, use this
 *     order: src_folded, src, src_eqv, src_related and hash table entry."
 *
 * So the constant is KEPT only when its cost is <= the register's:
 *   COST(subreg lowpart of a pseudo) = 1       (cse.c:725-741 notreg_cost,
 *                                               cse.c:509-514 COST)
 *   COST(const_int)  = rtx_cost (x, SET) * 2   (same notreg_cost)
 *   rtx_cost of a Thumb CONST_INT with outer == SET (arm_rtx_costs,
 *     config/arm/arm.c, TARGET_THUMB branch): 0 if the value is < 256;
 *     COSTS_N_INSNS(2) = 6 if thumb_shiftable_const; else COSTS_N_INSNS(3) = 10.
 *
 *   >> 0x895 is neither < 256 nor shiftable, so COST is 20 against the
 *   >> register's 1 and the substitution is UNCONDITIONAL.  **NO SPELLING OF
 *   >> THE INTEGER 0x895 KEEPS THE TWO USES APART.**  0x95 gives COST 0, the
 *   >> constant wins the tie-break, and the reference's block appears.  That is
 *   >> the whole of this park, and it is a COST BOUND, not a spelling plateau.
 *
 * ---- THE PARK'S OWN "NEXT" IS REFUTED, MEASURED ----
 *
 * The park says: "If a save-bit namespace is ever given a .sym file, re-try
 * this first -- a `_FLAG_895` address would pool separately from the integer
 * and the CSE could not fire."  **IT FIRES.**  Measured with
 * `extern int _FLAG_895;` and `(int)(&_FLAG_895)` in all five places: 27 of 62,
 * and the generated block still reads `ldr r5, <sym> / mov r0, r5 / bl
 * __GetFlag / ... / strh r5, [r3]` -- the symbol is hoisted into the same
 * callee-saved register the integer was.  A symbol_ref's COST is also above 1,
 * so it loses the same comparison.  **A save-bit .sym entry would NOT help this
 * function, and that should not be re-proposed on this evidence.**
 *
 * ---- ONE BODY IS STRUCTURALLY CORRECT AND NUMERICALLY WORSE: KEEP IT ----
 *
 * `q = L4998;` assigned INSIDE the guarded block with a SECOND pointer `p`
 * assigned just before the tail call (instead of the park's single hoisted `p`)
 * reads **26 of 62 at 54 instructions**, but it fixes three things the park
 * body gets wrong and leaves exactly one:
 *     push {r5, lr} / pop {r5}        -- EXACT, one callee-saved register
 *     the whole tail                  -- EXACT (ldr r5 / mov r0,r5 / bl /
 *                                        mov r0,r5)
 *     the in-block base is reloaded   -- as the reference does
 * and the only remaining defect is the shared 0x895 pseudo above.  It is kept
 * as scratch_elev/b341/B/v_p2/v07_two_ptrs.c.  If the cost bound is ever broken
 * -- by a per-file flag that turns off the first cse, or by pass 3 admitting a
 * `:: "i"(0)` asm (cse.c:5742-5745 is the ONLY thing that stops a cse
 * equivalence, per batch 336) -- START FROM THAT BODY, NOT FROM THE PARK'S.
 *
 * MEASURED EXACTLY INERT, crossed with the two-pointer body: `do { } while (0)`
 * as the first in-block statement, before the `if`, after `q = L4998;`, and in
 * both places at once (all 26 -- so **the anchor is not a cse fence**, which
 * agrees with batch 336's table); an int local for the value inside the block.
 * MEASURED WORSE: `int id = 0x895;` initialised AT ITS DECLARATION, the
 * batch-310 rematerialisation lever -- 34 with the park's body, 39 with the
 * two-pointer body.  It makes the pseudo live from function entry as advertised,
 * and it STILL WINS r5, because with only two long-lived values and three
 * callee-saved registers free there is nothing for it to lose the
 * allocno_compare race TO.  That bounds the batch-310 lever: it needs a
 * CONTESTED register, not merely a long range.
 * MEASURED AS A DIFFERENT PROGRAM (recorded so it is not re-found): dropping
 * the pointer local entirely and naming L4998 at each store -- 46 to 55 of 62
 * at 48 instructions and NINE pool words, because gcc folds each offset into a
 * pooled `.L4998+N`.  The named base is required, exactly as the park says.
 *
 * NEXT: nothing source-level remains below the cost bound.  This park is a
 * CLOSED-ON-COST park unless pass 3 admits a cse fence.
 */
extern unsigned char gState[];
extern int _AREA_3c;
extern unsigned char L48f0[] __asm__(".L48f0");
extern unsigned char L4ae8[] __asm__(".L4ae8");
extern unsigned char L4998[] __asm__(".L4998");
extern int __GetFlag(int id);
extern void __Func_808b868(unsigned char *p);

unsigned char *OvlFunc_926_2008388(void)
{
    unsigned char *g;
    unsigned char *p;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_3c))
        return L48f0;
    if (*(short *)(g + (0xe1 << 1)) == 3)
        return L4ae8;
    p = L4998;
    if (__GetFlag(0x895)) {
        *(short *)(p + 0x7a) = 0x895;
        *(short *)(p + 0xaa) = 0x895;
        *(int *)(p + 0xc8) = 0x90 << 17;
        *(int *)(p + 0xd0) = 0xf8 << 16;
        *(short *)(p + (0x85 << 1)) = 0x895;
        *(short *)(p + 0x122) = 0x895;
    }
    __Func_808b868(L4998);
    return L4998;
}
