/* OvlFunc_951_2008880 -- NON-MATCHING, 4 differing STREAM encodings of 51.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch_elev/b272/B/BEST_OvlFunc_951_2008880.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 51 / ours 51 stream encodings; 42/42 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7d6418/2008880.c \
 *     asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c_a_a_a_c.s --func OvlFunc_951_2008880
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
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
        REG_BLDALPHA = *t++;
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
