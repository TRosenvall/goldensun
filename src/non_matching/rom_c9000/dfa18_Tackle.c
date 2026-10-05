/* ===================== BATCH 327 (brief A) ADDENDUM -- READ FIRST =====================
 * BaseAnim_Tackle -- STILL NON-MATCHING, 2 differing encodings of 402.  BODY
 * UNCHANGED.  NO SWEEP RUN THIS BATCH, BY DESIGN: this header's own closing line
 * asked for "the citation, not another sweep", and the citation is below.
 *
 * FIGURE RE-MEASURED MYSELF: --func 2 of 402 (ref 402, ours 402); --whole
 * 2 of 402, relocations exact.  So the 2 is still a true distance.
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/dfa18_Tackle.c asm/rom_c9000/rom_dfa18_c_c_c_c_a.s --func BaseAnim_Tackle
 *
 * *** FIRST, A POLICY FACT THAT OUTRANKS THE FIGURE AND WAS NOT RECORDED HERE.
 * THIS BODY CARRIES 10 PINS (PIN1/2/3 = r0,r1,r2, plus r9,r11,r0,r8,r3,r7,r4),
 * AND THIS HEADER ITSELF SAYS REMOVING ONLY THE r9 PIN COSTS 47 -> 224.  SO THE
 * PIN-FREE FIGURE IS IN THE HUNDREDS AND THE 2 IS A TEN-PIN FIGURE.  Under
 * docs/owner-decisions.md standing standard 3, BaseAnim_Tackle CANNOT BE A
 * PASS-2 LANDING EVEN AT 0 -- it is a pass-3 (depinning) item by construction.
 * Batch 327's brief ranked it "the best landing chance" on the figure alone.
 * A LOW PARK FIGURE IS NOT A SHORT DISTANCE TO A LANDING WHEN IT IS BOUGHT
 * WITH PINS; RANK LOW-BAND PARKS BY (figure, pins), NEVER BY figure. ***
 *
 * -------- THE RESIDUE IS A SWAP, NOT A SHIFT (new; decoded this batch) --------
 *   idx 223  ref 9c06 = ldr r4,[sp,#24]   ours 9808 = ldr r0,[sp,#32]
 *   idx 224  MATCHES in both              = mov r1,r9   (a FIXED POINT)
 *   idx 225  ref 9808                     ours 9c06
 * ours = 549, 551, 1101   ROM = 1101, 551, 549.  The middle insn does not move;
 * the two ENDS exchange.  The ROM's order is the EXACT REVERSE of chain order.
 *
 * -------- WHY NO READY-LIST RANKING REACHES IT: the priority rung is tied
 * -------- BY CONSTRUCTION, and this header's stated reason was the wrong one.
 * This header claimed "ALL FOUR RUNGS TIE BY CONSTRUCTION" and justified it with
 * the DEPENDENT-COUNT argument, which does not bear on priority.  From my own
 * .23.sched2 (-da -fsched-verbose=6) on this body:
 *     549  prio 35  deps 594 1104 556       1104 prio 8
 *     551  prio 35  deps 594 1107 556       1107 prio 3
 *    1101  prio 35  deps 594 1110 556       1110 prio 3
 *     556  prio 34  <- THE SITE-2 CALL      594  prio 1
 * All three fills share the dominant dependent 556, and prio(556) = 34 exceeds
 * every private next-writer (8, 3, 3) by >= 26, so all three are pinned at
 * 34 + 1 = 35.
 * *** THE GENERAL FORM, worth more than this function: IN A BASIC BLOCK THAT
 * CONTAINS A LATER CALL, EVERY ARGUMENT FILL AND EVERY ADDRESS RELOAD OF AN
 * EARLIER CALL HAS THAT LATER CALL IN ITS INSN_DEPEND (a CALL_INSN depends on
 * every preceding set of a call-clobbered register), AND THE LATER CALL'S
 * PRIORITY IS THE BLOCK-TAIL LENGTH, SO IT DOMINATES EVERY PRIVATE NEXT-WRITER.
 * THE PRIORITY RUNG IS THEREFORE DEAD FOR THE WHOLE WINDOW, AND PERTURBING THE
 * LATER CALL'S OWN ARGUMENT SETUPS CANNOT REVIVE IT. ***  (That was the one
 * crack this header had left open; it is shut.)
 *
 * -------- AND LUID'S DIRECTION, CITED --------
 * rank_for_schedule (haifa-sched.c:4030-4115) binds `tmp = *(rtx*)y` and
 * `tmp2 = *(rtx*)x` -- SWAPPED -- so its tail `return INSN_LUID (tmp) -
 * INSN_LUID (tmp2)` is LUID(y) - LUID(x): the ready array sorts DESCENDING by
 * LUID and the scheduler takes the LAST element, i.e. the LOWEST LUID.  (The
 * same swap makes the priority rung sort ascending, so the highest priority is
 * taken last = scheduled first.  Both readings are consistent.)
 *
 * -------- CHAIN ORDER IS FORCED, AND THIS HEADER HAD ONLY HALF THE CITATION --
 * It cited only LOAD_ARGS_REVERSED (undefined by every target, calls.c:1692).
 * THERE IS A SECOND REVERSAL SWITCH IT NEVER CHECKED: args[] itself can be
 * filled back-to-front at calls.c:1097 under PUSH_ARGS_REVERSED.  That is also
 * 0 on this target, and the chain is
 *     arm.h:1314   ACCUMULATE_OUTGOING_ARGS 1
 *  -> calls.c:44   PUSH_ARGS = !ACCUMULATE_OUTGOING_ARGS          = 0
 *  -> calls.c:67   PUSH_ARGS_REVERSED = PUSH_ARGS                 = 0   (:73 default 0)
 * So args[0] IS argument 0 and load_register_parameters (calls.c:1692-1696)
 * walks it FORWARD.  *** ARGUMENT 0'S FILL IS UNCONDITIONALLY CHAIN-EARLIER
 * THAN ARGUMENT 1'S ON THIS TARGET. ***
 *
 * THE ROM PUTS ARGUMENT 1 (mov r1,r9) BEFORE ARGUMENT 0 (ldr r0,[sp,#32]).  The
 * rungs above say the scheduler can only reproduce chain order; the switches say
 * chain order can never put argument 1 first.  THEREFORE NO SOURCE SPELLING WITH
 * `ctx` AS A PLAIN REGISTER ARGUMENT 0 CAN PRODUCE THE ROM'S WINDOW.
 *
 * THE ONE ESCAPE LEFT -- this header's own named next step, now derived as the
 * UNIQUE route rather than guessed.  The ROM's `ldr r0,[sp,#32]` must not be
 * argument 0's forward fill at all but a reload for THE CALL INSN ITSELF (call
 * reloads are emitted immediately before the call, hence after every forward
 * fill), while the target load sits at its rtx_for_function_call expand position
 * (calls.c:2916, which runs BEFORE load_register_parameters at :3029) -- giving
 * exactly [ldr r4] [mov r1] [ldr r0].  For that, `emit_move_insn (r0,
 * args[0].value)` must emit NOTHING at the forward position, which requires
 * args[0].value to already BE hard reg r0.  I could not construct that for an
 * ordinary pseudo argument and do not believe it is constructible in C.
 * STATED AS A BOUND WITH ITS EVIDENCE ATTACHED, NOT AS A CLOSED DOOR.
 *
 * NOT RE-SWEPT and still valid: every inert/worse list below, measured at this
 * same 2-of-402 baseline in batch 318.
 * =====================================================================================
 */
/* ===================== BATCH 318 (brief A) ADDENDUM -- READ FIRST =====================
 * BaseAnim_Tackle -- STILL NON-MATCHING, 2 ENCODINGS OF 402.  NO CHANGE TO THE
 * BODY: of 18 crossed variants this batch, none beat it and the ten that were
 * exactly inert are listed below so nobody re-runs them.  DECLINING TO CLOSE.
 *
 * FIGURE RE-MEASURED MYSELF, both ways:
 *   --func  : 2 of 402 (ref 402, ours 402), idx 223 ref 9c06 ours 9808,
 *             idx 225 ref 9808 ours 9c06
 *   --whole : 2 of 402, 916 bytes, *** RELOCATIONS EXACT ***  <- so the 2 IS a
 *             distance, unlike the two overlay twins' figures
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/dfa18_Tackle.c \
 *     asm/rom_c9000/rom_dfa18_c_c_c_c_a.s --func BaseAnim_Tackle
 * SPLIT: `python3 tools/datacheck.py asm/rom_c9000/rom_dfa18_c_c_c_c_a.s` prints
 * NOTHING -- no data section, one function.  CONVERTS WHOLE, no split, no data
 * work, no .sym entry, no flag row.  PINS: 10 (tools/shimcount.py, via PIN3);
 * needs a fakematch.txt row.
 *
 * -------- THE SCHED2 BOUND IS CONFIRMED, AND NOW DERIVED FROM THE TRACE --------
 * Taken with -fsched-verbose=6 on THIS body, the decision is at t = 56:
 *     ;;  Ready list (t = 54):  549  1101  545   -> schedules 545
 *     ;;  Ready list (t = 56):  551  1101  549   -> schedules 549  (ROM wants 1101)
 *     ;;  Ready list (t = 58):  1101 551         -> schedules 551
 * and the insns, read out of the dump:
 *     549  (set (reg r0) (mem:SI (plus (reg sp) 32) 0))      arg 0, `ctx`
 *     551  (set (reg r1) (reg/v:SI 9 r9))                    arg 1, `base`
 *     1101 (set (reg r4) (mem:SI (plus (reg sp) 24) 22))     the call target
 *     594 = site-1 *call_indirect, 556 = site-2's
 *     1104 (set (reg r0) (reg sl))       = idx 227, the next r0 write
 *     1110 (set (reg r4) (const_int 32)) = idx 233, site 2's 0x20 stack arg
 *
 * *** ALL FOUR RUNGS TIE BY CONSTRUCTION, which is stronger than this header's
 * previous claim. ***  Each fill's INSN_DEPEND is exactly {its own call, the
 * next write of its own register, the site-2 call} -- three, and NECESSARILY
 * three and equal, because sched-deps links a set only to the NEXT set of the
 * same register and a CALL_INSN depends on every preceding set of a
 * call-clobbered one.  Priority ties at 35 for all three: all are dominated by
 * the same site-2 call (prio 34, cost 1).  The CLASS rung demotes only 551,
 * anti-dependent on 545 = `str r1,[sp,#4]`, the last stack-arg store.  So LUID
 * decides, and `load_register_parameters` emits the fills FORWARD with
 * LOAD_ARGS_REVERSED defined by no target, so LUID(549) < LUID(551) < LUID(1101).
 * I ALSO CHECKED THE OBVIOUS ESCAPE: reordering the two stack-arg stores makes
 * 549 class 2 and hands 1101 the first slot -- AND IT STILL FAILS, because the
 * next pair is 549 against 551 with everything tied and LUID picking 549 while
 * the ROM picks 551.  The matching site-2 call (idx 239-241) is the same three
 * insns with the same ties scheduled in LUID order, which is exactly why it
 * matches.  @224 IS AN ARGUMENT-EXPANSION-ORDER QUESTION.  That stands.
 *
 * -------- REFUTED: THIS HEADER'S ALIAS-SET PARAGRAPH --------
 * It says "*** ALL THREE MEMs ARE ALREADY IN ALIAS SET 0, THE WIDEST THERE IS ***".
 * At sched2 the target load is `(mem:SI (plus (reg sp) (const_int 24)) 22)` --
 * alias set 22, the `DrawFn dfs[2]` array's own set.  Only `ctx`'s load is set 0.
 * The CONCLUSION survives (the three fills have no mutual memory dependence),
 * but the load-bearing reason is the one the header also gives -- *dead between
 * two frame MEMs at constant disjoint offsets*, which memrefs_conflict_p
 * excludes whatever the sets say.  Strike the alias-set sentence, keep the
 * offset one.  (The brief's ADDENDUM question is therefore answered: the
 * alias-set lever is not live here, but not because every set is 0.)
 *
 * -------- MEASURED THIS BATCH AT THE 2-OF-402 BASELINE (the old lists were at 8) --------
 * EXACTLY INERT at 2, size 0, relocations ok -- free to keep or drop:
 *   an r4 pin on the first call's target assigned immediately before the call;
 *   an unpinned `DrawFn f0 = dfs[0]` local; r4 pins on BOTH calls; 0x28/0x20
 *   named into shared locals; `ctx` named into a block local (shared, and one
 *   per call); the SECOND call's target named into a local BEFORE the first
 *   call; both targets named before the first call; `0x28` written `0x20 + 8`;
 *   `&apos` named into a pointer.
 * WORSE: an r0 pin for `ctx` 8; an r1 pin for `base` 8; r4+r1 crossed 8; r4+r0
 *   crossed 8; `apos.x / 2 - 0x10` named once 196 AT 8 BYTES SMALLER; the two y
 *   offsets named 168; `base` named into a block-local 166.
 * So the pin, naming and dependent-count levers are exhausted at the NEW
 * baseline too -- and the "give 1101 a fourth dependent" idea is DEAD FOR A
 * REASON, not for want of trying: after the site-1 call `reg_last_sets[r4]` is
 * the call itself, so no later insn can ever depend on 1101.
 *
 * WHAT IS LEFT, stated as a question rather than a count of attempts: a
 * spelling in which `ctx` reaches r0 WITHOUT an `emit_move_insn` from
 * `load_register_parameters` at its forward position.  I could not find one and
 * I do not think one exists for an ordinary register argument -- when the
 * argument's pseudo is spilled, reload rewrites that fill IN PLACE instead of
 * emitting a new insn, so the forward LUID survives either way.  If that is
 * genuinely closed then this park closes, but it wants the citation, not
 * another sweep.
 * =====================================================================================
 */
/* BaseAnim_Tackle -- NON-MATCHING, 2 ENCODINGS OF 402 (was 8).  SIZE EXACT
 * (916 bytes), INSTRUCTION COUNT EXACT (402), FRAME EXACT (`sub sp, #0x48`),
 * RELOCATIONS EXACT.  ONE function, no .rodata -- CONVERTS WHOLE when it lands,
 * no split, no data work.  PRODUCTION-FLAG FIGURE (the generic asm/%.o: src/%.c
 * rule; no per-file flag group, no flag row).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/dfa18_Tackle.c \
 *     asm/rom_c9000/rom_dfa18_c_c_c_c_a.s --func BaseAnim_Tackle
 *   XX ENCODINGS differ in 2 place(s) (ref 402, ours 402)
 *      idx 223 ref 9c06 ldr | ours 9808 ldr
 *      idx 225 ref 9808 ldr | ours 9c06 ldr
 *
 * NOT POOL-INFLATED: both differing encodings are real instructions (checked
 * with a disassembling per-index differ, batch 316).
 *
 * ================= BATCH 316: WINDOW @58 IS CLOSED.  8 -> 2. =================
 * THE EDIT, AND IT IS ONE DECLARATION:
 *     was   DrawFn d1;  DrawFn d0;        d0 = *q0p;  d1 = *q1p;
 *     now   DrawFn dfs[2];                dfs[0] = *q0p;  dfs[1] = *q1p;
 * with the three `d0(...)` / one `d1(...)` call sites spelled `dfs[0](...)` /
 * `dfs[1](...)`.  A two-member struct (`struct { DrawFn a, b; } dfs;`) is
 * BYTE-IDENTICAL to the array, so the aggregate KIND is free; what matters is
 * that the two pointers are ONE ADDRESSABLE OBJECT rather than two scalars.
 * The array must keep the two scalars' DECLARATION SLOT: declared after `view`
 * instead it reads 9, so the declaration-order rule still governs.
 *
 * WHY, AND IT RETIRES THIS PARK'S OWN STRUCTURAL-IMPOSSIBILITY ARGUMENT.
 * The previous header closed @58 like this: insn 1011 stores to an alias-set-0
 * reload SPILL SLOT, insn 126 loads `(mem (reg r3) 19)`, true_dependence holds,
 * so sched-deps emits a TRUE dependence 1011 -> 126; the ROM's order puts 126
 * BEFORE 1011; sched2 cannot hoist an insn above its own producer; therefore
 * the ROM's grouping is unschedulable and the blocker is reload's spill-store
 * placement, one pass earlier.  EVERY STEP OF THAT IS SOUND ABOUT THE INSN
 * CHAIN IT WAS READING -- AND IT WAS THE WRONG CHAIN.  Make the two pointers
 * one addressable aggregate and insns 1011/1014 never exist: the values are
 * read from a frame object by source-level loads, there is no spill-store/load
 * pair to order, and the window comes out exact.
 * *** A STRUCTURAL-IMPOSSIBILITY ARGUMENT IS ONLY AS GOOD AS ITS CLAIM THAT THE
 * INSN CHAIN IS FORCED.  THIS PARK EVEN WROTE THE ESCAPE DOWN -- "the only route
 * left is a source shape in which d0 and d1 are NOT both spilled at their defs"
 * -- AND THEN DISMISSED IT ("the ROM does spill both, so that route is almost
 * certainly closed too").  The ROM does put both in the frame.  It does not put
 * them there AS SPILLED SCALARS. ***
 * The park's own stack map was the evidence, unread: `d1(0x1c), d0(0x18)` are
 * ADJACENT AND 4 APART, and their sources `q0p = pt+0xb8` / `q1p = pt+0xbc` are
 * adjacent too.  TWO SCALARS WERE ONE OBJECT.
 * Confirmation in the RTL: with the aggregate, the call's address operand reads
 * `(subreg:SI (reg/v:DI 36) 1)` -- the 8-byte object is ONE DImode pseudo, which
 * is exactly why the per-scalar spill stores disappeared.
 * SUPERSEDED, do not re-read: the whole "WINDOW @58" section of the old header,
 * including its inert list (pinned r2/r3 on the loaded values, pinned r2/r3 on
 * q0p/q1p, dropping the q0p/q1p locals) and its worse list (swapping the d0/d1
 * and q0p/q1p assignment orders).  All of it measured a chain that no longer
 * exists.
 *
 * ============ WINDOW @224 (THE SURVIVING TWO) -- OPEN, AND RE-DIAGNOSED ============
 * ref   ldr r4,[sp,#24] / mov r1,r9 / ldr r0,[sp,#32]
 * ours  ldr r0,[sp,#32] / mov r1,r9 / ldr r4,[sp,#24]
 * -- the other three `_call_via_r4` sites match.  From .23.sched2
 * (-fsched-verbose=6) on the NEW candidate:
 *      insn  prio  dep  INSN_DEPEND
 *       549    35    3  594 1104 556     (ldr r0,[sp,#32]  = argument 0, `ctx`)
 *       551    35    3  594 1107 556     (mov r1,r9        = argument 1)
 *      1101    35    3  594 1110 556     (ldr r4,[sp,#24]  = the call target)
 * WHICH TWO COMPETE AND WHICH RUNG: 549 against 1101.  Priority ties at 35; the
 * CLASS rung ties at 3 (neither is in INSN_DEPEND of the last-scheduled insn);
 * the DEPENDENT-COUNT rung ties at 3; INSN_LUID decides -- rank_for_schedule's
 * last line, haifa-sched.c:4029-4113.  Note for every sched2 claim in this tree:
 * the ladder is priority -> CLASS vs last_scheduled_insn -> dependent count ->
 * INSN_LUID, and INSN_REG_WEIGHT is DEAD after reload (`!reload_completed`).
 *
 * WHY LUID CANNOT BE WON, AND WHY THE RESIDUE IS NOT A SCHEDULING PROBLEM.
 * Insn 1101 is a RELOAD insn: the call's address operand is a spilled pseudo and
 * reload1.c's `emit_reload_insns` emits input reloads IMMEDIATELY BEFORE the
 * insn that needs them, while `load_register_parameters` (calls.c:1684-1696) has
 * already emitted every argument fill ahead of the call -- forward, argument 0
 * first, because *** LOAD_ARGS_REVERSED IS DEFINED BY NO TARGET IN THIS COMPILER
 * *** (grep: calls.c's own #ifdef, tm.texi, ChangeLog.0, nothing else).  So
 * LUID(1101) > LUID(549) is FORCED.
 * AND THE STRONGER FACT: the ROM's order is 1101, 551, 549 -- the EXACT REVERSE
 * of LUID order -- and NO READY-LIST RANKING CAN PRODUCE IT.  Grant 1101 the
 * first slot by any means; last_scheduled_insn is then 1101, INSN_DEPEND(1101) =
 * {594, 1110, 556} contains neither 549 nor 551, so both are class 3, both have
 * three dependents, and LUID picks 549.  THE ROM PICKS 551.  Therefore the ROM's
 * three insns were never simultaneously ready: its CHAIN ORDER differs, so its
 * EXPAND order differs.
 * *** @224 IS AN ARGUMENT-EXPANSION-ORDER QUESTION, NOT A SCHEDULING ONE.  Stop
 * probing pins, alias sets and the schedule. ***
 * NEXT STEP: find a spelling in which argument 0 (`ctx`) needs NO fill insn at
 * its expand position, so the only fills emitted for this call are argument 1
 * and the target.
 *
 * THE BRIEF'S ALIAS-SET NEXT STEP FOR @224 IS REFUTED AT THE RTL.  From
 * .19.flow2: insn 541 `(set (mem/f:SI (plus (reg 13 sp) (const_int 4)) 0) ...)`,
 * insn 543 `(set (reg r0) (mem:SI (plus (reg 13 sp) (const_int 32)) 0))`, insn
 * 1086 `(set (reg r4) (mem:SI (plus (reg 13 sp) (const_int 24)) 0))`.
 * *** ALL THREE MEMs ARE ALREADY IN ALIAS SET 0, THE WIDEST THERE IS. ***  No
 * dependence exists between them not because of the alias sets but because of
 * the ADDRESS comparison: sp-based MEMs at fixed, disjoint offsets, which
 * memrefs_conflict_p excludes whatever the sets say.  A union member changes
 * nothing.  SECOND BOUND ON THE ALIAS-SET LEVER, beside the pool-load bound:
 * *** IT IS ALSO DEAD BETWEEN TWO FRAME MEMs AT CONSTANT DISJOINT OFFSETS. ***
 *
 * MEASURED THIS BATCH ON @224, all at the OLD baseline of 8 and all negative:
 * the r4 pin POSITION-SWEPT -- immediately before the call 8 (inert, as the old
 * header recorded); before GetBattleActorPos3, spanning both calls, at the loop
 * top, taken from `*q0p`, and applied to both calls, ALL 16 WITH A RELOCATION
 * DIFFERENCE (an r4 pin held across the site collides with `_call_via_r4`'s own
 * r4); the unpinned control 8; r4+r0 pinned together 197.  Also 346/347/329 with
 * relocation differences for `volatile` on one or both pointers and for calling
 * through `(*q0p)(...)` at the site.  So the pin lever is genuinely exhausted
 * here and the old header's single inert entry was not a one-at-a-time artefact.
 *
 * LEVER 5 (callee return type) IS EXHAUSTED, RE-SWEPT AT THE NEW BASELINE.
 * A rejected list is only rejected at the baseline it was measured on, so all
 * TWENTY `extern void` callees were re-flipped to `extern int` on the new
 * candidate: 16 INERT at 2; WORSE Func_80cd52c 4, UpdateScreenShake 4,
 * Func_80df9d0 5, LoadVFXFile 9.  Same verdict as at 8, now established at 2.
 *
 * ============ THE LEVERS THAT GOT THIS PARK HERE, all still true ============
 *     WHERE A sched2 WINDOW IS A PERMUTATION AGAINST A COMPILER-GENERATED
 *     OPERAND, GIVE THAT OPERAND A SOURCE STATEMENT BY PINNING IT TO THE HARD
 *     REGISTER THE ROM USES.
 *   @75  CLOSED (12 -> 10).  `{ register unsigned char *b0 __asm__("r0");
 *        b0 = base; Func_80df9d0(b0, gBuffer, 0x28, arg2); }`
 *   @305 CLOSED (10 -> 8).  `register vec3_t *pp __asm__("r7")` declared in the
 *        j-loop block, assigned `pp = &pos;` BETWEEN `j = 0;` and
 *        `p = (Part *)(base + (0xe1 << 7));`, then passed.  The PIN is what
 *        works: an unpinned `vec3_t *pp = &pos` local measures 148 and 4 bytes
 *        larger, because unpinned it takes a spill slot.
 * The same r0 edit landed Anim_Vine, so this is bank-wide.  And the lever has a
 * PRECONDITION, learned on OvlFunc_968_2009af0 this batch: the pin register must
 * not be contested across a call in the pinned local's live range, or the frame
 * grows and the figure explodes.
 *
 * ============ THE DECLARATION-ORDER AND PIN LEVERS, all unchanged ============
 * THE ROM'S STACK LAYOUT TELLS YOU THE SOURCE'S DECLARATION ORDER DIRECTLY.  The
 * frame grows downward, so declared ARRAYS get the high offsets in REVERSE
 * declaration order and SPILLED SCALARS then fill downward in ASCENDING PSEUDO
 * NUMBER, i.e. declaration order.  Reading the ROM's slots high to low gives
 * `ctx(0x20), dfs[1](0x1c), dfs[0](0x18), view(0x14), gfx(0x10), hitp(0x0c),
 * slot(0x08)`.  AND READ THAT MAP FOR ADJACENCY, NOT JUST ORDER: two slots that
 * are adjacent, same-sized and fed from adjacent sources are ONE AGGREGATE, and
 * reading them as two spilled scalars is what cost this park several batches.
 * One reorder took 204 -> 147.  Unspilled locals consume a pseudo but no slot,
 * so they can sit anywhere.
 * A BLOCK-SCOPED DECLARATION GETS A LATER PSEUDO NUMBER THAN A COMPILER TEMP and
 * therefore a lower spill slot: moving `Desc **slot` into a block opened AFTER
 * the `&hit` statement made `slot` the later pseudo, 37 -> 32.
 * "ASSIGN THE `base + K` POINTER LAST" is a repeatable statement-order lever and
 * was worth 26 -> 12 on its own: `j = 0` before `p = base + (0xe1<<7)`
 * (26 -> 23); `frame = 0` before `slot = base + 0x7828` (20 -> 15); `i = 0`
 * before `p = ...` (15 -> 14); a named `msk = 0xff` between them (14 -> 12).
 * `&x` PASSED DIRECTLY AS A CALL ARGUMENT, WITH THE POINTER LOCAL ASSIGNED
 * AFTERWARDS, IS A DIFFERENT SHAPE from passing the local: `f(..., &hit);
 * hitp = &hit;` gives compute-into-reg / copy-to-arg / store-to-slot, worth
 * 46 -> 37.
 * `base` IS r9 HERE and the pin is load-bearing (removing it costs 47 -> 224);
 * also pin the frame counter to r11 and the inner particle counter to r8.
 * A PINNED CALL-CLOBBERED REGISTER BREAKS cse1's CONSTANT SHARING, because
 * cse1's `invalidate_for_call` kills the equivalence at the intervening call --
 * `{ register int k3 __asm__("r3"); k3 = 0x7828; ... }` measured 47 -> 46 and
 * produced the ROM's two separate pool loads from one word; the same form works
 * for a SYMBOL in a register-offset load (`register char *tb __asm__("r4")` for
 * Data_ede48, 23 -> 20).
 * WRITE DESTRUCTIVE SHIFTS AS SEPARATE STATEMENTS: `sz >>= 4; sz += 2;` gives
 * the ROM's `asr r5,#4 / add r5,#2`.
 * NAMING ONE STRUCT FIELD INTO A LOCAL BEFORE A 6-ARGUMENT INDIRECT CALL was
 * worth 57 -> 47 (`int hx = hitp->x;`); naming a SECOND field in the same call
 * was 57 -> 114.  APPLY ONE FIELD AT A TIME.
 * A CONSTANT INDEX INTO A DATA SYMBOL GETS FOLDED INTO THE POOL WORD AND objcmp
 * CANNOT SEE IT: `(char *)Data_ede48 + (h - 2)` emitted a WRONG ADDEND on an
 * R_ARM_ABS32 while every instruction read correctly.  Hoist the index to a
 * local (`ix = h - 2;`).  A REAL objcmp BLIND SPOT.
 * Tackle dispatches on `variant` with a `switch` + BARE `default:` -- a bare
 * `default:` is what suppresses the jump table, the OPPOSITE of
 * Anim_UnleashIntro's recorded lever.  Read the branch polarity per function.
 * -fno-schedule-insns2 is 266, so sched2 is required.
 *
 * MEASURED NEGATIVES, do not re-run: unpinned `base` 224; `hitp` hoisted to the
 * top 367; a region-C `Desc *` named local 229; `slot` reused across regions C
 * and D 234; `slot` assigned before region C 229; `slot` assigned inside the loop
 * body 238; no `slot` local at all 110 at frame 0x44 (the address is SUNK INTO
 * THE LOOP BODY, not hoisted -- so it was never a "two pool loads" route); a
 * named `int co = 0x7828` inert; `ix + (char *)Data_ede48` inert; dropping the
 * q0p/q1p address locals inert; an r4 pin on `slot`'s constant inert; an r3 pin
 * on `frame = 0`'s zero inert; a `char *tbl` table local inert; an explicit
 * `vec3_t *pp = &pos` local 148 with size 4 larger; `bp = base` before
 * Func_80df9d0 310.
 *
 *
 * CAUTION ON EVERY NEGATIVE ABOVE: each was measured at the baseline current
 * when it was taken (355, 204, 57, 47, 12 or 8), NOT at 2.  Batch 316 showed
 * twice over that a list rejected at one baseline is UNTESTED at the next -- and
 * that one-at-a-time testing is why @58 survived five batches.  Re-measure
 * before quoting any of them as a floor.
 *
 * No .sym entry is warranted.  No per-file Makefile flag override applies.
 * Progression: 355 -> 311 -> 204 -> 147 -> 57 -> 47 -> 12 -> 8 -> 2.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

typedef struct {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    short ids[4];
} Desc;

typedef struct {
    int x;
    int y;
    int z;
    int dx;
    int dy;
    int dz;
    int life;
} Part;

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern int *iwram_3001eec[];
extern unsigned char gBuffer[];
extern unsigned char gPtrs[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void Func_80df9d0(void *a, void *b, int c, int d);
extern void Func_80df90c(int a, int b, int c);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void *_GetBattleActor(int id);
extern int Random(void);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int v);
extern void _Func_80bd7dc(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(void *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void BaseAnim_Tackle(void *context, int variant)
{
    register unsigned char *base __asm__("r9");
    void *ctx;
    DrawFn dfs[2];
    char *view;
    unsigned char *gfx;
    vec3_t *hitp;
    Desc **slotA;
    unsigned char *pt;
    DrawFn *q0p;
    DrawFn *q1p;
    unsigned char *data;
    CopyFn copy;
    int fid;
    int arg;
    int arg2;
    register int frame __asm__("r11");
    vec3_t hit;
    vec3_t apos;
    vec3_t pos;

    base = (unsigned char *)((char **)&iwram_3001eec)[0];
    ctx = (void *)((char **)&iwram_3001eec)[1];
    view = *(char **)((char *)&iwram_3001eec - 0x6c);
    gfx = (unsigned char *)((char **)&iwram_3001eec)[2];
    slotA = (Desc **)(base + 0x7828);
    *slotA = (Desc *)context;
    AnimStart(0);
    if ((*slotA)->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
        BuildDraw2DFuncEx(0x2f, 7, 7, 0xb, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 2);
        BuildDraw2DFuncEx(0x2f, 7, 7, 0xf, 2);
    }
    pt = gPtrs;
    q0p = (DrawFn *)(pt + 0xb8);
    q1p = (DrawFn *)(pt + 0xbc);
    dfs[0] = *q0p;
    dfs[1] = *q1p;
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_99, base, 1, 0);
    arg2 = 0x90;
    arg2 <<= 1;
    {
        register unsigned char *b0 __asm__("r0");
        b0 = base;
        Func_80df9d0(b0, gBuffer, 0x28, arg2);
    }
    LoadVFXFile(FILE_bd, base, 1, 1);
    switch (variant) {
    case 0:
        fid = FILE_c2;
        break;
    case 1:
        fid = FILE_b9;
        break;
    case 2:
        fid = FILE_bb;
        break;
    default:
        fid = FILE_c0;
        break;
    }
    data = GetFile(fid);
    {
        PIN3;
        q1 = (int)data;
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    *(int *)(base + (0xef << 7)) = 2;
    arg = 0x90;
    *(int *)(base + 0x7784) = 0x4b;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    {
        Desc **s2 = (Desc **)(base + 0x7828);
        Func_80df90c((*s2)->f8, (*s2)->ids[0], 0xa);
        {
            int *ab = (int *)_GetBattleActor((*s2)->ids[0]);
            int *src = (int *)*ab;
            Part *p;
            int msk;
            register int i __asm__("r8");
            i = 0;
            msk = 0xff;
            p = (Part *)(base + (0xe1 << 7));
            do {
            p->x = src[2];
            p->y = src[3] + (0xa0 << 12);
            p->z = src[4];
            p->dx = (Random() & 0x1ff) << 11;
            p->dy = ((Random() & msk) - 0x40) << 11;
            p->dz = ((Random() & msk) - 0x80) << 11;
            if (p->x > 0) {
                p->dx = -p->dx;
            }
            p->life = i / 2 + 0x10;
                i++;
                p++;
            } while (i != 0x40);
        }
    }
    {
        register int k3 __asm__("r3");
        k3 = 0x7828;
        GetBattleActorPos3((*(Desc **)(base + k3))->ids[0], &hit);
    }
    hitp = &hit;
    {
    Desc **slot;
    frame = 0;
    slot = (Desc **)(base + 0x7828);
    do {
        if (frame <= 0xe) {
            GetBattleActorPos3((*slot)->f8, &apos);
            dfs[0](ctx, base, apos.x / 2 - 0x10, apos.y - 0x30, 0x28, 0x20);
            dfs[1](ctx, base, apos.x / 2 - 0x10, apos.y - 0x10, 0x28, 0x20);
        }
        if (frame == 0xa) {
            Func_80d6888((*slot)->ids[0], 7, 5, 0, 8);
            _SetBattleActorKnockback((*slot)->ids[0], 4);
            _Func_80bd7dc(0x86);
            *(int *)(base + 0x77a8) = 8;
        }
        if (frame >= 8 && frame <= 0x13) {
            int k = (frame - 8) / 2;
            int hx = hitp->x;
            dfs[0](ctx, gBuffer + k * 0x3c0, hx / 2 - 0x10, apos.y - 0x28, 0x14, 0x30);
        }
        if (frame >= 8 && frame <= 0x3f) {
            Part *p;
            int j;
            register vec3_t *pp __asm__("r7");
            InitMatrixStack();
            MatrixSetLook(view, view + 0xc);
            j = 0;
            pp = &pos;
            p = (Part *)(base + (0xe1 << 7));
            do {
                int sz = p->life;
                if (sz > 0) {
                    int h;
                    int ix;
                    Func_80e3944(p, pp);
                    sz >>= 4;
                    sz += 2;
                    h = sz * 2;
                    ix = h - 2;
                    pos.x = pos.x >> 1;
                    {
                    register char *tb __asm__("r4");
                    tb = (char *)Data_ede48;
                    dfs[0](ctx, gfx + *(unsigned short *)(tb + ix),
                       pos.x - sz / 2, pos.y - sz, sz, h);
                    }
                    Func_80e38b8(p, 0x3c, -0x200);
                    p->life = p->life - 1;
                }
                j++;
                p++;
            } while (j != 0x40);
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x3c);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
