/* OvlFunc_883_2009490 -- NON-MATCHING, 6 differing STREAM encodings of 75.
 *
 * ref 75 / ours 75 stream encodings; 51/51 16-bit instructions -- IN PHASE,
 * so a true DISTANCE.  objcmp --whole reports NO relocation difference, so this
 * is a plain distance, not the dirty-relocation shape batch 338 found.
 * Re-measured at the head of batch 339.
 *
 * THE RESIDUE IS THREE ADJACENT TRANSPOSITIONS, all at __MapActor_Emote, at
 * stream indices 14, 22 and 32:
 *     rom   mov r2,#0x14 / mov r0,#0x16 / lsl r1,#0x1   / bl __MapActor_Emote
 *     ours  mov r2,#0x14 / lsl r1,#0x1  / mov r0,#0x16  / bl __MapActor_Emote
 *     rom   mov r2,#0x28 / mov r0,#0x0  / lsl r1,#0x1   / bl __MapActor_Emote
 *     ours  mov r2,#0x28 / lsl r1,#0x1  / mov r0,#0x0   / bl __MapActor_Emote
 *     rom   mov r2,#0x0  / mov r0,#0x16 / ldr r1,=0x103 / bl __MapActor_Emote
 *     ours  mov r2,#0x0  / ldr r1,=0x103 / mov r0,#0x16 / bl __MapActor_Emote
 * All 45 other instructions are exact.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_780898/2009490.c \
 *     asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_a.s --func OvlFunc_883_2009490
 *
 * EXACT WITH THREE PINS *AND* THE `void` RETURN TYPE -- MEASURED, byte-identical
 * (75 encodings and 21 relocations identical).  BOTH halves are required:
 *
 *   1. `extern void __MapActor_Emote(int slot, int a, int b);` -- ALREADY IN
 *      THIS BODY.  It is EXACTLY INERT ALONE at 6 of 75 and it is a
 *      PREREQUISITE, not a dead end.
 *   2. `register int eN __asm__("r0") = <slot>;` at each of the three sites.
 *      r0 only -- no r1 pin is needed here, so 3 pins, not 6.
 *
 * Candidates kept at scratch_elev/b339/A/v/9490_void_pin_r0.c (3 pins) and
 * v/9490_void_pin.c (6 pins); both are byte-identical.
 *
 * WHY THIS PARK IS *NOT* ON THE SAME RUNG AS 200bd40 AND 20082b4, which is why
 * the pin alone is EXACTLY INERT.  From the sched2 dependence table for the
 * first Emote site:
 *
 *     insn 152  mov r1,#0x80   prio 145   dependents 2
 *     insn 153  lsl r1,#1      prio 144   dependents 3   <- 141, 40, 35
 *     insn  30  mov r0,#0x16   prio 144   dependents 2   <- 141, 35
 *     insn  34  mov r2,#0x14   prio 144   dependents 4
 *
 * The contest between `lsl r1,#1` and `mov r0,#0x16` is settled on the
 * DEPENDENT-COUNT rung (haifa-sched.c:4098-4112), ONE RUNG ABOVE the LUID
 * tie-break -- so no amount of LUID movement reaches it.  Verified directly
 * rather than inferred: with the r0 pins in place the dump shows insn 30 HAS
 * moved to the front of the group (LUID changed) and the output is bit-for-bit
 * unchanged.
 *
 * THE ASYMMETRY IS THE CALLEE'S RETURN VALUE.  Declared `int`, the call matches
 * *call_value_insn (240) and SETS r0, so r0's chain is cut AT the call and
 * `mov r0,#0x16` never acquires the output dependence onto the next r0 writer --
 * 2 dependents against `lsl r1,#1`'s 3, because r1 is not a return register and
 * its `mov r1,#0x2` for the following call still depends on the shift.  Declared
 * `void` the call matches *call_insn (239), sets nothing, the output dependence
 * survives, both sides reach 3, the rung TIES, and the LUID tie-break below it
 * becomes reachable -- which is what the r0 pin then wins.  The return value is
 * discarded at all three sites, so `void` is a declaration choice, not a device.
 *
 * TWO CORRECTIONS TO THE PARK'S OWN DIAGNOSIS.  (1) It is NOT "POOL-LOADS-FIRST,
 * single-basic-block variant": the THIRD site's expensive argument IS a pool load
 * (`ldr r1,=0x103`) and the first two are shifts, and all three fail on the same
 * dependent-count rung, so the literal pool has nothing to do with it.  (2) The
 * park's negative "__MapActor_Emote declared `int` (return-type lever) 6" was
 * measuring the direction the body ALREADY had -- it is `void` that moves the
 * rung, and it only shows anything CROSSED with the pin.
 *
 * -fsched-verbose=6 NOTE FOR WHOEVER RETURNS: it lands in the .23.sched2 DUMP
 * FILE only when a -d flag has enabled that dump.  With -da the verbose output
 * is in <file>.c.23.sched2 and stderr is EMPTY; with no -d flag at all it goes
 * to STDERR and no dump file is written.  Both were used here.  The note in
 * docs/brief-template.md ("dump file, not stderr") is only half the rule, and
 * an agent that passes -fsched-verbose=6 alone and then hunts for a file will
 * conclude the flag did nothing.
 *
 * AUTOSWEEP, batch 339 -- BOTH CHEAP AXES CLOSED, do not re-screen.  Flag axis:
 * -ffixed-r7, -fno-strict-aliasing, -fno-strength-reduce,
 * -fno-rerun-cse-after-loop, -fno-rerun-loop-opt all inert at 6;
 * -fno-schedule-insns2 21.  Anchor axis: all 25 top-level statement boundaries
 * measured, all inert or worse, best 6 against base 6.  Full table in
 * scratch_elev/b339/A/autosweep.log.
 *
 * ===================== THE CLASS, NAMED AND CITED (batch 339 brief A) =========
 *
 * THREE of this brief's four targets are ONE class with ONE gate: the ROM emits
 * the CHEAP `mov r0, #imm` argument IN THE MIDDLE of an expensive argument's
 * two-insn build, gcc emits it LAST.  The pass is sched2, the rung is the LUID
 * tie-break, and the reason the zero always loses is in the EXPANDER:
 *
 *   calls.c:803-857  precompute_register_parameters().  For every register
 *     argument it emits, BEFORE any hard-register load, a copy into a pseudo
 *     when
 *         ! (GET_CODE (args[i].value) == REG || SUBREG of REG)
 *      && args[i].mode != BLKmode
 *      && rtx_cost (args[i].value, SET) > 2
 *      && ((SMALL_REGISTER_CLASSES && *reg_parm_seen)
 *          || preserve_subexpressions_p ())            <- calls.c:849-856
 *
 *   The fourth clause is UNCONDITIONALLY TRUE in Thumb: SMALL_REGISTER_CLASSES
 *   is `TARGET_THUMB` (arm.h:1061) and `*reg_parm_seen` is set at calls.c:817
 *   at the TOP of the same loop body, so even argument 0 sees it already 1.
 *
 *   The third clause is the whole selector.  arm.h CONST_COSTS, `case CONST_INT`
 *   with outer == SET (arm.c:2077-2085 as the macro expands):
 *       (unsigned) INTVAL < 256          -> 0
 *       thumb_shiftable_const (INTVAL)   -> COSTS_N_INSNS (2)
 *       otherwise                        -> COSTS_N_INSNS (3)
 *   So a constant in 0..255 is NEVER precomputed and EVERY other constant
 *   ALWAYS is -- note `(unsigned)`, which puts every negative constant in the
 *   "always" half.  The expensive argument's insns therefore land at the
 *   PRECOMPUTE position and the cheap `mov r0, #imm` lands at the
 *   load_register_parameters position (calls.c:1684-1753, forward order), i.e.
 *   LAST.  It carries the HIGHEST INSN_LUID of the group, by construction.
 *
 *   sched2 then ties: rank_for_schedule gives both candidates equal
 *   INSN_PRIORITY, equal class against last_scheduled_insn, equal dependent
 *   count, and falls through to `INSN_LUID (tmp) - INSN_LUID (tmp2)`
 *   (haifa-sched.c:4113-4116).  The zero loses because it is last.
 *
 *   SIGN CONVENTION, VERIFIED, because it is easy to get backwards and the
 *   whole analysis depends on it: schedule_block takes `ready[--n_ready]`
 *   (haifa-sched.c:6023) -- the LAST array element -- and debug_ready_list
 *   prints in array order, so in `Ready list (t = N): a b c` the insn actually
 *   scheduled is `c`.  Confirmed against four separate ties in these dumps.
 *
 * THE PIN-FREE ESCAPE, AND THE GATE THAT CLOSES IT HERE.  Name the expensive
 * constant as a local and local-alloc will move it to the use -- but only
 * through local-alloc.c:884-888:
 *
 *       if (REG_N_REFS (regno) == 2
 *           && REG_BASIC_BLOCK (regno) < 0
 *           && rtx_equal_p (XEXP (note, 0), SET_SRC (set)))
 *         reg_equiv_replace[regno] = 1;
 *
 *   and then local-alloc.c:928-988, where at the insn the pseudo dies it either
 *   substitutes the constant (validate_replace_rtx, which SUCCEEDS here because
 *   *thumb_movsi_insn alternative 3 takes a `K` shiftable const, arm.md:3838-3840)
 *   and deletes the init, or ELSE moves the init to just before the use.  Either
 *   way the build lands AFTER the zero's load and the ROM's order falls out.
 *
 *   `REG_BASIC_BLOCK (regno) < 0` means the set and the single use are in
 *   DIFFERENT BASIC BLOCKS -- the source comment says so outright ("If the
 *   register is only used in one basic block, this can't succeed or combine
 *   would have done it").  THIS CORRECTS AND COMPLETES THE PARK'S OWN RULE.
 *   The park said "THE BRANCH MUST BE BEFORE THE SITE"; that is TRUE and the
 *   real condition is sharper -- not a DOMINATING block, just a basic-block
 *   BOUNDARY between the assignment and the use, plus REG_N_REFS == 2 (the
 *   local must be used EXACTLY ONCE).  The park's 150-guarded / 98-straight-line
 *   sizing is therefore sound, and `tools/piece.py`-style re-sizing should test
 *   `REG_BASIC_BLOCK`, not dominance.
 *
 * THE PIN-FREE PRECEDENT IS LANDED IN THIS TREE, pin-free, with the identical
 * residue: src/overlays/rom_7c37ac/ovl_30_c_c_c_c_c_a.c (OvlFunc_938_2008184)
 * names `x = 0x80 << 8; y = 0x80 << 7;` at the top and gets
 * `lsl r2,#7 / mov r0,#0 / lsl r1,#8` -- because an if/else sits between the
 * assignments and the call.  Found by grepping the GENERATED half of asm/ for
 * the shape (187 instances of it exist in gcc's own output, which by itself
 * refutes "gcc never emits this order"); five generated files carry the exact
 * `lsl r2,#7 / mov r0,#0 / lsl r1,#8 / bl __MapActor_SetSpeed` triple, three of
 * them pin-free.
 *
 * THE PINNED RECIPE, which this tree ALREADY SHIPS for this class:
 * src/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_a_c.c, booked as a 2-pin
 * fakematch.  A bare `register int pN __asm__("rN") = ...` for the zero AND for
 * the expensive argument, DECLARED IN ARGUMENT ORDER, makes both values hard
 * registers at expand, skips the precompute entirely, and gives the zero the
 * lowest LUID.  Per docs/owner-decisions.md:182-183 ("Prefer a pin-free body...
 * park it at its pin-free figure for pass 3") this park is NOT reported as a
 * landing; the exact recipe is recorded below so the landing is one edit away
 * whenever the pin budget is settled.  See reports/pass3-depin.md.
 */
extern unsigned char gScript_883__0200e248[];
extern void OvlFunc_883_200d72c(void);

struct Actor { unsigned char pad00[0x18]; int f18; int f1c; unsigned char pad20[0x4c]; void *f6c; };

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __CutsceneWait(int n);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __Func_80925cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093054(int a, int b);

void OvlFunc_883_2009490(unsigned char *beh, unsigned char *scr)
{
    struct Actor *a;
    struct Actor *q;
    int v;

    a = __MapActor_GetActor(0x16);
    __CutsceneStart();
    __Func_809259c(0x16, 2);
    __MapActor_Emote(0x16, 0x80 << 1, 0x14);
    __Func_809259c(0, 2);
    __MapActor_Emote(0, 0x81 << 1, 0x28);
    __MapActor_SetBehavior(0, beh);
    __CutsceneWait(0xa);
    __MapActor_Emote(0x16, 0x103, 0);
    __MapActor_RunScript(0x16, scr);
    __MapActor_WaitScript(0);
    v = 0x80 << 9;
    __CutsceneWait(0x14);
    __Func_80925cc(0x16, 2);
    a->f18 = v;
    a->f1c = v;
    q = __MapActor_GetActor(0);
    q->f18 = v;
    q->f1c = v;
    __MessageID(0xfce);
    __Func_8093054(0x16, 0);
    __MapActor_GetActor(0x16)->f6c = OvlFunc_883_200d72c;
    __MapActor_SetBehavior(0x16, gScript_883__0200e248);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_883_2009490 -- asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_a.s
 *
 * BLOCKER: POOL-LOADS-FIRST, single-basic-block variant
 * (the same blocker as src/non_matching/ovl_7cb2c0/200bd10.c, same round)
 *
 * 6 of 70, same length, 64 lines exact.  All six are the SAME callee,
 * __MapActor_Emote, at three sites:
 *
 *     rom  mov r2, #0x14 / mov r0, #0x16 / lsl r1, #0x1
 *     ours mov r2, #0x14 / lsl r1, #0x1  / mov r0, #0x16
 *
 * The ROM emits the cheap `mov r0` BEFORE finishing the shifted or pooled
 * argument; gcc finishes the expensive argument first.  The function has no
 * labels at all, so there is no block that dominates the calls without being
 * their own block, and the basic-block lever is unreachable.
 *
 * MEASURED (all 70 lines, all 6 differing at position 14):
 *   plain literals                                     6
 *   __MapActor_Emote declared `int` (return-type lever) 6
 *
 * The return-type lever is worth noting as a negative here because it closed
 * the byte-for-byte identical shape on OvlFunc_881_200b130 in the same round
 * (2 of 70 -> exact, on __StartTask).  So the lever is real and it is not
 * general: it moved a pool-loaded SYMBOL address past a shift there, and does
 * nothing for a shifted integer argument here.  That distinction is worth
 * keeping -- it matches the round-5 measurement that the symbol tell governs
 * hoisting across a call rather than setup order within one.
 *
 * Best C: scratch/u9490.c.
 *)

*/
#endif
