/* OvlFunc_964_2009fdc -- 0x02009fdc, and
 * OvlFunc_964_200a040 -- 0x0200a040,
 * both in asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_a.s
 *
 * A place/restore pair: seat two entities, park two actors, and mirror the
 * whole thing back. They are contiguous, near-copies of each other, and share
 * a TU, so one solution would land both -- and one blocker parks both.
 *
 * 11 of 43 and 13 of 43.  Candidates below.
 *
 * BLOCKER: the straight-line repeated-constant CSE class, with no dominating
 * branch.  See "Constant rematerialisation needs a DOMINATING BRANCH" in
 * docs/elevation.md.  2009fdc rebuilds `mov #0xc6 / lsl #18` at two
 * MapActor_SetPos sites and gcc hoists it into r5; 200a040 does the same with
 * the -1 pair, and gcc goes further, deriving -1 from the live 0x31 as
 * `sub r5, r5, #0x32`.  Neither function has a branch anywhere, so cse1's
 * commoning stands and gcse cprop -- which is strictly cross-block -- can never
 * undo it.  tools/blocked_cse.py already flags both (1 and 3 repeats).
 *
 * ROOT-CAUSED, not merely observed.  With the repeated constants artificially
 * made distinct (0xc5 << 18 for the second x; -1,-2,-3,-4 for the edac pairs)
 * the residue collapses to 5 and 7, and what remains is ONLY `mov r0` sitting
 * after the two constant builds where the ROM has it in the gap -- the
 * arg-interleave shape, which is the same missing control-flow boundary seen
 * from the other side.  Both symptoms are one cause.
 *
 * A NEGATIVE RESULT WORTH THE PARK ON ITS OWN: `goto L; L:` IS NOT A USABLE
 * SUBSTITUTE FOR A BRANCH.  Written between the two SetPos calls it changes
 * nothing -- output byte-identical at 11/13 -- because gcc-2.96's jump pass
 * deletes the jump AND the now-unreferenced label BEFORE cse runs.  The only
 * reachable boundary is one that SURVIVES TO CSE.  So the dominating-branch
 * rule cannot be satisfied by manufacturing a branch; it has to already be in
 * the function's logic.  Nobody should spend another screen on this.
 *
 * Confirmed against the compiler on a purpose-built probe: with a real
 * `if (gGlobal)` between the two SetPos calls, -O2 alone STILL commons
 * 0x3180000 into r5; -O2 -fno-rerun-cse-after-loop PLUS the boundary rebuilds
 * both constants and drops `push {r5}` entirely.  That is the conjunction
 * blocked_cse.py's header states, now measured on this family.
 *
 * 200a040 is doubly blocked: its `neg r1, r1 / neg r2, r2` pair is the "two -1
 * in one argument list" sub-class, recorded as having ZERO occurrences across
 * all generated .s.
 *
 * WHAT WAS WON ANYWAY, and should be kept if these are ever revisited.  The
 * declaration lever carried the pair from 22/33 to 11/13 IN ONE EDIT: adding
 * void prototypes for the three callees fixed every argument-fill-order
 * difference in both functions at once.  Note that the sibling
 * src/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a_b.c matches with NO declarations at
 * all -- so "the neighbouring solved file omits them" is not evidence that this
 * one should.  Full prototypes was also the only Pareto point across the four
 * implicit/explicit combinations: each of the others helped one function and
 * hurt the other.
 *
 * The stack-arg spelling is confirmed by isolation rather than guessed.  Both
 * functions pass the same stack pair to both Func_8010704 calls -- (8, 0x31)
 * then (0x31, 0x2b) -- with 0x31 held in r5 across the first call.  Writing
 * that as t1/t2 at the top and t3 in an inner block reproduces the ROM's
 * str [sp] / str [sp,#4] ordering exactly: on a probe with the blocking
 * constants perturbed, the first difference is at index 20, so the whole
 * prologue and both bl blocks are instruction-exact.
 *
 * A MAKEFILE TRAP, FOUND HERE, NOT ACTIONABLE YET.  The wildcard
 * `asm/overlays/rom_7ed0a0/ovl_30_c_c_c_a_a%.o` applies O1_CFLAGS to this TU,
 * and -O1 is very likely wrong for it: the diff regresses to 17/14 with the
 * first disagreement at index 3, in prologue ordering that is exact at -O2.
 * `-O1 -fschedule-insns2` reproduces -O2 exactly, so the difference is purely
 * the post-reload scheduler.  This file's own history records the same trap
 * pointing the other way (OvlFunc_964_2009348).  If either function is ever
 * solved, it needs an explicit -O2 rule.
 *
 * TRIED, beyond the above: coordinate constants as locals at the top of the
 * function (52 -- an r8/r10 spill) and just before each call (11, identical to
 * literals); coordinates derived arithmetically as (x*2+1) << 19 from the
 * stack-arg variables (17, and it grows the prologue) -- so despite the tiles
 * (8,49) and (49,43) being exactly the two Func_8010704 stack-arg pairs, the
 * original used literals at the call sites; -Os and -O3 (both 11/13); and
 * TWENTY-EIGHT further -O2 flags, none of which move it:
 *   no-defer-pop, no-optimize-sibling-calls, no-cse-follow-jumps,
 *   no-cse-skip-blocks, no-expensive-optimizations, no-thread-jumps,
 *   no-strength-reduce, no-peephole, no-force-mem, no-function-cse,
 *   no-caller-saves, no-delayed-branch, no-gcse, no-rerun-cse-after-loop,
 *   rerun-loop-opt, no-delete-null-pointer-checks, no-schedule-insns,
 *   no-branch-count-reg, reorder-blocks, no-regmove,
 *   no-optimize-register-move, no-strict-aliasing, no-common, ssa.
 * Four more are strictly worse: no-omit-frame-pointer (20/19), force-addr
 * (23/22), rename-registers (14/16), no-schedule-insns2 (17/14).
 *
 * That flag sweep is now known to have been unnecessary: the responsible pass
 * is the first cse_main, which toplev.c:2917 runs under plain `optimize > 0`
 * with no -f flag gating it at all.  There is no flag to find.
 */

extern void __Func_8010704(int a, int b, int c, int d, unsigned int e, unsigned int f);
extern void __Func_808edac(int a, int b, int c);
extern void __MapActor_SetPos(int slot, int x, int y);

void OvlFunc_964_2009fdc(void)
{
    unsigned int t1 = 8;
    unsigned int t2 = 0x31;
    __Func_8010704(0x48, 0x31, 1, 1, t1, t2);
    {
        unsigned int t3 = 0x2b;
        __Func_8010704(0x71, 0x2b, 1, 1, t2, t3);
    }
    __Func_808edac(0x64, 0, 0);
    __Func_808edac(0x65, 0, 0);
    __MapActor_SetPos(0xf, 0x88 << 16, 0xc6 << 18);
    __MapActor_SetPos(0x10, 0xc6 << 18, 0xae << 18);
}

void OvlFunc_964_200a040(void)
{
    unsigned int t1 = 8;
    unsigned int t2 = 0x31;
    __Func_8010704(8, 0x71, 1, 1, t1, t2);
    {
        unsigned int t3 = 0x2b;
        __Func_8010704(0x31, 0x6b, 1, 1, t2, t3);
    }
    __Func_808edac(0x64, -1, -1);
    __Func_808edac(0x65, -1, -1);
    __MapActor_SetPos(0xf, 0, 0);
    __MapActor_SetPos(0x10, 0, 0);
}
