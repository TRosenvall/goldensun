/* Func_80ab1f4 @ 0x080ab1f4  --  NOT MATCHING, 4 of 19 encodings
 *
 * MEASURED THIS BATCH.  PIN COUNT: 0 (tools/shimcount.py reports no shims).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/rom_ab1f4.c \
 *     asm/rom_a1000/rom_aa538_c_c_a_c_c.s --func Func_80ab1f4
 *
 *   --func : XX ENCODINGS differ in 4 place(s) (ref 19, ours 19), first at index 9
 *   --whole is NOT a figure for this file: the reference TU holds THREE
 *   functions (Func_80aafb8, Func_80ab1f4, Func_80ab21c), so --whole reports
 *   the other two "missing from candidate" plus a SIZE and RELOCATIONS diff.
 *   Use --func until the split below is taken.
 *
 * SPLIT (for when it lands).  CORRECTED -- the park's split filenames were
 * computed against the OLD reference name and are stale:
 *   python3 tools/datacheck.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s  -> CLEAN (exit 0)
 *   python3 tools/split_s.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s Func_80ab1f4 --dry-run
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_a.s  (1 function, 283 lines)
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_b.s  (1 function,  26 lines)  <- this one
 *     would write asm/rom_a1000/rom_aa538_c_c_a_c_c_c.s  (1 function, 114 lines)
 *
 * ===== WHAT I REPRODUCED AND WHAT I ADD =====
 *
 * REPRODUCED, both of the park's findings.  The epilogue blocker is closed and
 * stays closed: `pop {r1} / bx r1` means r0 is live at exit, the function
 * returns its callee's result, and the declared return type is what fixes it.
 * The remaining residue is one sched2 rung, 19 instructions against 19:
 *
 *     idx  REF                      | OURS
 *      9   3001 add r0, #1          | 9b04 ldr r3, [sp, #16]
 *     10   9b04 ldr r3, [sp, #16]   | 3101 add r1, #1
 *     11   3101 add r1, #1          | 1c32 mov r2, r6
 *     12   1c32 mov r2, r6          | 3001 add r0, #1
 *
 * ALL FOUR ARE REAL INSTRUCTIONS.  This function has NO literal pool at all
 * (its one relocation is the `bl` at index 14), so no rung below is blind.
 *
 * ADDED (1): the park's numbers are confirmed from the dump rather than
 * inherited.  .23.sched2's own ready lists for block 0:
 *     t=11:  33  39  25  31  12   -> picks 12   (the a5 load)
 *     t=12:  33  25  39  31       -> picks 31
 *     t=14:  33  25  39           -> picks 39
 *     t=15:  33  25               -> picks 25   (`add r0,#1`, LAST)
 * The rank order is 33 < 25 < 39 < 31 < 12.  `add r0,#1` is second-lowest of
 * the five, above only the outgoing-stack-argument store.  The ladder is
 * priority (all tie at 66; arm_adjust_cost charges 1 for any link into a
 * CALL_INSN, so the load gets no latency credit) -> dependent count
 * (12:4, 31:3, 39:3, 25:2) -> INSN_LUID.  Our order IS the dependent-count
 * order.  Frame offsets were already confirmed correct and are not re-derived.
 *
 * ADDED (2): THE TWO BLOCKERS ARE COUPLED, AND THE COUPLING IS NOT THE BINDING
 * CONSTRAINT -- WHICH IS WORTH SAYING BECAUSE IT CLOSES A TEMPTING DEAD END.
 * `add r0,#1` has only 2 dependents because r0 is the return value, so the
 * epilogue's `pop {r1}` gives it no output dependence.  The `void` spelling
 * does give it one -- and costs the epilogue, as the park says.  But even with
 * that third dependent it is 3 against the a5 load's 4, so the void spelling
 * CANNOT win the rung either.  The gap is not one dependent; it is two.
 *
 * ADDED (3): the corpus says what the only reachable route is.  Searching the
 * GENERATED assembly of matching functions:
 *     `add rX, rX, #1` immediately before `ldr rY, [sp, #N]`   ->  0 hits
 *     `ldr rY, [sp, #N]` immediately before `add rX, rX, #1`   -> 34 hits
 *     any `add rX, rX, #imm` before `ldr r3, [sp, #N]`         ->  1 hit
 * The single hit is src/overlays/rom_7d0e88/ovl_1528_a_a_c_a_c_c.c:
 *     add r5, r5, #12 / ldr r3, [sp, #4] / add r5, r5, sl / ... / mov r3, r5 / bl
 * It beats the sp load on the PRIORITY rung, not the dependent-count rung,
 * because its value is three insns from the call instead of one.
 *
 * > BOUND, with its evidence attached.  To beat the a5 load, `add r0,#1` needs
 * > either >= 4 dependents or a longer chain to the call.  Its two extra
 * > dependents would have to come from later writers of r0, and r0 is the
 * > return value (that is what closed the epilogue).  A longer chain needs an
 * > extra instruction, and the stream is already 19 of 19 -- and the two +1s
 * > cannot be moved onto the operands instead of the sums, because the ROM's
 * > encodings are `add r0,#1` (3001) and `add r1,#1` (3101), i.e. on the sums
 * > in r0 and r1, not on x in r1 and y in r2 (which would be 3101/3201).
 * > This is what I measured on this body; it is NOT a claim that the rung is
 * > unreachable in general -- the corpus hit above shows the priority route
 * > works where an extra chain insn exists.
 *
 * ===== MEASURED, BUILDING ON THE PRIOR CROSSFIRE RUN, NOT REPEATING IT =====
 *
 * The prior run's "named sums" row measured exactly inert at 4 and was
 * therefore a candidate prerequisite, so this round CROSSED it with the
 * declaration dimension the brief asked for.  crossfire.py, 5 edits at depth 3
 * (26 live subsets): COMPLETELY FLAT.  Every subset exactly inert at 4, same
 * instruction count, clean relocations, nothing better and nothing worse:
 *     named sums (the prior prerequisite)                          4
 *     pad_00[0x0c] replaced by real declared members               4
 *     callee without a prototype                                   4
 *     window declared const                                         4
 *     a5 cast at the call                                           4
 *     ALL 21 pairs and triples of those, named sums included        4
 * Probed separately: col/row as a `u16 pos[2]` array -- 4, inert.
 * -fno-schedule-insns2 is REJECTED: 12 of 19, first at index 1.
 *
 * ON THE BRIEF'S DECLARATION LEVER, which was the reason to look here: the
 * `u8 pad_00[0x0c]` padding is NOT hiding the defect.  Replacing it with real
 * members is exactly inert, and so is turning col/row into an array -- because
 * THE ADDRESSING IS ALREADY THE ROM'S (`ldrh r0,[r4,#12]`, `ldrh r1,[r4,#14]`)
 * and 19 of 19 instructions already agree.  A correct declaration here can
 * only move the SCHEDULE, and the schedule depends on the successor graph,
 * which these edits leave bit-identical.  The lever class is real -- it already
 * paid out on this function once, as the `void` return type -- but it is spent.
 * The padding is left as padding rather than filled with invented members:
 * inventing f00/f04/f08/f0a would be a guess, and it buys nothing measurable.
 *
 * ===== BATCH 326, BRIEF C: THE PARK'S BOUND IS REFUTED FROM SOURCE =====
 *
 * The park's bound reads: "To beat the a5 load, `add r0,#1` needs either >= 4
 * dependents or a longer chain to the call."  That is computed on the
 * DEPENDENT-COUNT rung, and it assumes the ladder is
 * priority -> dependent count -> INSN_LUID.  **THE LADDER HAS FOUR RUNGS, NOT
 * THREE.**  `rank_for_schedule` (haifa-sched.c:4029-4116), in order:
 *
 *   1. `:4041`  INSN_PRIORITY.
 *   2. `:4046`  INSN_REG_WEIGHT -- gated `!reload_completed`, so DEAD in sched2.
 *   3. `:4069-4095`  THE `last_scheduled_insn` CLASS RUNG.  Each ready insn is
 *      classified 3 (independent of the last-scheduled insn, OR joined to it by a
 *      link of cost 1), 1 (data-dependent) or 2 (anti/output-dependent), and the
 *      HIGHEST class wins outright.
 *   4. `:4100-4110`  dependent count, then `:4115` INSN_LUID.
 *
 * RUNG 3 SITS ABOVE THE DEPENDENT COUNT, SO THE PARK'S BOUND DOES NOT BIND.
 * And the dump shows rung 3 deciding this very contest.  With `-da
 * -fsched-verbose=6`, insn 12 (the a5 load) appears in two consecutive sorted
 * ready lists with NO change to its priority (66) and NO change to its dependent
 * count (4):
 *
 *     Ready list (t = 10):   33  12  25  29      -> picks 29   (12 is 2nd WORST)
 *     Ready list (t = 11):   33  39  25  31  12  -> picks 12   (12 is BEST)
 *
 * The only thing that changed is `last_scheduled_insn`: 10 at t=10, 29 at t=11.
 * Insn 12's two producers are 10 and 61, so at t=10 it is a dependent of the
 * just-scheduled insn and demoted, and at t=11 it is independent and promoted.
 * **A rank that moves from second-worst to best between consecutive cycles cannot
 * be bounded by a dependent-count argument.**
 *
 * THE DEPENDENCE TABLE, so nobody re-derives it (`.23.sched2`, block 0):
 *
 *     insn  prio  cost  dependents
 *       4    72    1    67 66 42 27 21
 *      10    67    1    67 66 39 12
 *      12    66    2    67 66 65 42        <- the a5 load, 4 dependents
 *      21    71    2    67 66 42 23
 *      23    69    1    66 27 25
 *      25    66    1    66 42              <- `add r0,#1`, 2 dependents
 *      29    67    1    66 39 31
 *      31    66    1    67 66 42
 *      33    65    2    67 66 65 42
 *      39    66    1    67 66 42
 *      42    65   32    67 66 65 54 46     <- the call
 *
 * WHY THE a5 LOAD'S 4 IS STRUCTURAL -- which STRENGTHENS the park on rung 4 even
 * as rung 3 retires its bound.  Insn 12's dependents are the call (42), the use
 * note (66), the epilogue's `add sp` (65) and the return (67).  The last two are
 * ANTI-DEPENDENCES IT PICKS UP MERELY BY READING THE FRAME: 65 writes sp and 67
 * reads it.  So EVERY incoming stack-argument load in this function has >= 4
 * dependents and every register-to-register add has 2, whatever the source says.
 * a5 and a6 are the 5th and 6th parameters and must come off the stack, so rung 4
 * can never be won here.  Rung 3 is the only way in.
 *
 * > THE SHARPER TARGET, replacing the park's.  Insn 25 wins at t=11 IF AND ONLY IF
 * > insn 10 -- one of insn 12's two producers -- is the insn scheduled at t=10.
 * > Our schedule picks 10 at t=9 and 29 at t=10; SWAPPING THOSE TWO PICKS IS THE
 * > WHOLE LANDING.  The t=9 contest is 10 against 29, both priority 67, both class
 * > 3, decided on rung 4 by dependent count 4 against 3.  So what is needed is
 * > ONE MORE DEPENDENT ON INSN 29, OR ONE FEWER ON INSN 10 -- not two more on insn
 * > 25.  That is a different and much softer target, and it is stated here with
 * > the dump lines above as its evidence rather than as a claim to build on.
 *
 * MEASURED THIS BATCH, 8 more bodies, screening the dependence graph rather than
 * the declarations (the park had already measured the declaration dimension shut):
 *     named sums, col first then row                              4   inert
 *     `p = a5;` named before the call                             4   inert
 *     `q = a6;` named before the call                             4   inert
 *     both a5 and a6 named                                        4   inert
 *     `p = a4;` named before the call                             4   inert
 *     `1 + window->col + x` / `1 + window->row + y`               4   inert
 *     named sums, ROW first then col                             13   WORSE
 *     `x + window->col + 1` / `y + window->row + 1`     SIZE 44 vs 40, 20 insns
 *                                                        against 19 -- a LENGTH
 *                                                        defect, so 17 is
 *                                                        misalignment, not distance
 * None of the six inert bodies moved the sched2 ready lists at all, which is the
 * park's own conclusion holding: these edits leave the successor graph identical.
 * The ROW-first ordering is worse because it swaps the register assignment of the
 * two sums (first difference at index 1, `1c04` against `1c0d`).
 *
 * DECLINING TO CLOSE.  Figure unchanged at 4 (re-measured batch 326: ref 19,
 * ours 19, SIZE equal, no INSTRUCTION COUNT line -- the 4 IS a distance).  The
 * park's BOUND is retired and replaced by the sharper target above; its
 * diagnosis is confirmed
 * from the dump, the two blockers are shown to be coupled, the gap is sized at
 * two dependents rather than one, the only corpus-attested escape route is
 * named, and the declaration dimension is measured shut.
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
