/* Func_80ab1f4 @ 0x080ab1f4  --  NOT MATCHING, 4 of 19
 *
 * FIGURE: 4 of 19 encodings (ref 19, ours 19, relocations ok), first diff at
 * index 9.  Measured, not inherited.  The park body measures the SAME 4 -- but
 * on two DIFFERENT differences, and one of them was a blocker the park never
 * saw.  This body is strictly better content at the same count.
 *
 * Verify with:
 *     docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_a1000/rom_ab1f4.c \
 *       asm/rom_a1000/rom_aa538_c_c_a_c_c.s --func Func_80ab1f4
 *
 * THE PARK'S RECIPE PATH IS STALE.  src/non_matching/rom_a1000/rom_ab1f4.c
 * cites asm/rom_a1000/rom_aa538_c_c_a.s twice (header and merged note); the live
 * file is asm/rom_a1000/rom_aa538_c_c_a_c_c.s.  Repoint both.
 * Note also that src/non_matching/rom_a1000/80ab314.c is NOT a park for this
 * function -- it names asm/rom_a1000/rom_aa538_c_c_c_a_a.s and its "227 of 307"
 * is its own figure for Func_80ab314.  rom_ab1f4.c is the real subject.
 *
 * SPLIT (for when it does land).  The live .s holds THREE functions.
 *   python3 tools/datacheck.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s -> CLEAN
 *   python3 tools/split_s.py asm/rom_a1000/rom_aa538_c_c_a_c_c.s Func_80ab1f4 --dry-run
 *     would write ..._c_c_a.s  (1 function, 283 lines) [Func_80aafb8]
 *     would write ..._c_c_b.s  (1 function, 26 lines)  [Func_80ab1f4]
 *     would write ..._c_c_c.s  (1 function, 114 lines) [Func_80ab21c]
 * PIN COUNT: 0.
 *
 * FINDING 1 -- A SECOND BLOCKER THE PARK MISSED, AND IT IS CLOSED.
 * The park says the only difference is one transposition.  It is not; the
 * epilogue differed too:
 *     rom    pop {r1} / bx r1
 *     park   pop {r0} / bx r0
 * The Thumb epilogue pops lr into a low register that is not holding the return
 * value.  The ROM avoids r0, so r0 IS LIVE AT EXIT -- the function returns a
 * value, and `_Func_8022768`'s result is what it returns.  Declaring both as
 * s32 and writing `return _Func_8022768(...)` closes it.  That is the brief's
 * declared-return-type lever, and the park's "void" signature was simply wrong.
 *
 * FINDING 2 -- THE REMAINING RESIDUE, AND THE EXACT RUNG IT SITS ON.
 * One insn, `add r0, #1`, is scheduled three slots late:
 *     rom    add r1,r2 | add r0,#1 | ldr r3,[sp,#0x10] | add r1,#1 | mov r2,r6
 *     ours   add r1,r2 | ldr r3,[sp,#0x10] | add r1,#1 | mov r2,r6 | add r0,#1
 *
 * -fsched-verbose=6, at the cycle where they compete (last_scheduled_insn is
 * `add r1,r2`, so the CLASS rung is a 3-3 tie):
 *     insn 12  r3=[sp+0x10]   prio 66  dependents {42, 65, 66, 67}  = 4
 *     insn 25  r0=r0+0x1      prio 66  dependents {42, 66}          = 2
 *     insn 31  r1=r1+0x1      prio 66  dependents {42, 66, 67}      = 3
 *     insn 39  r2=r6          prio 66  dependents {42, 66, 67}      = 3
 *     insn 33  [sp]=r5        prio 65
 * Priorities TIE at 66 -- arm_adjust_cost charges 1 for any link into a
 * CALL_INSN, so the load gets no latency credit.  The pick is therefore made on
 * the DEPENDENT-COUNT rung, and our order is exactly that rung's order:
 * 4, then 3, then 3 (LUID), then 2.  `add r0,#1` loses because it has the
 * FEWEST dependents in the window.
 *
 * The a5 load's two extra dependents are both sp anti-dependences -- on insn 65
 * (`add sp,#4`) and insn 67 (the pop/return) -- which it has purely for being
 * sp-relative.  `add r0,#1` cannot acquire a third or fourth dependent: r0 is
 * the return value, so the epilogue's `pop {r1}` gives it no output dependence
 * (and the void spelling, which does give it one, costs the epilogue).  That is
 * the whole shape of the residue: 4 vs 2, with no source-reachable way to close
 * the gap in either direction.
 *
 * So the park's intuition -- "the incoming stack argument's load is not
 * reachable from statement order ... it is an ABI fetch, not something the
 * source sequences" -- is RIGHT, and now has a mechanism and a number instead
 * of a hunch.  But it named the wrong cause (it blamed gcc "pulling the fifth
 * stack argument up one slot") and it stopped one blocker short.
 *
 * CROSSED PAIRS AND TRIPLES TRIED, ALL EXACTLY INERT AT 4 WITH THE RETURN TYPE:
 *   both coordinates in named locals, x first                               4
 *   x only in a named local                                                 4
 *   sums first, then the two +1s as separate statements                     4
 *   x finished before y, y's +1 left in the call                            4
 *   `__asm__("" : "+r" (cx))` between the add and the call                  4
 *   a5 copied into a named local first                                      4
 *   a4 and a6 copied into named locals first                                4
 *   `1 + w->col + x` (PLUS operand order, in integer space)                 4
 *   `register s32 a5` on the parameter                                      4
 *   a seventh, unused parameter                                             4
 *   the window struct replaced by a union (alias-set lever)                 4
 *   named local + barrier + a5 local (triple)                               4
 * The pre-sched2 stream is BIT-IDENTICAL across the first four of those, which
 * is why the park's statement-order list found nothing: gcc canonicalises them
 * all, and the a5 load is emitted in the prologue region at LUID 4 in every one.
 *
 * DECLINING TO CLOSE.  The diagnosis is refuted and replaced, the epilogue
 * blocker is closed, and the residue is reduced to one scheduler rung with its
 * inputs measured.  No reachability claim either way on that rung.
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
