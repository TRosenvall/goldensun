/* OvlFunc_959_200a69c (0x0200a69c) -- NON-MATCHING.
 *
 * NON-MATCHING, 10 of 48 STREAM encodings with FIVE SHIMS (NO register pins)
 *   -- re-measured batch 343 brief B, and the batch-336 figure reproduces.
 *   IN PHASE: 33 reference instructions against 33, size equal, and
 *   **RELOCATIONS IDENTICAL** -- all thirteen `bl` offsets line up, which they
 *   did not at twenty-nine.  So the 10 is a TRUE DISTANCE.
 *   PIN-FREE / DEVICE-FREE FIGURE, for the pin policy: **29 of 48** (the same
 *   body with the five barriers deleted, 34 instructions against 33 -- one
 *   instruction LONG, so that figure measures phase, not distance).
 *   Superseded figures: 29 device-free, 18 with nine register pins.
 *   A FULLY REGISTER-PINNED BODY OF THIS FUNCTION IS AN EXACT MATCH; its recipe
 *   is recorded below and deliberately not shipped, under the batch-319 pin
 *   policy.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e7574/200a69c.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s --func OvlFunc_959_200a69c
 *
 * >> TWO HEADER DEFECTS FIXED, BATCH 343.  (1) The CURRENT figure now goes
 *    FIRST.  The previous header opened with "29 of 48 encodings PIN-FREE" while
 *    the batch-336 section 150 lines down recorded 10, and the BODY below is the
 *    barriered 10 body -- so `tools/parkcheck.py`, `tools/piece.py` and every
 *    ranking built on them read this park as 29 when it measures 10.  The work
 *    was never lost; only the claim line was stale.  (2) THE SHIMS ARE NO LONGER
 *    INVISIBLE: `tools/shimcount.py` now reports **5 -- asm + operand** on this
 *    body, so the park's note that shimcount "reports ZERO ... fourth known
 *    blind spot" is FIXED and obsolete.  Any manifest for this park must carry
 *    pins: 5 and fakematch: true. <<
 *
 * ======== THE LEVER THAT GOT IT TO 10 (batch 336): cse.c:5741-5745 ========
 *
 * Inside `cse_insn`:
 *
 *     /* A volatile ASM invalidates everything.  *\/
 *     if (GET_CODE (insn) == INSN
 *         && GET_CODE (PATTERN (insn)) == ASM_OPERANDS
 *         && MEM_VOLATILE_P (PATTERN (insn)))
 *       flush_hash_table ();
 *
 * cse pass one drops its ENTIRE value table at such an insn, so the repeated
 * constants are no longer unified across the intervening call and each site
 * rebuilds its own, which is what the ROM does.  The prologue becomes
 * `push {lr}` and the extra `pop` disappears.
 *
 * >> THE PATTERN MUST BE A BARE `ASM_OPERANDS`.  A no-operand no-clobber
 * >> `__asm__ volatile ("")` expands to ASM_INPUT and the test above is false;
 * >> `::: "memory"` wraps the asm in a PARALLEL with the clobber and the test is
 * >> false again (measured WORSE on the TU-mate).  What works is zero outputs,
 * >> zero clobbers and at least one IMMEDIATE input, so no code is emitted:
 * >>     __asm__ volatile ("" :: "i"(0));
 *
 * ======== WHAT THE REMAINING 10 IS, AND IT IS NOW BOUNDED WITH CITATIONS ======
 *
 * Eight of the ten are ARGUMENT-SETUP ORDER at four call sites, with every
 * instruction present, no length difference and the relocations already exact.
 * `tools/aligncmp.py`: **44 of 48 aligned-equal (91.7%), 8 differing in 8
 * hunks.**  The defect is the SAME at all four sites and it is one rule: THE ROM
 * EMITS `movs r0, #<slot>` BEFORE THE `lsls` THAT COMPLETES THE SHIFTED
 * CONSTANT; WE EMIT IT AFTER.
 *
 *     site 1  ROM  movs r1,#0x80 / movs r2,#0x80 / movs r0,#0xb / lsls r1,#9 / lsls r2,#8
 *             ours movs r1,#0x80 / movs r2,#0x80 / lsls r1,#9 / lsls r2,#8 / movs r0,#0xb
 *     site 2  the same with #0xf
 *     site 3  ROM  movs r1,#0xec / movs r0,#0xb / lsls r1,#1 / movs r2,#0xb4
 *             ours movs r1,#0xec / lsls r1,#1 / movs r0,#0xb / movs r2,#0xb4
 *     site 4  ROM  movs r1,#0xec / movs r2,#0xb4 / movs r0,#0xf / lsls r1,#1
 *             ours movs r1,#0xec / movs r2,#0xb4 / lsls r1,#1 / movs r0,#0xf
 *
 * ** WHICH RUNG OF rank_for_schedule THIS IS, read out of the vendored compiler.
 * ** haifa-sched.c's `rank_for_schedule` has six rungs and FIVE OF THEM ARE
 * ** PROVABLY TIED in this window, so the residue sits on the LAST one:
 * **   1. INSN_PRIORITY (haifa-sched.c:4040-4043).  Priority is the longest
 * **      path to the block end.  `movs r1` -> `lsls r1` -> call is 2;
 * **      `lsls r1` -> call and `movs r0` -> call are both 1.  So the two
 * **      `movs r1`/`movs r2` go first (they do, on both sides) and the
 * **      contested three -- `lsls r1`, `lsls r2`, `movs r0` -- all tie at 1.
 * **   2. INSN_REG_WEIGHT (4046-4049) is guarded by `!reload_completed`, so it
 * **      is DEAD IN sched2.  This is worth stating because register pressure is
 * **      the obvious story and the rung that would express it does not run.
 * **   3. the interblock rungs (4051-4067) need `INSN_BB (tmp) != INSN_BB (tmp2)`
 * **      and this function is ONE basic block -- thirteen calls, no branches,
 * **      no loops -- so they are unreachable.
 * **   4. the CLASS rung relative to last_scheduled_insn (4069-4096) assigns
 * **      class 3 when `link == 0 || insn_cost (last, link, tmp) == 1`.
 * **      arm.md:254 gives every `core_cycles single` insn latency 1, so
 * **      insn_cost is 1 for every ALU insn here and EVERYTHING is class 3.
 * **      Dead in an ALU window -- exactly as docs/elevation.md records.
 * **   5. the DEPENDENT-COUNT rung (4098-4111).  `lsls r1` has one dependent
 * **      (the call) and `movs r0` has one dependent (the call).  Tied, and
 * **      neither can go below one.
 * **   6. INSN_LUID (4113) -- "original insn order", the stable-sort tiebreak.
 * **      THIS IS THE ONE THAT DECIDES IT.
 * **
 * ** AND THE LUID ORDER IS FIXED BY calls.c, NOT BY THE SOURCE.
 * ** `precompute_register_parameters` (calls.c:849-856) forces a FRESH PSEUDO
 * ** for any argument that is not already a REG and whose
 * ** `rtx_cost (args[i].value, SET) > 2`, whenever
 * ** `SMALL_REGISTER_CLASSES && *reg_parm_seen` -- both true of every Thumb call
 * ** that passes anything in a register.  `arm_rtx_costs` (arm.c:2075-2082)
 * ** charges a Thumb CONST_INT under 256 ZERO and a `thumb_shiftable_const`
 * ** COSTS_N_INSNS(2) = 6.  So `0x80 << 9` and `0xec << 1` are precomputed and
 * ** `0xb` / `0xf` are NOT.  `load_register_parameters` then emits the hard-reg
 * ** copies in ARGUMENT ORDER, arg 0 first (calls.c:1694-1697 -- the `#else`
 * ** branch, since LOAD_ARGS_REVERSED is not defined for ARM) -- but it runs
 * ** AFTER precompute, so the big constants' insns already sit EARLIER in the
 * ** chain and carry the LOWER LUIDs.  `movs r0` is last in RTL, so sched2's
 * ** last rung puts it last.
 * **
 * ** PREDICTION CHECKED: if the speeds were NOT precomputed, load_register_
 * ** parameters would emit r0 first, LUID(movs r0) would be 0, and replaying the
 * ** six rungs gives movs r1 / movs r2 / movs r0 / lsls r1 / lsls r2 -- the ROM's
 * ** site-1 order exactly, and movs r1 / movs r0 / lsls r1 / movs r2 for site 3,
 * ** also exactly.  So the bound is: TO MATCH, THE >= 256 ARGUMENTS MUST NOT GO
 * ** THROUGH precompute, which requires them to be REGs already -- and any
 * ** source shape that puts a compile-time constant in a variable is propagated
 * ** back to a CONST_INT by cse before expand_call sees it.  Measured below. **
 *
 * MEASURED THIS BATCH (stream terms; base is the body below at 10 of 48):
 *   the SLOT constant given its own named local at each of the four sites,
 *     `s0 = 0xb; __MapActor_SetSpeed(s0, ...)`  -- the park named the two
 *     SPEEDS and never the slot ..................... 10 of 48 (byte-equal)
 *   the same with the slot assigned BEFORE the barrier  11 of 48 (worse)
 *   the same with a barrier on either side of it ...... 11 of 48 (worse)
 *   a `do { } while (0);` anchor added before each call 11 of 48 (worse)
 *   all three arguments named as locals at every site .. 32 of 48, 33/31 insns
 *                                                        (much worse, and SHORT)
 * MEASURED, batch 336:
 *   park body, no barrier ....................... 29 of 48, RELOCS DIFFER
 *   `:: "i"(0)` before the four repeated sites ... 10 of 48  <- kept
 *   the same barrier before EVERY call ........... 14 of 48
 *   the same, skipping the first two sites ....... 10 of 48
 *   `0x10000` / `0x8000` / `0x1d8` as plain hex .. 10 of 48
 *   the two speeds assigned to named locals first  10 of 48
 *   two barriers per site instead of one ......... 10 of 48
 *
 * BOTH CHEAP AXES ARE CLOSED (tools/autosweep.py, batch 343 -- a recorded
 * negative, do not re-screen):
 *   FLAG axis    all eight per-file groups.  -fno-gcse, -ffixed-r7,
 *                -fno-strict-aliasing, -fno-strength-reduce,
 *                -fno-rerun-cse-after-loop and -fno-rerun-loop-opt are EXACTLY
 *                INERT at 10; -O1 is 16 and -fno-schedule-insns2 is 16, both
 *                worse.  sched2 being WORSE is the useful half: the production
 *                schedule is right and the residue is in what sched2 is GIVEN.
 *   ANCHOR axis  18 of 18 boundaries, all inert at 10.
 *
 * ======== THE PINNED EXACT MATCH, kept so it is not lost ========
 *
 *     #define PIN3 register int q0 __asm__("r0"); \
 *                  register int q1 __asm__("r1"); \
 *                  register int q2 __asm__("r2")
 *     { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xb; q1 <<= 9; q2 <<= 8; ...speed... }
 *     { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xf; q1 <<= 9; q2 <<= 8; ...speed... }
 *     { PIN3; q1 = 0xec; q0 = 0xb; q1 <<= 1; q2 = 0xb4; ...script... }
 *     { PIN3; q1 = 0xec; q2 = 0xb4; q0 = 0xf; q1 <<= 1; ...script... }
 *
 * Every other statement is the literal-argument form below.  The assignment
 * ORDER differs between the two pairs; that is not decoration, it is what the
 * reference's scheduling needs -- and the side-by-side above now says WHY:
 * site 3 wants `movs r0` second and site 4 wants it third.
 *
 * ======== WHAT THE PARK GOT RIGHT, AND WHAT WAS REFUTED ========
 *
 * RIGHT, and it is nearly all of it: the direction (OURS LONGER -- gcc PERFORMED
 * an optimisation the original build did not), the cause (cse pass one unifying
 * constants >= 256 into callee-saved registers), the entry condition
 * (calls.c:849-856 with arm.c:2075-2082 -- a constant argument below 256 never
 * gets a pseudo, one at or above 256 always does, and that is a one-line test on
 * any call site), the register arithmetic (global.c:987-997 -- `calls_crossed`
 * non-zero copies `used1` from `call_used_reg_set`, so the candidates are LO_REGS
 * minus call-used minus fixed, exactly two registers here; global.c:390-392 keeps
 * an as-yet-unused callee-saved register out of pass zero only), and that the
 * extra instruction is the interwork epilogue's SEPARATE second `pop`.
 *
 * REFUTED, batch 333: the batch-204 note "gcc is holding something ELSE in a
 * callee-saved register besides the two speeds" -- it was the THIRD repeated
 * constant at the two script calls, which batch 204 did not pin.
 * REFUTED, batch 336: "NEXT: nothing.  The class has no known source-level
 * lever."  cse.c:5741-5745 is a door; the park had ruled out only the ASM_INPUT
 * spelling.
 * ALSO REFUTED: the extra `pop` is NOT a declared-return-type error.  The ROM's
 * epilogue is `pop {r0}` / `bx r0`, which is `thumb_exit`'s VOID case, and this
 * park already declares `void`.  Fourteen parks in this tree do disagree with
 * their declaration; this is not one of them.
 * REFUTED, batch 333, and the inverse of the usual intuition: PRESSURE, NOT
 * SPELLING, forces the rematerialisation, so a park in this class with FEW
 * repeated constants is HARDER than one with many.  Seventeen single-use named
 * constant locals is what makes `OvlFunc_953_200a820`
 * (src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_b.c) work; this function has only
 * three repeated constants, two of which fit the two available registers, so
 * naming them at the top makes it WORSE -- thirty-nine instructions, with gcc
 * reaching past the low registers to a high one.
 *
 * PIECE MEMBERSHIP (tools/piece.py, re-verified batch 343): FOUR functions in
 * asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s -- this one, OvlFunc_959_200a718
 * (55 of 63, parked at src/non_matching/overlays/200a718.c), OvlFunc_959_200a7b0
 * (753 of 835) and OvlFunc_959_200b054 (no claim).  All four parked.  This is the
 * FIRST member and `split_s.py --dry-run` writes only `_b.s` (this function) and
 * `_c.s` (the other three), with no `_a.s`.  >> IT SPLITS OUT ALONE.
 *
 *   >> NEXT: pass 3.  The eight-encoding residue is rank_for_schedule's LUID rung
 *      and the LUID order is decided inside expand_call, so there is no source
 *      spelling left to try -- the five barriers and the nine-pin exact match are
 *      both pass-3 debt and belong in reports/pass3-depin.md together.  Anyone
 *      reopening this should attack the LUID order, not the schedule. <<
 */
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);
extern int __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __SetCameraTarget(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_959_200a69c(void)
{
    __Func_8093500(0xb, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __asm__ volatile ("" :: "i"(0));
    __MessageID(0x247c);
    __ActorMessage(0xd, 0);
    __asm__ volatile ("" :: "i"(0));
    __MapActor_SetSpeed(0xb, 0x80 << 9, 0x80 << 8);
    __asm__ volatile ("" :: "i"(0));
    __MapActor_SetSpeed(0xf, 0x80 << 9, 0x80 << 8);
    __asm__ volatile ("" :: "i"(0));
    __Func_809218c(0xb, 0xec << 1, 0xb4);
    __asm__ volatile ("" :: "i"(0));
    __Func_809218c(0xf, 0xec << 1, 0xb4);
    __SetCameraTarget(0xb, 1);
    __MapActor_WaitMovement(0xb);
    __MapActor_SetAnim(0xb, 4);
    __CutsceneWait(0x1e);
}
