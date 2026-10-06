/* Func_80ab1f4 @ 0x080ab1f4 -- NON-MATCHING, 4 differing encodings of 19.
 *
 * FIGURE RE-DERIVED batch 327 brief E.  19 instructions both sides, SIZE equal,
 * no objcmp INSTRUCTION COUNT line -- the 4 IS a distance.  PIN COUNT 0.
 *
 * Verify with -- ONE LINE:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/rom_ab1f4.c asm/rom_a1000/rom_aa538_c_c_a_c_c.s --func Func_80ab1f4
 *     -> XX ENCODINGS differ in 4 place(s) (ref 19, ours 19), first at index 9
 *   `--whole` is NOT a figure for this file: the reference TU holds THREE
 *   functions (Func_80aafb8, Func_80ab1f4, Func_80ab21c).  Use --func.
 *
 * SPLIT, for when it lands:
 *   python3 tools/datacheck.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s  -> CLEAN (exit 0)
 *   python3 tools/split_s.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s Func_80ab1f4 --dry-run
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
