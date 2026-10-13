/* Func_80ab1f4 -- park.  The subject line must stay FIRST (piece.py,
 * autosweep.py, shortrefs.py, scratchfill.py read line 1).
 */
/* PARK HOLDS AT **4 of 19**.  Batch 349 brief C -- NEGATIVE-LIST RE-TEST.
 *
 * FOUR NUMBERS, re-derived first: 4 differing encodings of a 19-entry stream
 * (ref 19 / ours 19) = 18 instructions plus one 32-bit entry, size equal, no
 * objcmp INSTRUCTION COUNT line, first differing index 9 (ref 3001, ours
 * 9b04).  The 4 IS a distance.  PINS 0.  `--whole` is NOT a figure here: the
 * reference TU holds three functions.  Use --func.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/rom_ab1f4.c \
 *     asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s --func Func_80ab1f4
 * SPLIT SHAPE (for when it lands, NOT run now): datacheck.py CLEAN;
 * split_s.py --dry-run would write rom_aa538_c_c_a_c_c_{a,b,c}.s and this
 * function is the `_b` (1 function, 26 lines).
 *
 * ===== THE NEGATIVE LIST, RE-TESTED WITH OBJECT FINGERPRINTING =====
 * THE PARK RECORDS **13 SPELLINGS AS INERT AT 4**.  THEY ARE **2 DISTINCT
 * PROGRAMS**.  Eleven rebuilt rows read 4; ten of them are BIT-IDENTICAL TO
 * THE BASE OBJECT:
 *     `s32 last = a6;` named before the call        bit-identical
 *     `s32 d = a4;` named                           bit-identical
 *     `s32 p = a5;` named                           bit-identical
 *     `const struct Window *window`                 bit-identical
 *     both sums named in locals, COL first          bit-identical
 *     pad_00[0x0c] replaced by declared members     bit-identical
 *     the callee with NO prototype                  bit-identical
 *     `a5` cast at the call site                    bit-identical
 *     col/row as a `u16 pos[2]` array               bit-identical
 *     `1 + window->col + x`                         bit-identical
 * The eleventh, `void` on the callee prototype AND our definition together,
 * reads 4 and is **NOT** bit-identical -- it is the one real distinct program
 * in the park's inert list, and the park is right that it is a genuine
 * prerequisite with a dump-visible effect (deps(25) 2 -> 3, the t=11 ready
 * list going `33 39 25 31 12` -> `33 39 31 25 12`).
 * The WORSE rows reproduce exactly as recorded: `void` on the definition only
 * = 6; named sums ROW first = 13 (first index moves to 1); `x + window->col +
 * 1` = 17 with +4 bytes and RELOCDIFF, i.e. a LENGTH defect and so not a
 * distance at all.
 *
 * >> *** A CAUTION ABOUT THE METHOD ITSELF, AND THIS PARK IS THE COUNTEREXAMPLE
 * >> THAT ESTABLISHES IT.  A BIT-IDENTICAL ROW MEANS "DID NOT CHANGE THE
 * >> OUTPUT", NOT "THE COMPILER NEVER SAW A DIFFERENCE." ***  The `void` row
 * >> here is the opposite case -- a row that is NOT bit-identical while being
 * >> inert on the figure, because it emits a DIFFERENT WRONG ORDER at the same
 * >> four indices.  Fingerprinting separates the ten rows that tested nothing
 * >> from the one that tested something and lost; it does not promote
 * >> bit-identity into a statement about passes.  Where a row's argument is
 * >> about an RTL quantity (a dependence count, a ready list), the dump is
 * >> still the authority and bit-identity is not a refutation of it. <<
 *
 * ===== THE BOUND SURVIVES, AND THE RUNG LIST IS NOW CONFIRMED FROM SOURCE =====
 * `rank_for_schedule` read out in full at haifa-sched.c:4028-4116, so the
 * park's rung numbering is checkable in one command and is CORRECT:
 *   :4040-4043  priority         `INSN_PRIORITY(tmp2) - INSN_PRIORITY(tmp)`
 *   :4045-4048  reg pressure     DEAD, gated `!reload_completed`
 *   :4050-4067  interblock       same bb here, skipped
 *   :4069-4096  CLASS vs last_scheduled_insn, three classes
 *   :4098-4110  dependent count  `depend_count2 - depend_count1`
 *   :4113-4115  `return INSN_LUID (tmp) - INSN_LUID (tmp2)` -> LOWER LUID WINS
 * There is NO rung earlier than priority, so nothing is omitted above it, and
 * the sort direction is confirmed: `SCHED_SORT` (haifa-sched.c:4016-4021) runs
 * `qsort` for N_READY > 2 and the scheduler takes the LAST element, which is
 * why the printed ready list's tail is the pick.
 * Rungs 1, 3, 4, 5 and readiness all reproduce as the park records them.
 * deps(25) <= 3 (it is excluded from the epilogue's last-writer set because the
 * call re-writes r0, and from `add sp,#4` because it does not read sp) against
 * deps(12) = 4 ({42 call, 65 add sp, 66 use sp, 67 epilogue}), and LUID(12) <
 * LUID(25) loses the tie as well.  Insn 12 is `ldr r3,[sp,#16]`, the a5 load:
 * a5 is our 5th parameter so it MUST read the frame, and it is the callee's
 * 4th argument so it MUST be r3's last writer -- both halves of deps(12) are
 * fixed by the ROM's own instruction.
 *
 * ONE THING CHECKED THIS BATCH THAT THE PARK DID NOT STATE: the batch-337
 * ASM_OPERANDS lever (an asm with a colon section fences combine --
 * combine.c:1183-1188 via rtlanal.c:1637/1648) cannot reach this park either,
 * because there is nothing for combine to fence.  The four contested insns are
 * `add r0,#1`, `ldr r3,[sp,#16]`, `add r1,#1` and `mov r2,r6`; none is a
 * constant materialisation and none is created by reload.  That was the
 * mechanism that refuted the equivalent "terminal" verdict on Func_80a96d8 in
 * this same brief, so it is recorded here as checked and inapplicable rather
 * than left for a future round to try.
 *
 * Blocker class unchanged: sched2 `rank_for_schedule` rung 4 with a STRUCTURAL
 * CAP on the quantity, plus a rung-5 LUID loss behind it.  Park terminal at 4
 * for any body with the ROM's 19-entry stream.
 *
 * ---------------------------------------------------------------------------
 * THE PRIOR HEADERS FOLLOW IN FULL AND UNCHANGED.
 */
/* Func_80ab1f4 -- park. The subject line must stay FIRST: tools that identify a
 * park by its subject (piece.py, autosweep.py, shortrefs.py, scratchfill.py)
 * read line 1, and a note prepended above it makes them resolve the wrong
 * function or no function at all. Notes go UNDER this line. Added in batch
 * 341 after the pre-fix autosweep was found sweeping wrong subjects.
 */
/* NOTE (batch 336): this park's reference .s was RENAMED by the split that
 * landed Func_80ab21c.  split_s.py consumed rom_aa538_c_c_a_c_c.s; this
 * function now lives in rom_aa538_c_c_a_c_c_a.s with Func_80aafb8, and the
 * recipes below are updated to it.  parkcheck read UNCHECKABLE until then.
 */
/*
 * ========= BATCH 336, BRIEF B: THE READINESS PRECONDITION IS CHECKED ========
 * ========= AND CLOSED STRUCTURALLY.  NO BARRIER PLACEMENT CAN FIT.  =========
 *
 * FIGURE RE-DERIVED: 4 differing encodings of 19 -- a STREAM length of 19 (18
 * instructions plus one 32-bit entry) both sides, no SIZE line, no INSTRUCTION
 * COUNT line, first at index 9 (ref 3001, ours 9b04).  PINS 0 (shimcount reports
 * nothing of either class).  Superseded figures: none; it has been four since
 * batch 326.
 *
 * I was sent to re-check ONE thing: the previous round's readiness closure, on
 * the ground that readiness is the rung that was wrong on Func_80063bc in batch
 * 335.  **The previous round's CONCLUSION survives and its REASON does not.**
 *
 * (1) THE REASON WAS WRONG.  Batch 329 wrote: "delaying 12 past t=11 needs a
 *     producer finishing at t >= 10 with cost >= 2.  Its only producers are 61
 *     (sp, true) and 10 (ANTI, which arm_adjust_cost prices 0)."  That treats the
 *     anti-dependence as irrelevant because its COST is zero.  It is not: an insn
 *     is not ready until every predecessor has been SCHEDULED, whatever the cost.
 *     My own `.23.sched2` says so in one line --
 *         ;;  Ready list (t = 9): 33 25 29 10
 *         ;;      --> scheduling insn <<<10>>> on unit core
 *         ;;      dependences resolved: insn 12 into ready
 *     -- so insn 12's readiness at t=10 is set ENTIRELY by the zero-cost anti-dep
 *     on insn 10, and `arm.c:2414-2418` (REG_DEP_ANTI / REG_DEP_OUTPUT -> 0) only
 *     says it adds no queue delay on top.  **readiness(12) = max(sched(61) + 1,
 *     sched(10) + 1)**, and the ROM pins both: insn 61 (`sub sp,#4`) is its index
 *     5 and insn 10 (`mov r6,r3`) its index 7, both already matching.
 *
 * (2) AND THE CONCLUSION IS NOW STRONGER THAN "no producer is available": THE
 *     BARRIER LEVER IS EXCLUDED BY A COUNTING ARGUMENT, not by a search.
 *     A bare `__asm__ volatile ("")` is a TWO-SIDED barrier -- ASM_INPUT depends
 *     on every earlier insn and `reg_pending_sets_all` makes every later insn
 *     depend on it (`haifa-sched.c:3567-3598`, `:3780-3789`).  So a barrier
 *     PARTITIONS the block: the schedule must place every chain-predecessor of
 *     the barrier before every chain-successor.  **The ROM's order therefore has
 *     to be a PREFIX CUT of the `.19.flow2` chain order, and it is not, at any
 *     position.**
 *         chain order  61  4 10 12 14 21 23 25 27 29 31 33 39 42
 *         ROM order     4 21 23 27 61 14 10 29 25 31 39 12 33 42
 *     Every non-empty chain prefix contains insn 61, which sits at ROM position
 *     5, so a viable cut would have to be a ROM prefix of length >= 5; the chain
 *     prefix of length 5 is {61,4,10,12,14} and the ROM prefix of length 5 is
 *     {4,21,23,27,61}.  No two prefixes of equal length agree, at any length.
 *     Measured, which is the point:
 *         barrier at the top of the body ........................ 12 of 19
 *         barrier after the first sum (`cx` named) .............. 7 of 19
 *         barrier after both sums (`cx`, `cy` named) ............ 4 of 19, but
 *                                 FIRST AT INDEX 6, not 9 -- a different residue
 *         `void` + each of those three ......................... 12 / 9 / 6
 *         `void` + `s32 p = a5;` named + barrier ............... 12
 *     The one that keeps the figure at 4 moves the first difference EARLIER, so
 *     it is not a near miss; nothing here is a prerequisite either.
 *
 * (3) `void` ON PROTOTYPE AND DEFINITION REPRODUCES AS EXACTLY INERT at 4 of 19,
 *     19 of 19, confirming batch 327's banked prerequisite and batch 329's proof
 *     that it is insufficient (it raises deps(25) from 2 to 3 against the bar of
 *     4 and loses the 4-4 tie on LUID anyway).
 *
 * SO THE WHOLE OF `rank_for_schedule` IS NOW CLOSED WITH READINESS INCLUDED, and
 * the readiness rung is closed by the SHAPE OF THE CHAIN rather than by a list of
 * things that did not work.  Any future attempt needs a lever that changes the
 * `.19.flow2` CHAIN ORDER itself -- insns 4, 10, 12, 14 are the parameter copies
 * `assign_parms` emits ahead of NOTE_INSN_FUNCTION_BEG, so nothing written in the
 * body can precede the a5 load, which is also why LUID(12) < LUID(25) is not
 * addressable from the source.
 *
 * Blocker class unchanged: sched2 `rank_for_schedule` rung 4 with a structural cap
 * on the quantity -- now plus a structural exclusion of the readiness escape.
 * Park terminal at 4.
 *
 Func_80ab1f4 @ 0x080ab1f4 -- NON-MATCHING, 4 differing encodings of 19.
 *
 * FIGURE RE-DERIVED batch 327 brief E.  19 instructions both sides, SIZE equal,
 * no objcmp INSTRUCTION COUNT line -- the 4 IS a distance.  PIN COUNT 0.
 *
 * Verify with -- ONE LINE:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/rom_ab1f4.c asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s --func Func_80ab1f4
 *     -> XX ENCODINGS differ in 4 place(s) (ref 19, ours 19), first at index 9
 *   `--whole` is NOT a figure for this file: the reference TU holds THREE
 *   functions (Func_80aafb8, Func_80ab1f4, Func_80ab21c).  Use --func.
 *
 * SPLIT, for when it lands:
 *   python3 tools/datacheck.py asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s  -> CLEAN (exit 0)
 *   python3 tools/split_s.py asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s Func_80ab1f4 --dry-run
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s  (1 function, 283 lines)
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_b.s  (1 function,  26 lines) <- this one
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_c.s  (1 function, 114 lines)
 *
 * ================================================================
 * THE FULL INSN -> INSTRUCTION MAP.  The batch-326 table was INCOMPLETE: it
 * omitted insns 61, 14, 27, 46, 54, 65, 66 and 67, and insn 61 is load-bearing.
 * ================================================================
 *   idx 0  push {r5,r6,lr}          idx 10  OURS 31  | ROM 12
 *   idx 1  mov r4,r0        = 4     idx 11  OURS 39  | ROM 31
 *   idx 2  ldrh r0,[r4,#12] = 21    idx 12  OURS 25  | ROM 39
 *   idx 3  add r0,r0,r1     = 23    idx 13  str r5,[sp]      = 33
 *   idx 4  ldrh r1,[r4,#14] = 27    idx 14  bl _Func_8022768 = 42
 *   idx 5  sub sp,#4        = 61    idx 15  add sp,#4        = 65
 *   idx 6  ldr r5,[sp,#20]  = 14    idx 16  pop {r5,r6}
 *   idx 7  mov r6,r3        = 10    idx 17  pop {r1}
 *   idx 8  add r1,r1,r2     = 29    idx 18  bx r1            = 67
 *   idx 9  OURS 12 | ROM 25
 * where 25 = `add r0,#1`, 31 = `add r1,#1`, 39 = `mov r2,r6`,
 *       12 = `ldr r3,[sp,#16]` (the a5 load), 14 = `ldr r5,[sp,#20]` (a6).
 *
 * *** THE RESIDUE IS A ROTATION, NOT A SWAP: `add r0,#1` moves from LAST to
 * *** FIRST of four.  INDICES 0-8 AND 13-18 ALREADY AGREE WITH THE ROM.
 *
 * ================================================================
 * BATCH 326'S "SHARPER TARGET" IS REFUTED.  IT NAMES TWO MATCHING ENCODINGS.
 * ================================================================
 * It reads: "our schedule picks 10 at t=9 and 29 at t=10; SWAPPING THOSE TWO
 * PICKS IS THE WHOLE LANDING ... what is needed is ONE MORE DEPENDENT ON INSN 29,
 * OR ONE FEWER ON INSN 10 -- not two more on insn 25."
 *
 * Insn 10 IS `mov r6,r3` at index 7 and insn 29 IS `add r1,r1,r2` at index 8,
 * **and the ROM has them in exactly that order.**  Swapping them converts two
 * CORRECT encodings into two wrong ones.  This is the brief's own warning --
 * check that a matching encoding matches for the right reason -- firing in the
 * opposite direction: here two encodings are right and a propagated target asks
 * for them to be broken.  (It is also why "one more dependent on insn 29" would
 * not help even if it were free.)
 *
 * ================================================================
 * THE CONTEST IS ENTIRELY AT t=11, AND IT IS THREE-WAY
 * ================================================================
 * `.23.sched2`, block 0 (`from 55 to 67`), dependence table in CHAIN order:
 *   insn  prio  cost  dependents
 *     61    68    1   66 65 42 33 14 12      <- sub sp,#4  (reload frame adjust)
 *      4    72    1   67 66 42 27 21
 *     10    67    1   67 66 39 12            <- mov r6,r3
 *     12    66    2   67 66 65 42      = 4   <- ldr r3,[sp,#16]  (a5)
 *     14    67    2   67 66 65 42 33
 *     21    71    2   67 66 42 23
 *     23    69    1   66 27 25
 *     25    66    1   66 42            = 2   <- add r0,#1
 *     27    69    2   67 66 42 29
 *     29    67    1   66 39 31         = 3   <- add r1,r1,r2
 *     31    66    1   67 66 42         = 3   <- add r1,#1
 *     33    65    2   67 66 65 42
 *     39    66    1   67 66 42         = 3   <- mov r2,r6
 *     42    65   32   67 66 65 54 46         <- the call
 *   t=9  Ready: 33 25 29 10      -> 10  (prio 67 ties 29; rung 4 deps 4 > 3)
 *   t=10 Ready: 33 12 25 29      -> 29  (prio 67, rung 1 outright)
 *   t=11 Ready: 33 39 25 31 12   -> 12  *** THE ONLY CONTEST THAT MATTERS ***
 *
 * THE t=11 RANKING, FULLY RECONSTRUCTED -- it reproduces the printed order exactly:
 *   rung 1 priority: 33 = 65 (worst); 39, 25, 31, 12 all = 66.            TIES
 *   rung 3 CLASS, last_scheduled_insn = 29 (dependents {66,39,31}):
 *     39: REG_DEP_ANTI from 29 (29 READS r2, 39 WRITES r2); arm_adjust_cost
 *         prices anti at 0, so insn_cost != 1 => **class 2, DEMOTED**
 *     31: link from 29 with cost 1 ("insn 31 into queue with cost=1"), and
 *         `:4077` makes `insn_cost(...) == 1` class 3 => **class 3**
 *     25, 12: no link => class 3
 *   rung 4 dependent count among the class-3 group: 12 = 4, 31 = 3, 25 = 2.
 *         MORE WINS -> 12.   Printed order `33 39 25 31 12` matches exactly.
 *
 * ================================================================
 * THE REPLACEMENT TARGET, AND THE PREREQUISITE THAT IS NOW BANKED
 * ================================================================
 * Because `:4115` returns `INSN_LUID(tmp) - INSN_LUID(tmp2)` the LOWER LUID WINS,
 * and the chain order above gives LUID(25) < LUID(31) < LUID(39).  So
 * **deps(25) = 3 already beats BOTH 31 and 39 on rung 5** -- the gap to those two
 * is ONE dependent, not two.
 *
 * AND THE `void` RETURN TYPE SUPPLIES EXACTLY IT, MEASURED, WITH THE DUMP AS
 * EVIDENCE.  Declaring the prototype AND the definition `void` together:
 *   figure 4, 19 of 19, SIZE equal -- EXACTLY INERT
 *   deps(25) goes {57,42} -> {58,57,42}, i.e. **2 -> 3**
 *   the t=11 ready list goes `33 39 25 31 12` -> `33 39 31 25 12`, i.e.
 *   **insn 25 OVERTAKES insn 31**, exactly as rung 5 predicts, output unchanged.
 * This is batch 326's "a tie-break lever is invisible until something else
 * manufactures the tie", observed in the dump rather than inferred.  It is a
 * DEMONSTRATED prerequisite, not a candidate one.
 *
 * ALSO CORRECTED: both the park and batch 326 assert the `void` spelling "costs
 * the epilogue" because `pop {r1} / bx r1` proves a non-void return.  **ON THIS
 * BODY IT COSTS NOTHING** -- indices 17-18 still match, because the callee
 * clobbers r0 either way, so thumb_exit emits the same pop.  Neither measured it.
 * (The body below nonetheless KEEPS `s32`, because the function really does
 * forward its callee's result and `void` buys no encoding.  The figure is
 * recorded here so the next round does not re-measure it.)
 *
 * > WHAT IS STILL MISSING, stated with its evidence.  Insn 12 must be DEMOTED on
 * > rung 3 at t=11, and `last_scheduled_insn` must therefore be one of insn 12's
 * > only two producers:
 * >     insn 10 = `mov r6,r3` (index 7) -- ANTI dep: 10 reads r3, 12 writes it
 * >     insn 61 = `sub sp,#4` (index 5) -- 12 reads sp
 * > **Both already sit at indices that MATCH the ROM**, so moving either into the
 * > t=10 slot trades correct encodings for the 4.  There is no rung-4/5 route
 * > either: LUID(12) < LUID(25) in the chain order above, so ties go to 12, and
 * > deps(12) would have to fall to <= 2.  Its 4 is {67,66,65,42}, of which 65
 * > (`add sp,#4`) and 67 (`pop {r1}/bx r1`) are anti-dependences insn 12 picks up
 * > MERELY BY READING THE FRAME -- which a5 must do, being the 5th parameter.
 * > So the open question is whether a5 can be made to arrive through a register
 * > that the ROW SUM writes, NOT whether insn 25 can gain dependents (it can gain
 * > exactly one, and one is not enough).
 *
 * MEASURED THIS BATCH (all 19 of 19, SIZE equal, no COUNT line):
 *   base ........................................................ 4
 *   `void` on PROTOTYPE AND DEFINITION together ................. 4  inert, and
 *                                      it moves the ready list (see above)
 *   `void` on the DEFINITION ONLY (prototype left s32) .......... 6  WORSE
 *   `s32 last = a6;` named before the call ...................... 4  exactly inert
 *   `s32 d = a4;` named before the call ......................... 4  exactly inert
 *   `const struct Window *window` ............................... 4  exactly inert
 *   both sums named in locals (`cx`, `cy`) ...................... 4  exactly inert
 *   all pairs and triples of those (crossfire, depth 3) ......... 4  exactly inert
 * Inherited and not re-run: pad_00[0x0c] replaced by real declared members 4;
 *   callee without a prototype 4; a5 cast at the call 4; col/row as a `u16 pos[2]`
 *   array 4; `p = a5;` / `q = a6;` / `p = a4;` named 4; `1 + window->col + x` 4;
 *   named sums ROW first 13 WORSE; `x + window->col + 1` SIZE 44 vs 40 at 20
 *   insns -- a LENGTH defect, so its 17 is misalignment, not distance;
 *   -fno-schedule-insns2 REJECTED at 12 of 19.
 *
 * Reproduced and still closed: the epilogue blocker.  `pop {r1} / bx r1` means r0
 * is live at exit and the declared return type is what fixes it.
 *
 * Blocker class: sched2 rank_for_schedule RUNG 4 (dependent count) at t=11, with
 * the rung-3 escape requiring a reorder of encodings that already match.
 * DECLINING TO CLOSE: figure unchanged at 4, with one prerequisite banked and
 * batch 326's target retired.
 *
 * ========= BATCH 329, BRIEF C: CLOSED.  deps(insn 25) HAS A HARD CAP OF 3. =======
 *
 * FIGURE RE-DERIVED: 4 differing encodings of 19 (ref 19, ours 19), SIZE equal, no
 * INSTRUCTION COUNT line, first at index 9 (ref 3001, ours 9b04).  PINS 0.
 *
 * A TRAP TO RECORD FIRST, because I nearly wrote a false correction out of it:
 * `.23.sched2`'s RTL LISTING IS THE POST-SCHEDULE ORDER, not the chain order.
 * Read in it, the order looks like 29, 12, 31, 39, 25 and the park's rung-5
 * claim looks inverted.  **LUIDs come from `.19.flow2`**, which gives the real
 * pre-sched chain 61, 4, 10, 12, 14, 21, 23, 25, 27, 29, 31, 33, 39, 42, 46, 54,
 * 65, 66, 67 -- so LUID(12) < LUID(25) < LUID(31) < LUID(39), and the park's
 * rung-5 reading is CORRECT as written.
 *
 * THE DEPENDENT COUNTS DECOMPOSE, and that is what closes the function.  Every
 * dependent in block 0 comes from exactly one of four sources:
 *   + insn 42, the call .......... true; every argument insn has it
 *   + insn 66, `(use (reg sp))` .. ANTI, and it links to EVERY insn in the block,
 *                                  so it is a free +1 for all of them
 *   + insn 67, `*epilogue_insns` . an `unspec_volatile`, and its TRUE links are
 *                                  precisely the LAST WRITER OF EVERY HARD
 *                                  REGISTER: 10=r6, 14=r5, 4=r4, 12=r3, 39=r2,
 *                                  31=r1, 42=r0, 65=sp
 *   + insn 65, `add sp,#4` ....... ANTI, only for insns that READ sp
 * giving deps(12) = 4 {42,65,66,67}, deps(31) = deps(39) = 3, deps(25) = 2 {42,66}.
 *
 * >>> INSN 25 (`add r0,#1`) IS EXCLUDED FROM 67 PRECISELY BECAUSE THE CALL
 * >>> RE-WRITES r0, and from 65 because it does not read sp.  So deps(25) can gain
 * >>> AT MOST ONE, and only by removing the call's set of r0.
 *
 * THE CONTROL THAT PROVES IT, measured here and not inherited.  The park banked
 * `void` on prototype and definition as a demonstrated prerequisite; it is real,
 * and it is now also PROVED INSUFFICIENT.  The void body reads 4 of 19 (19 of 19,
 * SIZE equal) -- inert -- and its own `.23.sched2` shows WHY it moved the ready
 * list: the epilogue insn gains a TRUE link to insn 25 that the base build does
 * not have, because with the result discarded the call no longer sets r0 and insn
 * 25 becomes r0's last writer.  deps(25) goes 2 -> 3 and the t=11 ready list goes
 * `33 39 25 31 12` -> `33 39 31 25 12`, i.e. 25 overtakes 31 on rung 5 exactly as
 * the park predicted.  It still loses to 12's 4, and a 4-4 tie would also lose
 * because LUID(12) < LUID(25).  **3 is the ceiling and 4 is the bar.**
 *
 * EVERY ROUTE, WITH ITS CLOSURE.  ROM order in the window is 25, 12, 31, 39, 33;
 * ours is 12, 31, 39, 25, 33; the single decision is t=11, ready `33 39 25 31 12`.
 *   rung 1  all of 12/25/31/39 sit at prio 66 = prio(call) + 1, because each feeds
 *           only the call and `arm_adjust_cost` returns 1 for a CALL_INSN consumer
 *           and never raises.  33 is the only one that loses here.  CLOSED.
 *   rung 3  12 would have to carry a link from 29 (`add r1,r1,r2`) with cost != 1.
 *           29 touches r1/r2; 12 touches r3/sp.  And demoting 12 ALONE hands t=11
 *           to 31, not to 25 -- 31 and 12 would BOTH have to fall, and 31's link
 *           from 29 is a true dependence priced at exactly 1, which `:4077` makes
 *           class 3.  CLOSED.
 *   rung 4  deps(25) <= 3 < 4 = deps(12), per the cap above.  CLOSED.
 *   rung 5  LUID(12) < LUID(25), so every tie goes to 12.  CLOSED.
 *   readiness  delaying 12 past t=11 needs a producer finishing at t >= 10 with
 *           cost >= 2.  Its only producers are 61 (sp, true) and 10 (ANTI, which
 *           `arm_adjust_cost` prices 0).  A memory dependence would need a STORE
 *           ahead of the a5 load, and the only store in the function is the
 *           outgoing-argument `str r5,[sp]`, which is after it.  CLOSED.
 *   deps(12) <= 2  needs a5 neither to read sp nor to be r3's last writer at the
 *           epilogue.  a5 is the 5th parameter (so it arrives on the frame) and
 *           callee argument 4 (so it arrives in r3).  Both are fixed by the ROM's
 *           own `ldr r3,[sp,#16]`.  CLOSED.
 *
 * VERDICT: 4 of 19 is NOT REACHABLE through `rank_for_schedule` for any body with
 * the ROM's 19-instruction stream.  Batch 326's target was retired by batch 327;
 * the replacement target banked by batch 327 is retired here.  The park is
 * terminal at 4 unless the 19-instruction stream itself can be reached another
 * way, which `-fno-schedule-insns2` (12 of 19) does not do.
 *
 * Blocker class: sched2 `rank_for_schedule` rung 4, with a STRUCTURAL CAP on the
 * quantity -- not an unexplored tie-break.
 */
#include "gba/types.h"

struct Window {
    u8 pad_00[0x0c];
    u16 col;
    u16 row;
};

extern s32 _Func_8022768(s32 x, s32 y, s32 a3, s32 a4, s32 a5);

/* Window-relative wrapper: adds the window's own column and row, plus one for
 * the border, forwards everything else unchanged, and returns the callee's
 * result.
 */
s32 Func_80ab1f4(struct Window *window, s32 x, s32 y, s32 a4, s32 a5, s32 a6)
{
    return _Func_8022768(window->col + x + 1, window->row + y + 1, a4, a5, a6);
}
