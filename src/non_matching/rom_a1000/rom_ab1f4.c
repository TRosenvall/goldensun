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
 * DECLINING TO CLOSE.  Figure unchanged at 4; the diagnosis is now confirmed
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
