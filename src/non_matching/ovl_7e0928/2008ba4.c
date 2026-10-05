/* MEASURED FIGURE, backfilled in batch 324 (this park carried none).
 *
 *   72 differing encodings of 75.  SIZE DIFFERS (ref 75 encodings, ours 73).
 *
 * All sixteen relocations are the SAME SYMBOLS at a uniform -4 offset, so the
 * figure IS a distance: we are two instructions short near the top (first diff
 * at index 1) and everything after it is shifted.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7e0928/2008ba4.c asm/overlays/rom_7e0928/ovl_30_c_c_c_a_a_a.s --func OvlFunc_956_2008ba4
 *
 * The figure is EVIDENCE.  Everything below it is a HYPOTHESIS, and across
 * pass two a park's diagnosis has been wrong roughly 40 times in 42.
 */

/* OvlFunc_956_2008ba4 -- 0x02008ba4.  NOT MATCHING.
 *
 * TWO FIGURES, AND THE FIRST IS NOT A PRODUCTION-FLAG FIGURE:
 *   -fno-gcse      2 of 75   (ref 75, ours 75)   -- a true distance
 *   tree default  72 of 75   (ours 73) + RELOCDIFF -- TWO INSTRUCTIONS SHORT
 * Both re-measured this batch (brief 321-B), not inherited.  PIN COUNT 0
 * (tools/shimcount.py reports nothing of either class).
 *
 * Verify with -- NAMES THE INSTALLED PATH, and the flag is load-bearing:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     -e OBJCMP_EXTRA=-fno-gcse goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e0928/2008ba4.c \
 *     asm/overlays/rom_7e0928/ovl_30_c_c_c_a_a_a.s --func OvlFunc_956_2008ba4
 *     -> (built with: -fno-gcse)
 *        XX ENCODINGS differ in 2 place(s) (ref 75, ours 75)
 *           first at index 21: ref 0073  ours 4f21
 * `--whole` adds nothing: same 2, no SIZE line, no relocation line.
 * WITHOUT the -e line the same body reads 72 of 75 at 73 instructions.
 *
 * SPLIT SHAPE: NONE.  tools/datacheck.py is silent on the reference and
 *   tools/split_s.py ... OvlFunc_956_2008ba4 --dry-run says
 *   "holds only OvlFunc_956_2008ba4 and no data; convert it directly".
 *
 * THE RESIDUE IS A STRICT ADJACENT TRANSPOSITION, both halves real code:
 *   [21] ref 0073 lsls r3,r6,#1       ours 4f21 ldr r7,[pc,#132]
 *   [22] ref 4f21 ldr r7,[pc,#132]    ours 0073 lsls r3,r6,#1
 * (`ldr r7,[pc,#132]` is a pool-LOAD INSTRUCTION, not a pool word; all three
 * pool words are in the ROM's order.)
 *
 * ================================================================
 * WHICH RUNG DECIDES, READ OFF -fsched-verbose=6 RATHER THAN INFERRED
 * ================================================================
 * .23.sched2, basic block 1 (the block after `bl __CutsceneStart`):
 *
 *   ;;      insn  code    bb   dep  prio  cost   blockage units
 *   ;;       42   239     0     0    38    32    1 - 32   core : 69 62 57 54 52 50
 *   ;;       45   173     0     0    38     2    1 - 32   core : 69 54
 *   ;;       50   112     0     1    38     1    1 - 32   core : 69 63 57 52
 *   ;;      Ready list (t = 32):    50  45
 *   ;;              --> scheduling insn <<<45>>> on unit core      (ROM picks 50)
 *
 *   rung 1 PRIORITY        38 == 38                       ties
 *   rung 2 INSN_REG_WEIGHT dead (!reload_completed)
 *   rung 3 interblock      same bb                        dead
 *   rung 4 CLASS vs last_scheduled_insn (= insn 42, the call)
 *            insn 45: writes r7, CALLEE-SAVED here, reads nothing -> no link
 *                     -> CLASS 3
 *            insn 50: writes r3, which the call clobbers
 *                     -> (insn_list:REG_DEP_ANTI 42) -> CLASS 2
 *          HIGHER CLASS WINS.  ***THIS RUNG DECIDES.***
 *   rung 5 dependent count 50 has FOUR (69 63 57 52), 45 has TWO (69 54).
 *          Never reached -- and note it would pick 50, i.e. the ROM.
 *   rung 6 INSN_LUID       never reached.
 *
 * THE RTL, so the note kind is not a guess:
 *   (insn 50 45 52 (set (reg:SI 3 r3) (ashift:SI (reg/v:SI 6 r6) (const_int 1)))
 *        112 {*thumb_ashlsi3} (insn_list:REG_DEP_ANTI 42 (nil)))
 *   (insn 45 42 50 (set (reg/v:SI 7 r7) (const_int 8307 [0x2073]))
 *        173 {*thumb_movsi_insn} (nil))            <- EMPTY backward list
 *
 * *** CORRECTION TO docs/elevation.md, "the sched2 CLASS rung is dead for a
 * *** SECOND reason".  That entry says to treat the ladder as
 * *** `priority -> dependent count -> INSN_LUID`.  IT IS WRONG HERE AND THIS
 * *** FUNCTION IS THE COUNTEREXAMPLE: priority ties, dependent count favours
 * *** the ROM's choice FOUR TO TWO, and the ROM's choice still loses.
 * ***
 * *** THE RUNG, FROM THE COMPILER'S OWN SOURCE (gcc-2.96 haifa-sched.c:4068,
 * *** rank_for_schedule), so nobody has to infer it again:
 * ***     link = find_insn_list (tmp, INSN_DEPEND (last_scheduled_insn));
 * ***     if (link == 0 || insn_cost (last_scheduled_insn, link, tmp) == 1)
 * ***       tmp_class = 3;
 * ***     else if (REG_NOTE_KIND (link) == 0)        [Data dependence]
 * ***       tmp_class = 1;
 * ***     else
 * ***       tmp_class = 2;
 * ***     if ((val = tmp2_class - tmp_class)) return val;
 * *** NOTE THE ORDER: the `insn_cost == 1` escape to class 3 is tested BEFORE
 * *** the note kind, so it is DEPENDENCE-KIND-AGNOSTIC.  What makes an anti or
 * *** output dependence land in class 2 anyway is config/arm/arm.c
 * *** arm_adjust_cost, whose FIRST act is
 * ***     if (REG_NOTE_KIND (link) == REG_DEP_ANTI
 * ***         || REG_NOTE_KIND (link) == REG_DEP_OUTPUT)
 * ***       return 0;
 * *** and 0 != 1, so the escape cannot fire for them.  The same function
 * *** returns 1 for a TRUE dependence whose consumer is a CALL_INSN, which IS
 * *** the case where a data-dependent insn gets class 3 -- that is the true
 * *** observation brief C generalised too far.
 * *** CORRECT WORDING: the class rung is skipped at t=0 (haifa-sched.c:5963
 * *** sets `last_scheduled_insn = 0`) and is blind to any dependence whose
 * *** insn_cost is 1 -- on ARM, a true dependence into a call.  It IS live and
 * *** decisive for an ANTI or OUTPUT dependence, because arm_adjust_cost prices
 * *** those at 0.
 *
 * MECHANISM PROVED WITH AN INSTRUMENT, not argued.  `register int base
 * __asm__("r4")` puts the 0x2073 carrier in a CALL-USED register (-fcall-used-r4
 * is on).  The pool load then acquires its own dependence on the call, its class
 * drops to 2, the tie falls to the dependent-count rung and THE ORDER FLIPS TO
 * THE ROM'S: .23.sched2 block 1 schedules <<<42>>> <<<50>>> <<<52>>> <<<188>>>,
 * i.e. the `lsl` BEFORE the pool load.  The instrument's own figure is
 * meaningless (72 of 75 at 71 instructions, +RELOCDIFF) -- it is evidence about
 * the rung and the body shipped here does not contain it.
 *
 * ================================================================
 * WHY THE RUNG CANNOT BE REACHED FROM SOURCE
 * ================================================================
 * To win, insn 50 needs class >= 3 or priority >= 39.
 *  - CLASS.  Insn 50's anti-dependence exists because the call clobbers r3 and
 *    the shift's destination IS r3 -- and r3 is THE ROM'S OWN destination
 *    (encoding 0073 is literally `lsls r3,r6,#1`), so the ROM's compile carried
 *    the same edge.  The mirror escape -- give insn 45 an edge too -- needs the
 *    0x2073 carrier in a call-clobbered register, which is the instrument above,
 *    and the ROM's carrier is r7 (encoding 4f21 is `ldr r7,[pc,#132]`),
 *    callee-saved.  Both sides of the class comparison are fixed by the ROM's
 *    own register allocation.
 *  - PRIORITY.  priority(45) = cost(45->54) + priority(54) = 2 + 36;
 *    priority(50) = cost(50->52) + priority(52) = 1 + 37, and
 *    priority(52) = 1 + priority(54), priority(54) = 1 + priority(57) where 57
 *    is `bl __MessageID` at 35 (arm_adjust_cost pins a true dependence INTO a
 *    CALL_INSN at 1).  Raising priority(50) to 39 needs one more true-dependence
 *    step between the shift and `add r0,r5,r7`; that is an extra instruction and
 *    the count is already exact.
 *  - LAST_SCHEDULED_INSN.  The call's blockage is 1-32 and both competitors are
 *    queued for 31 cycles, so nothing can issue between the call and the pair:
 *    `Ready-->Q: insn 45: queued for 31 cycles.`  There is no third insn to put
 *    at t = 32.
 *
 * MEASURED THIS BATCH (tools/crossfire.py, 5 edits crossed to depth 2, all under
 * -fno-gcse, 75 encodings; figures are from objcmp itself):
 *   base .................................................. 2
 *   base = 0x2073 moved BEFORE __CutsceneStart() ........... 2  exactly inert
 *   __MessageID(base + a * 3) instead of a * 3 + base ...... 2  exactly inert
 *   extern int __CutsceneStart(void) (call_value_insn) ..... 2  exactly inert
 *   all three crossed pairwise ............................. 2  exactly inert
 *   base = 0x2073 hoisted ABOVE the `if` ................... 5  +RELOCDIFF
 *   INSTRUMENT register int base __asm__("r4") ............. 72 of 75 at 71 insns
 * Three exactly-inert source-order/type edits that also do not move the figure
 * in company: the crossing law is exhausted here, because none of these edits
 * can move a CLASS comparison.
 *
 * ================================================================
 * THE DEFAULT-FLAG SHAPE IS A KNOWN, ALREADY-ACCEPTED MECHANISM
 * ================================================================
 * At the tree default the body is 73 instructions against 75.  The whole deficit
 * is the PROLOGUE/EPILOGUE: default saves ONE high register
 * (`mov r7,r8 / push {r7}` ... `pop {r3} / mov r8,r3`), the ROM and -fno-gcse
 * save TWO (`mov r7,sl / mov r6,r8 / push {r6,r7}` ... `pop {r3,r5} /
 * mov r8,r3 / mov sl,r5`).  Diffing the two .s files, the cause is gcse's CPROP:
 * it substitutes 0x2073 into `base + 1` and `base + 2` across the CFG edge,
 * folds them to 0x2074/0x2075 (TWO EXTRA POOL WORDS, `.word 8308` and
 * `.word 8309`), and `base` then needs no register across the calls at all --
 * so one fewer callee-saved register is live and the frame shrinks by two insns.
 * Under -fno-gcse `base` lives in r7 and the arms read `add r0,r7,#1` /
 * `add r0,r7,#2`, which is the ROM.
 *
 * THAT IS EXACTLY THE MECHANISM THE TREE ALREADY ADOPTED GCSE_CFLAGS FOR.
 * Makefile, the rule for asm/overlays/rom_78ef88/ovl_314_c_c_a_c_a_a_c_c_b.o:
 * "OvlFunc_896_200978c holds a message id in a local and later uses `m + 1`
 * inside a nested `if`... cse.c:6572 ends a cse block only at a CODE_LABEL, so
 * the assignment and the later `m+1` share ONE cse block across the conditional
 * jump and `related_value` produces the add.  It is gcse's cprop that then
 * substitutes across the CFG edge and kills the register -- so cse WANTS the
 * register form here and gcse takes it away."  Same routine shape, same residue,
 * same pass.  That rule also records the escape and why it is unavailable to us:
 * "with the use in the SAME STRAIGHT-LINE RUN, both give the add" -- and this
 * function's two ids are in two ARMS of a nested `if` by necessity, because they
 * are different messages.  `volatile` is recorded there as costing a stack frame.
 *
 * SO: a landing here needs BOTH the GCSE_CFLAGS row AND the 2-encoding class-rung
 * residue closed.  It is two hurdles, not one, and only the first has a precedent.
 *
 * PARK DIAGNOSES: SURVIVED / REFUTED
 * SURVIVED: both figures; the rung being sched2's CLASS comparison against
 *   `last_scheduled_insn`; that it is an ANTI dependence; that 0x2073 is a
 *   literal (the reference carries exactly two R_ARM_ABS32 records,
 *   iwram_3001ebc and gState, so a symbol spelling would ADD a relocation);
 *   that -fno-schedule-insns2 is shut; every inert entry re-measured.
 * SURVIVED, and I WITHDRAW A "CORRECTION" I FIRST WROTE AGAINST IT.  The park
 *   says "an anti-dependence, for which ARM's ADJUST_COST makes insn_cost 0, so
 *   the `== 1` escape does not fire".  I initially recorded that as refuted, on
 *   the reasoning that the escape belongs only to true dependences.  THE PARK IS
 *   RIGHT AND I WAS WRONG: haifa-sched.c tests the cost escape BEFORE the note
 *   kind, so the escape is kind-agnostic and the cost is exactly what shuts it;
 *   arm_adjust_cost returns 0 for REG_DEP_ANTI.  The park's sentence is correct
 *   as written, down to the mechanism.  Recorded here because a park this good
 *   being "corrected" by a reader who had not read the compiler is the failure
 *   mode docs/elevation.md warns about, running in the other direction.
 * REFUTED: the park's claim that the default-flag question is unasked.  It is
 *   the OvlFunc_896_200978c mechanism, named in the Makefile, with the compiler
 *   reading already done.
 *
 * NOT CLOSED.  What is left, in priority order:
 *   1. the CLASS rung needs a different arm_adjust_cost or a different
 *      rank_for_schedule -- nothing a C statement can express reaches it;
 *   2. the GCSE_CFLAGS row is an owner decision, and the precedent for it is
 *      exact.
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
