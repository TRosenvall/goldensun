/* OvlFunc_951_2008880 -- NON-MATCHING, 2 differing STREAM encodings of 51.
 *
 * ref 51 / ours 51 stream encodings; 42/42 16-bit instructions -- IN PHASE,
 * so a true DISTANCE.  objcmp --whole reports NO relocation difference, so this
 * is a plain distance, not the dirty-relocation shape batch 338 found.
 * Re-measured at the head of batch 339.
 *
 * THE RESIDUE IS NOW ONE ADJACENT TRANSPOSITION IN THE PREHEADER of the second
 * loop, at stream index 26 -- THE LOOP BODY IS BYTE-EXACT:
 *     rom   mov r10,r3 / mov r8,r2    / mov r6,#0x7
 *     ours  mov r10,r3 / mov r6,#0x7  / mov r8,r2
 * IMPROVED FROM 4 to 2 IN BATCH 339, pin-free (shimcount prints no rows).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7d6418/2008880.c \
 *     asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a_c.s --func OvlFunc_951_2008880
 *
 * THIS PARK'S STATED BLOCKER IS GONE.  The park ended with "DO NOT SWEEP THIS
 * AGAIN without a compiler-side change", on an INSN_PRIORITY argument that was
 * arithmetically correct and still had the wrong conclusion -- the standing
 * lesson, checked once more: A PRIORITY BOUND IS ONLY VALID IF READINESS IS
 * FIXED.  The park proved `add r5,#2` (priority 1) can never outrank `strh`,
 * `mov r0,#8` and `sub r6,#1` (priority 2) and that no source spelling can give
 * it the true dependence it would need.  All true.  You do not win that tie --
 * YOU EMPTY THE READY LIST, and the park never tested that because the residue
 * is INSIDE A LOOP BODY and every anchor sweep run on it, including batch 339's
 * autosweep, only places anchors at TOP-LEVEL statement boundaries.
 *
 * WHAT MOVED IT, 4 -> 2 (two edits, and NEITHER WORKS ALONE):
 *
 *   1. SPLIT THE LOAD OFF THE STORE with an `int` temp:
 *          int h;  ...  h = *t++;  REG_BLDALPHA = h;
 *      `int` and not `unsigned short`: the park already recorded that an
 *      `unsigned short` temp turns the ldrh into an ldrsh, and it is still true
 *      (measured 30 of 51 at 53 instructions, i.e. WORSE AND OUT OF PHASE).
 *      The split alone is inert at 4.
 *   2. AN ANCHOR BETWEEN THE TWO HALVES: `do { } while (0);` after `h = *t++;`
 *      and before `REG_BLDALPHA = h;`.  Anchors at all five of the do-body's
 *      OTHER statement boundaries measure 4, 6, 4, 6, 4 -- the position is the
 *      whole lever, and it is the one position that only exists once the load
 *      has been split out.
 *
 *   With both, the ldrh and the `add r5,#2` are the only insns before the fence,
 *   so after the ldrh the ready list holds `add r5,#2` ALONE and it issues
 *   second, which is the ROM's order.  THE ENTIRE LOOP BODY IS THEN BYTE-EXACT:
 *   mov r3,r10 / mov r2,r8 / strh / ldrh r3,[r5] / add r5,#2 / strh / mov r0,#8 /
 *   sub r6,#1 / bl / cmp / bge.  Pin-free: shimcount prints no rows for this body
 *   (batch 337 established the anchor carries no depin debt, and that holds here).
 *
 * WHAT IS LEFT, AND THE RUNG IT SITS ON.  Two encodings, one adjacent
 * transposition in the second loop's PREHEADER (basic block 2).  From the
 * sched2 dependence table for that block:
 *
 *     insn 180  mov r3,#0x3f42 (pooled)  prio 3   dependents 1 (insn 61)
 *     insn  61  mov r10,r3               prio 1   dependents 0
 *     insn  64  ldr r5,.LC1              prio 2   dependents 0
 *     insn  67  mov r6,#0x7              prio 1   dependents 0
 *     insn 183  mov r2,#0x4000050        prio 3   dependents 1 (insn 155)
 *     insn 155  mov r8,r2                prio 1   dependents 0
 *     insn 157  mov r7,#0x4000052        prio 2   dependents 0
 *
 * The contested pair is insn 67 (`mov r6,#0x7`, from `i = 7;`) against insn 155
 * (`mov r8,r2`, the hard-register half of the LOOP-HOISTED REG_BLDCNT address).
 * Equal priority 1, equal class, ZERO in-block dependents each -- their
 * consumers are in the loop body, a different block -- so rank_for_schedule
 * falls all the way to `INSN_LUID (tmp) - INSN_LUID (tmp2)`
 * (haifa-sched.c:4113-4116) and insn 67 wins because it is earlier.
 *
 * AND THE LUID ORDER IS NOT REACHABLE FROM SOURCE.  loop.c emits every movable
 * with `emit_insn_before (..., loop_start)`, so the hoisted address ALWAYS lands
 * immediately before NOTE_INSN_LOOP_BEG -- i.e. AFTER every preheader statement,
 * whatever order they are written in.  Measured: ALL SIX permutations of
 * `v = 0x3f42; t = L1fc0; i = 7;` crossed with an anchor at all four preheader
 * boundaries, 30 variants, best 2 and nothing below it (v,t,i / t,v,i / v,i,t
 * all 2; i,t,v / i,v,t / t,i,v all 3; every anchor placement 6 to 10).  This is
 * the same limit batch 336 found from the other side -- "a loop hoist is not
 * reachable by a barrier at all" -- stated here as its consequence for LUID:
 * THE HOISTED INSN HAS THE HIGHEST LUID IN THE PREHEADER BY CONSTRUCTION, so any
 * preheader insn it must FOLLOW is unreachable by statement ordering.
 *
 * WHAT TO TRY NEXT: give insn 155 an in-block dependent (so it wins the
 * dependent-count rung instead of the LUID rung), or stop the hoist of the
 * REG_BLDCNT address so that `mov r8,r2` is not a movable at all -- the ROM does
 * hoist it, so the second route has to put it back in the preheader by other
 * means.  The loop body is closed; only the preheader is open.
 *
 * AUTOSWEEP, batch 339, ON THE OLD BODY -- FLAG AXIS CLOSED: -fno-gcse,
 * -fno-strict-aliasing, -fno-strength-reduce, -fno-rerun-cse-after-loop,
 * -fno-rerun-loop-opt all inert at 4; -O1 15; -fno-schedule-insns2 15;
 * -ffixed-r7 57 (and out of phase).  ANCHOR AXIS: 14 of 16 top-level placements
 * measured, all inert or worse, 2 produced no row and are UNTESTED.
 * Re-crossing the flags against THIS body is the obvious unspent axis.
 *
 */
#include "gba/io.h"

extern unsigned char *iwram_3001e70;
extern unsigned short L1fc0[] __asm__(".L1fc0");
extern void __PlaySound(int id);
extern void __WaitFrames(int n);

void OvlFunc_951_2008880(void)
{
    unsigned char *p;
    unsigned short *t;
    int i, v;
    int h;

    p = iwram_3001e70;
    __PlaySound(0xd8);
    p += 0x164;
    for (i = 15; i >= 0; i--) {
        *(int *)(p + 0xc) -= 0x10000;
        __WaitFrames(4);
    }
    v = 0x3f42;
    t = L1fc0;
    i = 7;
    do {
        REG_BLDCNT = v;
        h = *t++;
        do { } while (0);
        REG_BLDALPHA = h;
        i--;
        __WaitFrames(8);
    } while (i >= 0);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_951_2008880 -- asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a.s
 *
 * BLOCKER: INSTRUCTION SCHEDULING (two adjacent swaps in one loop)
 *
 * 4 of 47 differing.
 *
 * Two adjacent-instruction swaps in the second loop: `mov r8, r2` vs `mov r6, #7`
 * in the preheader, and `add r5, #2` vs `sub r6, #1` in the body.  Pure
 * scheduling; contents and registers otherwise identical.
 * * MEASURED: *t++ vs *t; t++; vs t = t + 1 vs t[0]; named unsigned short / int temp
 * for the loaded value (the unsigned short temp turns the ldrh into an ldrsh);
 * for / while / do-while / while(--i >= 0); all six orderings of the three
 * preheader assignments; swapping i-- and t++; a named int d = 8 for the
 * __WaitFrames argument; -fno-rerun-cse-after-loop, -fno-schedule-insns,
 * -fno-strength-reduce, -fno-gcse.  Best is 4.
 *)

(* ==================== BATCH 272: UNREACHABLE, WITH THE ARITHMETIC ====================
 *
 * Still 4 of 47, but reached from a BETTER-CONDITIONED shape -- prologue, both
 * pool-load orders, the whole first loop and the epilogue are byte-exact, and the
 * residue is two adjacent transpositions only. Candidate:
 * scratch_elev/b272/B/BEST_OvlFunc_951_2008880.c.
 *
 *   preheader  rom  mov r8, r2 | mov r6, #7        ours  mov r6, #7 | mov r8, r2
 *   body       rom  ldrh r3,[r5] / add r5,#2 / strh r3,[r7] / mov r0,#8 / sub r6,#1
 *              ours ldrh r3,[r5] / sub r6,#1 / strh r3,[r7] / mov r0,#8 / add r5,#2
 *
 * THREE SHAPE FINDINGS got it here and are worth keeping even though it is not
 * closed:
 *
 *   - `int v = 0x3f42;` BEFORE the loop, not the literal in `REG_BLDCNT = 0x3f42`.
 *     expand_assignment forces the store's ADDRESS into a register before expanding
 *     the RHS, so the literal spelling creates the 0x4000050 pseudo first, it takes
 *     r10 and the value takes r8 -- the ROM's deal is the reverse. Naming the value
 *     creates its pseudo earlier. 16 differing to 8.
 *   - `p = iwram_3001e70; __PlaySound(); p += 0x164;` -- the `+ 0x164` must be a
 *     SEPARATE STATEMENT AFTER the call, or sched2 hoists `add r6, r3, r2` above the
 *     `bl` and the whole first loop shifts register. 24 to 16.
 *   - `do { ... i--; } while (i >= 0)` rather than a `for`. 8 to 6 by edit distance.
 *
 * THE BLOCKER IS NEITHER SCHEDULING-AS-RECORDED NOR AN ALIAS PROBLEM. It is
 * INSN_PRIORITY arithmetic, and the number needed is unreachable. From
 * -fsched-verbose=5 on the loop-2 body:
 *
 *   ;;  insn  code  bb  dep  prio  cost
 *   ;;    85   180   0    2     2     2   strh r3,[r7]   (REG_BLDALPHA)
 *   ;;    86     5   0    1     1     1   add r5,#2      (t++)
 *   ;;    89     5   0    0     2     1   sub r6,#1      (i--)
 *   ;;    92   173   0    0     2     1   mov r0,#8
 *   ;;  Ready list (t = 6):  86  85  92  89   --> scheduling insn <<<89>>>
 *
 * `add r5, #2` is READY at the slot but has PRIORITY 1 while all three competitors
 * have 2. Its only successor is the call, via a cost-0 ANTI-dependence (the call
 * neither reads nor writes r5, callee-saved here), so it inherits no priority;
 * `sub r6,#1` has a true dependence onto the `cmp` and `mov r0,#8` onto the `bl`.
 * rank_for_schedule compares priority FIRST and returns before any tie-break, so 89
 * wins.
 *
 * For the ROM's order insn 86 would need priority >= 3, which requires a TRUE
 * dependence onto insn 85 -- the `strh` to REG_BLDALPHA would have to READ r5. In the
 * ROM that store reads only r3 and r7, so no source spelling can create the edge.
 *
 * AND THE ALIAS READING DOES NOT APPLY, which was checked rather than assumed: this
 * is the "ready early with low priority" signal, but the missing edge would have to
 * be a REGISTER dependence, not a memory one (the load->store edge 84->85 already
 * exists and is correct). So a union access, a typed field and -fno-strict-aliasing
 * are all irrelevant here. -fno-schedule-insns2 is ACTIVELY WRONG: the ROM's order is
 * the SCHEDULED order, just a different schedule (edit 16 against 6).
 *
 * MEASURED, all 4 unless stated: six orderings of the three preheader assignments;
 * `*t; t++` split; a named temp for the loaded halfword; `int d = 8` for the
 * __WaitFrames argument; `i--` before and after the REG_BLDALPHA store; `t++` after
 * `i--`. Flags all inert at 4: -fno-sched-interblock, -fno-sched-spec, -fno-gcse,
 * -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-expensive-optimizations,
 * -fno-caller-saves, -fno-delayed-branch, -fno-peephole2, -fno-cse-follow-jumps.
 * WORSE: -fno-schedule-insns2 and -O1 (edit 16); a `for` loop in loop 2 and
 * `REG_BLDALPHA = L1fc0[i]` (edit 28).
 *
 * TWO FACTS FOR WHOEVER RETURNS: the second loop's data is `.L1fc0`, reachable as
 * `extern unsigned short L1fc0[] __asm__(".L1fc0");` and defined at
 * asm/overlays/rom_7d6418/ovl_30_c_c_c_c_a.s:1138 as exactly 16 bytes -- 8 halfwords,
 * one per iteration. And the callee is spelled `__WaitFrames` in this reference even
 * though the elevated sibling in the same directory calls it `__CutsceneWait`.
 *
 * DO NOT SWEEP THIS AGAIN without a compiler-side change.
 *)

*/
#endif
