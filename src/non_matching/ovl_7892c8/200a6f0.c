/* OvlFunc_888_200a6f0 (0x0200a6f0) -- NON-MATCHING.
 *
 * NON-MATCHING, 2 of 46 encodings  (MEASURED, batch 334, per-function and
 *   whole-file views agreeing).  INSTRUCTION COUNTS NOW AGREE -- ref 43,
 *   ours 43, SIZE 96/96.
 *   The previous figure, thirty-one of forty-six, was POSITIONAL on a stream
 *   two instructions short; this one is a distance, and the residue is SCHED2
 *   ORDER ONLY.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7892c8/200a6f0.c \
 *     asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_c_a_c_a_a_a.s \
 *     --func OvlFunc_888_200a6f0
 *
 * Pin-free. The piece holds this function alone.
 *
 * ============ BATCH 334: THE OLD VERDICT WAS WRONG ============
 *
 * The old header's register table was exactly right:
 *
 *              the angle    the +0x64 pointer
 *     rom      r6 (low)     r8  -- so it costs `mov r2, r8` and `mov r1, r8`
 *     ours     r8           r6 (low) -- no moves needed
 *
 * Its verdict -- "this is not a spelling problem, our code is better and there
 * is nothing to write. NEXT: nothing source-level" -- was not. There is.
 *
 * THE LEVER FOR THE PAIR: DECLARE THE ANGLE `unsigned short`, NOT `int`.
 * That alone takes it from ref 43 against ours 41 to 43/43 and eighteen of
 * forty-six. The old probes could not find it because all three varied the
 * POINTER's spelling, and the decision is made on the ANGLE's MODE.
 *
 * Why the mode decides it: this function has no branches, so it is ONE basic
 * block and global-alloc has nothing to do -- its own dump prints zero regs to
 * allocate, and the entire assignment is local-alloc's. local-alloc orders
 * quantities by QTY_CMP_PRI (local-alloc.c:1496-1498), which is
 * floor_log2(n_refs) * n_refs * size / (death - birth), and in Thumb it can
 * never hand out r7 (arm.h:898-899), so there are exactly TWO callee-saved low
 * slots, r5 and r6, for FOUR call-crossing values: the actor, the other
 * pointer, the angle and the +0x64 pointer. As an `int` the angle is SImode and
 * cheap in a high register, so the +0x64 pointer (four refs) outranks it and
 * takes r6. As `unsigned short` it is HImode, thumb_movhi_insn makes the high
 * registers dear, the angle takes r6, the pointer is pushed to r8 -- and reload
 * then inserts the ROM's two `mov` retrievals, which ARE the missing pair.
 *
 * `short ang` is NOT the same: signed reads ref 43 against ours 42 and
 * thirty-nine of forty-six, because the sign-extend appears.
 *
 * ============ THE OTHER THREE ONLY PAY CROSSED ============
 *
 * Measured one at a time, one of them looks WORSE. Crossed, they take it to 2.
 *
 *   * reading the other pointer BEFORE the +0x64 pointer swaps the r1/r2 head
 *     assignment. Eighteen to twelve, crossed with the next.
 *   * the sin accumulate written load-first -- the same shape as the cos line,
 *     so both accumulate into the LOADED register. ALONE THIS IS WORSE: twelve
 *     where the reversed spelling reads eight. Crossed with the temp below it
 *     reads TWO. One-at-a-time testing rejects the right answer here, which is
 *     the search-discipline failure the briefs keep naming.
 *   * a named temp for the `a+8` reload, read BEFORE the two v stores. Eight to
 *     seven on its own, and it is what lets the sin spelling pay.
 *
 * ============ THE RESIDUE IS 2 AND IT IS SCHED2 ============
 *
 * The ROM orders `str r3, [r5, #8]`, then the argument move, then the call; we
 * emit the two in the other order. Both must precede the call -- one is memory,
 * one is the argument -- so it is a rank_for_schedule tie-break, not a value
 * and not a register. arm_adjust_cost (arm.c:2416) puts the argument move at
 * exactly prio(call)+1; the store's chain runs on through the later reload of
 * `a+8`. Six orderings of the cos block and the tail were measured: the temp
 * read before the v stores is the only one at 2, reading it in place is twelve,
 * and storing `a+0x38` before the v stores is four.
 
 * ===== BATCH 336: THE RESIDUE IS A STALL SLOT, AND THE READINESS LEVER IS
 * ===== INERT HERE -- MEASURED, NOT ASSUMED
 *
 * RE-MEASURED AT 2 of 46, ref 46 ours 46, instructions 43/43, SIZE 96/96, first
 * difference at index 19 (ref 60ab = `str r3, [r5, #8]`, ours 1c30 =
 * `mov r0, r6`).  The park's figure and its whole account of HOW it got to 2
 * reproduce exactly.  The body below is unchanged.
 *
 * WHAT THE TWO ACTUALLY ARE, read off the scheduler's own log
 * (`-fsched-verbose=6`, which this build writes into `x.c.23.sched2`, not to
 * stderr).  In `.19.flow2` the pair is
 *
 *   insn 45  (set (mem:SI (plus r5 8)) (reg:SI 3 r3))     the `str`
 *   insn 50  (set (reg:SI 0 r0) (reg:SI 6 r6))            the `__sin` arg move
 *
 * and at t=49 the ready list is exactly `45 50`.  **The chosen insn is the LAST
 * element of the sorted array** -- `rank_for_schedule` (haifa-sched.c:4029)
 * takes `tmp = *(rtx *)y` and `tmp2 = *(rtx *)x`, so the ascending sort puts the
 * BEST last and `schedule_block` reads `ready[n_ready - 1]`.  Ours picks 50.
 *
 * THE DECIDING RUNG IS THE CLASS TEST, NOT LUID.  LUID(45) < LUID(50), and the
 * final rung (haifa-sched.c:4111, `INSN_LUID (tmp) - INSN_LUID (tmp2)`) makes
 * the LOWEST LUID win -- so if the rungs above it tied, the `str` would already
 * be chosen.  It is not, so an earlier rung demotes it: `last_scheduled_insn` at
 * t=49 is insn 43, `add r3, r3, r2`, and insn 45 is DATA DEPENDENT on it, which
 * is class 1 (haifa-sched.c:4068-4095); insn 50 has NO log links at all and is
 * class 3.  `arm_adjust_cost` (arm.c:2402-2437) cannot help: it zeroes only
 * anti/output dependences, returns 1 only for a CALL_INSN after a load or a
 * load-after-store from stack/pool, and otherwise returns `cost` unchanged.
 * **So the scheduler is filling the add->str latency slot with the independent
 * argument move, and the ROM did not.**
 *
 * THE BATCH-335 READINESS BARRIER IS INERT HERE AND COSTS TWO INSTRUCTIONS.
 * insn 50 is in the ready list from t=0 purely because it has no predecessor,
 * which is the exact precondition that batch 335 broke with a bare
 * `__asm__ volatile ("")`.  MEASURED, both placements:
 *
 *   bare barrier between the `a+8` store and `s = __sin(ang);`   41 instructions against 43, twenty differing
 *   bare barrier before the `a+8` store                           41 against 43, twenty-one differing
 *
 * Both LOSE two instructions rather than reordering two, because a two-sided
 * `ASM_INPUT` barrier also stops the `mov r0, r6` from being hoisted out of the
 * cos block at all.  **Record this as a limit on that lever: it fixes readiness
 * for an insn you want scheduled LATER, and it cannot be used to make a
 * stall-slot filler unavailable without changing the block's length.**
 *
 * THE ACCUMULATE DIMENSION IS NOW CLOSED, CROSSED NOT DIAGONAL.  The park had
 * varied the sin line alone; the cos line was never varied, and crossing both
 * is worse everywhere.  Figures are stream lengths, ref 46, all at 43/43
 * instructions so these are distances:
 *
 *   cos load-first, sin load-first (SHIPPED)                      2
 *   cos value-first, sin load-first                               six
 *   cos load-first, sin value-first                               seven
 *   cos value-first, sin value-first                              eleven
 *   cos value-first + `t` read after the v stores                 sixteen
 *   cos value-first + both value-first + `t` read late            twelve
 *   cos sum through a named `t` then stored                        six
 *   the `__sin` argument re-read from `q` instead of `ang`        37 instructions against 43
 *   a second `unsigned short ang2` copied for the `__sin` call    48 against 43
 *
 * NEXT, and it is narrow: the only way to win the class rung is for insn 45 NOT
 * to be data-dependent on whatever is scheduled immediately before it, which
 * means the stored value must be produced more than one insn earlier WITHOUT
 * adding an insn.  Every spelling that separates them (a named `t`, a
 * value-first sum) costs four or more because it also changes which register
 * the sum accumulates into.  A `define_peephole`-style fusion is not available:
 * `arm.md` has no pattern over `add`+`str`.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 2 of 46, instruction counts EQUAL, byte size and pool
 *   count equal too, so this figure IS a real distance.
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: all 14 statement boundaries at every brace depth
 *   measured, all inert or worse.  Nothing capped, nothing failed to compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  The residue is NOT a placement or flag question;
 *   it is a register choice, a tie, or a pass-level effect, and it needs a
 *   hypothesis.
 *
 *   Read as "inert against THIS body" -- a recorded negative expires silently
 *   when the body changes.
*/
extern int __cos(int a);
extern int __sin(int a);

void OvlFunc_888_200a6f0(unsigned char *a)
{
    unsigned char *q;
    unsigned char *b;
    unsigned short ang;
    int c;
    int s;
    int v;
    int t;

    b = *(unsigned char **)(a + 0x68);
    q = a + 0x64;
    ang = *(unsigned short *)q;
    c = __cos(ang);
    *(int *)(a + 8) = *(int *)(b + 8) + ((c * 8 - c) << 1);
    s = __sin(ang);
    v = *(int *)(b + 0x10) + ((s * 4 + s) << 1);
    t = *(int *)(a + 8);
    *(int *)(a + 0x10) = v;
    *(int *)(a + 0x40) = v;
    *(int *)(a + 0x38) = t;
    a += 0x66;
    *(unsigned short *)q = *(unsigned short *)q + *(unsigned short *)a;
}
