/* OvlFunc_956_2008ba4 -- 0x02008ba4.  NOT MATCHING, 2 of 75 encodings.
 * STILL 2 AFTER BATCH 318.  Figure RE-MEASURED, not inherited; the park's
 * OBSERVATIONS all reproduced; its BLOCKER CLASS replaced with a rung-by-rung
 * argument that says which rung is live and why each escape is shut.
 *
 * *** THE FIGURE IS NOT A PRODUCTION-FLAG FIGURE.  The park says so loudly and
 * *** it is RIGHT: at the tree default this body is 72 of 75 with the
 * *** relocation list four bytes short (I reproduced exactly that).  Only
 * *** -fno-gcse reaches 2.  So "2 of 75" is a figure ABOUT ONE FLAG, not a
 * *** distance at the flags the build uses, and any landing needs this object
 * *** added to GCSE_CFLAGS with owner approval.  Treat the park's shouting
 * *** paragraph as load-bearing, not as colour.
 *
 * Verify with -- THE PARK HAD NO RECIPE AT ALL; this is the first one, and it
 * is the whole reason parkcheck could never report this park's figure:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA=-fno-gcse goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e0928/2008ba4.c \
 *     asm/overlays/rom_7e0928/ovl_30_c_c_c_a_a_a.s --func OvlFunc_956_2008ba4
 * gives
 *   (built with: -fno-gcse)
 *   XX ENCODINGS differ in 2 place(s) (ref 75, ours 75)
 *      first at index 21: ref 0073  ours 4f21
 * WITHOUT the -e line it gives 72 of 75 + XX RELOCATIONS differ.  A recipe for
 * this park MUST carry the flag or it reports a different function's problem.
 *
 * MEASURED, not assumed, with a disassembling per-index differ:
 *   SIZE 184/184, COUNT 75/75, 16/16 relocations identical, and the memory
 *   profile identical in every opcode (ldr 5, ldrsh 1, str 2, push 2, pop 3).
 *   BOTH differing encodings are REAL INSTRUCTIONS -- NO POOL WORD is involved,
 *   and the three pool words themselves are in the ROM's order:
 *     [21] ref 0073 lsls r3,r6,#1       ours 4f21 ldr r7,[pc,#132]
 *     [22] ref 4f21 ldr r7,[pc,#132]    ours 0073 lsls r3,r6,#1
 *   (`ldr r7,[pc,#132]` is a pool-LOAD INSTRUCTION, not a pool word.)
 *   `--whole` adds nothing: same 2, no SIZE line, no relocation line.
 * So under its flag it IS a true distance of 2, and a strict adjacent
 * transposition.
 *
 * SPLIT SHAPE: NONE NEEDED, confirmed by tool and not by counting by hand.
 *   tools/datacheck.py  -- prints no data-section line for this file
 *   tools/split_s.py ... OvlFunc_956_2008ba4 --dry-run
 *     "holds only OvlFunc_956_2008ba4 and no data; convert it directly,
 *      no split needed"
 *   overlays/rom_7e0928/overlay.ld:45 names the object once.
 * PIN COUNT: tools/shimcount.py reports NO pins and no fakematch-class shim in
 * this body -- it is shim-free, and must stay so (every pin was measured worse).
 *
 * ================================================================
 * THE REAL BLOCKER: sched2's CLASS RUNG, and it is the ONLY rung in play
 * ================================================================
 * The park calls this "the POOL LOADS COME FIRST ordering class ... and this
 * function has no usable boundary".  That is the right pass and the wrong rung.
 * Read off `-fsched-verbose=6` on basic block 1 (the block after
 * `bl __CutsceneStart`):
 *
 *     insn 42  = bl __CutsceneStart   cost 32
 *     insn 45  = r7 = 0x2073          prio 38  cost 2  dep 0  dependents {69,54}
 *     insn 50  = r3 = r6 << 1         prio 38  cost 1  dep 1  dependents {69,63,57,52}
 *
 *     ;; Ready list (t = 32):    50  45      -> picks 45   (ROM picks 50)
 *
 * The winner is printed LAST.  Both are READY at t = 32 (the call's 32-cycle
 * latency queues them together) and both have priority 38, so the first rung
 * ties.  The decision is the SECOND rung, `rank_for_schedule`'s classification
 * against `last_scheduled_insn` (= insn 42, the call):
 *   - insn 45 is NOT in INSN_DEPEND(42)                       -> class 3
 *   - insn 50 IS  (the call CLOBBERS r3 and 50 WRITES r3, an
 *     anti-dependence, for which ARM's ADJUST_COST makes
 *     insn_cost 0, so the `== 1` escape does not fire)        -> class 2
 * and higher class wins.  The dependent-count rung, which insn 50 would win
 * FOUR TO TWO, is never reached.
 *
 * NOTE THE PRICE OF GETTING THE RUNG RIGHT: the park's remedy list is aimed at
 * a boundary/LUID problem, and NONE of it can touch a class comparison.  That
 * is why its seven register pins and its basic-block search all failed.
 *
 * ================================================================
 * EVERY ESCAPE FROM THAT RUNG, AND WHY EACH IS SHUT
 * ================================================================
 * (1) Give insn 50 class 3.  It would have to have NO dependence on the call.
 *     Its dependence exists because the call clobbers r3 and the shift's
 *     destination IS r3 -- and r3 is the ROM's own destination (encoding 0073
 *     is literally `lsls r3,r6,#1`), so the ROM's compile had the same edge.
 * (2) Give insn 45 class <= 2.  It would have to depend on the call.  It reads
 *     nothing and writes r7, which is CALLEE-SAVED (call-clobbered here is
 *     r0-r3, r4 via -fcall-used-r4, r12, lr), so no anti-dependence can exist;
 *     and a pool load is NOT a MEM at sched2 (the literal pool is built after
 *     sched2), so no memory edge can exist either.  Both halves are bounds the
 *     brief already records, meeting here.
 * (3) Break the tie one rung EARLIER, on priority.
 *       priority(45) = priority(54) + cost(45) = 36 + 2 = 38
 *       priority(50) = priority(52) + cost(50) = 37 + 1 = 38
 *     and both chains terminate on the SAME insn -- insn 57, `bl __MessageID`,
 *     priority 35.  45 reaches it in two steps of a 2-cycle load; 50 reaches it
 *     in three steps of 1-cycle ALU ops.  THE TWO PATHS ARE THE SAME LENGTH BY
 *     CONSTRUCTION, because the ROM's own code is `lsl / add / add / bl` with
 *     the pool load feeding the last `add`.  Lowering priority(45) needs the
 *     pool value's FIRST use to be further from the call than `add r0,r5,r7`
 *     is, and the ROM uses it there.  Raising priority(50) needs one more ALU
 *     step between the shift and that `add`, which is an extra instruction.
 * (4) Change `last_scheduled_insn` so the class rung compares against
 *     something else.  Nothing else is ready at t = 32: the call holds the
 *     machine for 32 cycles and the block's only other insns depend on these two.
 * (5) Turn the rung off.  `-fno-schedule-insns2` -- MEASURED, see below.
 *
 * ================================================================
 * WHAT I MEASURED THIS BATCH (all under -fno-gcse; 75 encodings; 45 crosses)
 * ================================================================
 * THE PARK'S "INERT" EDITS ARE INERT IN COMPANY TOO, which is a stronger
 * statement than its one-at-a-time list and the one the brief asks for.  I took
 * five of them -- `h = a << 1` as its own statement; an early `return` for the
 * guard; `unsigned base`; `base + a * 3`; `(a << 1) + a` for `a * 3` -- and
 * crossed each against NINE of the park's rejected-as-worse edits:
 *
 *   worse edit                      alone   x E1h  x E6ret  x E7u  x E8ord  x E10sa
 *   ------------------------------  -----   -----  -------  -----  -------  -------
 *   `prod` named plain                 73      73       73     73        -        2
 *   `prod` pinned to r5             8+RLC   8+RLC    8+RLC  8+RLC        -    8+RLC
 *   `id = base` copy at site 1         13      13       13     13        -       13
 *   base pinned to r5                  13       -       13      -       13       13
 *   base pinned to r6                  11       -       11      -       11       11
 *   base pinned to r8               64+RLC      -   64+RLC      -   64+RLC   64+RLC
 *   base pinned to r9               66+RLC      -   66+RLC      -   66+RLC   66+RLC
 *   base pinned to r10              65+RLC      -   65+RLC      -   65+RLC   65+RLC
 *   literal at site 1, base after        2       2        2      2        -        2
 *
 * EVERY cross equals its worse half exactly.  44 of 45 crosses are additively
 * inert and the 45th (`prod` + `(a<<1)+a`) merely returns to 2.  The crossing
 * law is not violated here -- it is simply exhausted: the inert edits are all
 * SOURCE-ORDER/TYPE edits, and NONE of them can move a class comparison.
 *
 * AND THE -fno-schedule-insns2 CROSS IS SHUT, which was the one route the park
 * left genuinely open ("it pins it correctly and then costs 18 elsewhere,
 * because the ROM's ENTRY block is itself heavily scheduled").  I crossed the
 * flag with SEVEN rewrites of the entry block aimed at producing the ROM's
 * entry order WITHOUT the scheduler (a named `off = 0xfa << 1` reused as
 * `off - 0x32` for the 0xe1<<1 compare, in six placements, plus a named
 * address and an `unsigned off`):
 *   base body + the flag ........................ 20
 *   off named, 4 placements ................. 21, 22, 22, 22 (three +RELOCDIFF)
 *   g before p .................................. 21 +RELOCDIFF
 *   off and off2 both named .......... 65, count 71 (-8 bytes) +RELOCDIFF
 *   unsigned off ................................ 22 +RELOCDIFF
 * NOTHING gets below 20, so the entry block does not merely LOOK scheduled --
 * it cannot be spelled into that order, and `-fno-schedule-insns2` is not a
 * route even crossed.  (The park's own figure for the bare flag was 18; mine is
 * 20 on this body.  Either way: shut.)
 *
 * ================================================================
 * PARK DIAGNOSES: SURVIVED / REFUTED
 * ================================================================
 * SURVIVED, reproduced exactly:
 *   - 2 of 75 under -fno-gcse, and 72 of 75 + RELOCDIFF at the tree default;
 *   - that it is sched2 doing it and that sched2 must stay ON;
 *   - that 0x2073 is a LITERAL and not a symbol -- the reference carries
 *     exactly two R_ARM_ABS32 records, iwram_3001ebc and gState, so an
 *     `&_MSG_2073` spelling would ADD a relocation.  No .sym proposal is
 *     warranted and I am not making one.
 *   - both load-bearing spelling details (the second named local for the
 *     derived ids; `__Func_8092c40` left undeclared);
 *   - every entry in its INERT list, re-measured, and now also in company;
 *   - every entry in its MEASURED WORSE table, re-measured, all nine.
 * REFUTED or replaced:
 *   - BLOCKER CLASS: it is NOT "pool loads come first, but the basic-block
 *     lever moves them".  No boundary, no LUID and no pin can reach this
 *     residue, because the deciding rung is the CLASS comparison against
 *     `last_scheduled_insn`, which is a function of the CALL'S CLOBBER SET and
 *     of nothing a C statement can express.  The park's framing sent seven pins
 *     and a boundary search at a rung that was never in play.
 *   - "gcc-2.96 will CSE a local register variable against another pseudo that
 *     happens to be in the same hard register (r7 gives `add r0,r7,r7` -- a
 *     miscompile)": I did not reproduce a miscompile, and I would not record one
 *     from a figure.  The r7 pin measures 68 of 75 here; that is a mismatch.
 *     A miscompile claim needs the emitted stream quoted, and this park quotes
 *     a figure.  Downgrade it to "r7 pin: 68" until someone shows the stream.
 *
 * NOT CLOSED.  A refuted diagnosis is not a closure.  What a future batch
 * should attack, in priority order:
 *   1. the DEFAULT-FLAG shape, not this one.  2 of 75 is a figure about
 *      -fno-gcse; the function has to land at production flags or carry an
 *      owner-approved GCSE_CFLAGS row.  At default flags the body is TWO
 *      INSTRUCTIONS SHORT, so the question there is "what does gcse remove, and
 *      what source shape makes it keep both?" -- a different question from this
 *      one, and nobody has asked it.
 *   2. anything that puts a THIRD ready insn in block 1 at t = 32, which is the
 *      only way to change `last_scheduled_insn` at the deciding cycle.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __ActorMessage(int a, int b);
extern void __MapTransitionIn(void);
extern void __MapTransitionOut(void);
extern void __WaitMapTransition(void);
extern int __Func_8091c7c(int a, int b);
extern void OvlFunc_common1_78(int a);

void OvlFunc_956_2008ba4(int a)
{
    unsigned char *p;
    unsigned char *g;
    int t;
    int id;
    int base;

    p = iwram_3001ebc;
    g = gState;
    t = *(int *)(g + (0xfa << 1));
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        __CutsceneStart();
        base = 0x2073;
        __MessageID(a * 3 + base);
        __Func_8092c40(a, 0);
        if (__Func_8091c7c(t, 0) == 0) {
            id = base + 1;
            __MessageID(a * 3 + id);
            __ActorMessage(a, 0);
            *(int *)(p + (0xe0 << 1)) = 0x80 << 2;
            *(int *)(p + (0xe4 << 1)) = 0xf;
            __MapTransitionOut();
            __WaitMapTransition();
            OvlFunc_common1_78(a);
            __MapTransitionIn();
            __WaitMapTransition();
        } else {
            id = base + 2;
            __MessageID(a * 3 + id);
            __ActorMessage(a, 0);
        }
        __CutsceneEnd();
    }
}
