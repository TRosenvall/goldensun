/* OvlFunc_881_2009c08 -- asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a.s
 *
 * BLOCKER: CONSTANT CSE, STRAIGHT LINE -- same as OvlFunc_882_200bc48, twice
 *
 * 34 of 49 differing, ours 52 lines (three too many).  21 calls, zero labels.
 * `__ClearFlag(0x16f)` and `__ClearFlag(0x171)`/`__SetFlag(0x171)` each appear
 * at two sites; the ROM reloads `ldr r0, =0x16f` and `ldr r0, =0x171` from the
 * pool at every site, gcc hoists both into r5 and r6.
 *
 * TWO THINGS MEASURED HERE THAT ARE NOT IN THE DOC:
 *
 *  1. gcc hoists a POOL LOAD, not just a multi-instruction build.  I expected
 *     it not to -- a pool load costs one instruction and so does the `mov` that
 *     replaces it, so there is nothing to gain.  It hoists anyway.  That means
 *     the "expensive constant" heuristic in tools/script_candidates.py must
 *     count `ldr rN, =V` as well as `mov`+`lsl`, and it does.
 *  2. THE SYMBOL-ADDRESS TECHNIQUE DOES NOT DEFEAT IT.  Adding
 *     `_CONST_16f = 0x16f;` / `_CONST_171 = 0x171;` to a bind-mounted copy of
 *     const.sym and spelling the arguments `(int)&_CONST_16f` leaves the line
 *     count at 52 -- gcc CSEs the symbol address exactly as it CSEs the
 *     integer.  The doc's rule that "two DISTINCT symbols of equal value
 *     reload" is about two different symbols; ONE symbol used twice is hoisted
 *     like anything else, and this is the measurement that separates the two.
 *
 * Best C is scratch/p9c08.c; the symbol variant is scratch/p9c08b.c.
 */

/* ---- MERGED from src/non_matching/overlays/2009c08.c ----
 * That file was a second park for the same function, written later under the
 * src/non_matching/overlays/ naming while this one already existed.  Its
 * analysis is kept verbatim below; the duplicate file is removed.
 *
 OvlFunc_881_2009c08 -- 0x02009c08, asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a.s
 *
 * 49 ROM lines against 52 of ours, 34 differing -- one decision, and the three
 * extra instructions locate it exactly.  Candidate: scratch/L9c08.c.
 *
 * The commoned-constant tell in its textbook form: ours emits
 * `push {r5, r6, r14}` where the ROM pushes only lr, hoists the two pooled flag
 * ids 0x16f and 0x171 into r6 and r5, and then passes `mov r0, r6` / `mov r0, r5`
 * at the four flag calls.  The ROM reloads `ldr r0, =0x16f` at each use.
 *
 * THIS IS THE COUNTEREXAMPLE TO THE TWO-REMEDY RULE.  docs/elevation.md records
 * that this tell is fixed either by CSE_CFLAGS or by separate named locals, and
 * says to try both before concluding anything.  Here BOTH fail, and so does
 * every other flag group in the tree:
 *
 *   CSE_CFLAGS                34   named locals (four, one per use)   34
 *   GCSE                      34   named locals + CSE                 34
 *   ALIAS                     34   named locals + GCSE                34
 *   STRENGTH                  34   SCHED2                             35
 *   FIXEDR7                   34   O1                                 35
 *
 * The docs offered a guess that the flag-group cases are flag ids crossing a
 * BRANCH while the named-local cases are constants reused within one block.
 * This function is the second kind -- br == 0, all four flag calls in a single
 * straight-line block -- and the named-local remedy still does not take, so
 * that guess does not survive as stated.  Recorded rather than patched over.
 */
