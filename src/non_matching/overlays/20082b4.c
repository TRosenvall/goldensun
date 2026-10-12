/* OvlFunc_911_20082b4 -- NON-MATCHING, 5 differing STREAM encodings of 34.
 *
 * ref 34 / ours 34 stream encodings; 28/28 16-bit instructions -- IN PHASE,
 * so a true DISTANCE.  objcmp --whole reports NO relocation difference, so this
 * is a plain distance, not the dirty-relocation shape batch 338 found.
 * Re-measured at the head of batch 339.
 *
 * THE RESIDUE IS TWO ADJACENT TRANSPOSITIONS, at stream indices 10 and 17:
 *     rom   lsl r2,#0x7 / mov r0,#0x0 / lsl r1,#0x8 / bl __MapActor_SetSpeed
 *     ours  lsl r2,#0x7 / lsl r1,#0x8 / mov r0,#0x0 / bl __MapActor_SetSpeed
 *     rom   mov r2,#0x8 / mov r0,#0x0 / mov r1,#0x0 / neg r2,r2 / bl __Func_809228c
 *     ours  mov r2,#0x8 / neg r2,r2   / mov r0,#0x0 / mov r1,#0x0 / bl __Func_809228c
 * All 23 other instructions are exact.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/20082b4.c \
 *     asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_a.s --func OvlFunc_911_20082b4
 *
 * EXACT WITH FOUR PINS -- MEASURED, byte-identical (34 encodings and 6
 * relocations identical).  TWO sites, two pins each, declared in ARGUMENT ORDER:
 *
 *     {
 *         register int p0 __asm__("r0") = 0;
 *         register int p1 __asm__("r1") = 0x80 << 8;
 *
 *         __MapActor_SetSpeed(p0, p1, 0x80 << 7);
 *     }
 *     ...
 *     {
 *         register int q0 __asm__("r0") = 0;
 *         register int q1 __asm__("r1") = 0;
 *
 *         __Func_809228c(q0, q1, -8);
 *     }
 *
 * Candidate kept at scratch_elev/b339/A/v/82b4_pin.c.  r0-only pins at both
 * sites measure 4 of 34 -- an improvement, still pinned, still not a landing.
 *
 * THE SECOND SITE IS THE SAME CLASS WITH A `neg` INSTEAD OF AN `lsl`.  -8 is
 * precomputed because arm.h's CONST_COSTS casts to `unsigned HOST_WIDE_INT`
 * before the `< 256` test, so EVERY negative constant falls in the expensive
 * half.  The mov+neg split is arm.md:3859-3865 (the `J` letter) run at
 * split_all_insns (toplev.c:3376 -- after reload_cse_regs and BEFORE sched2),
 * which is why the two halves start with adjacent LUIDs and the scheduler can
 * still separate them.
 *
 * MEASURED NEGATIVE THAT CORRECTS THE PARK: naming the two shifted constants
 * AND the -8 as locals assigned immediately before their calls is EXACTLY INERT
 * at 5 of 34.  The park says "naming the constants at the top ... is worse, not
 * better (33 -> 39 lines on OvlFunc_911_20082b4)".  That negative had expired
 * and the mechanism explains both halves: a local assigned right before the call
 * has its pseudo set at the assignment, which is still ahead of the zero's load,
 * so nothing moves; a local assigned at the TOP of a straight-line function is
 * simply allocated a callee-saved register, which is the +2 the park saw.
 *
 * AUTOSWEEP, batch 339 -- FLAG AXIS CLOSED, do not re-screen: -fno-gcse,
 * -ffixed-r7, -fno-strict-aliasing, -fno-strength-reduce,
 * -fno-rerun-cse-after-loop all inert at 5; -O1 13; -fno-schedule-insns2 13.
 * ANCHOR AXIS ALSO CLOSED: all 10 top-level statement boundaries measured,
 * all inert or worse, best 5 against base 5.  Both cheap axes are closed.
 * Full table in scratch_elev/b339/A/autosweep.log.
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
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_911_20082b4(int n)
{
    unsigned char *a;
    unsigned int q;

    a = __MapActor_GetActor(0);
    a[0x55] = 0;
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, 0, -8);
    q = (unsigned int)iwram_3001ebc;
    q += 0xe4 << 1;
    *(int *)q = 0x10;
    __Func_8091e9c(n);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_911_20082b4 -- 0x020082b4,
 * asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_a.s
 *
 * 33 of 33 lines, 5 differing.  Candidate at scratch/L82b4.c.
 *
 *      rom   lsl r2, #0x7 / mov r0, #0x0 / lsl r1, #0x8
 *      ours  lsl r2, #0x7 / lsl r1, #0x8 / mov r0, #0x0
 *
 * THIRD instance of the argument-setup-order class, after
 * src/non_matching/overlays/20099a4.c and 2008a80.c.  Everything else is exact.
 *
 * What the three together establish: gcc ALWAYS emits the `mov r0, #0` last in
 * the argument-setup block, and the ROM never does.  Across the three the ROM
 * places it in two different slots -- before both shifted builds in 20099a4,
 * between them in 2008a80 and here -- so there is no fixed convention to
 * imitate, and `--no-sched2` makes all three worse, so the ROM was built with
 * the second scheduling pass running.  The difference is in what that pass was
 * handed, which is the argument evaluation order, and nothing tried reaches it.
 *
 * TRIED across the three: naming the zero as a local; sharing one zero local
 * across the calls that take one; naming the shifted values as locals;
 * --no-sched2; -fno-schedule-insns; -fno-defer-pop; -fomit-frame-pointer.
 *
 * The class is now sized -- see docs/elevation.md.  It is worth detecting
 * BEFORE writing a candidate, because a function whose only defect is this is
 * otherwise indistinguishable from one that is nearly right for a reachable
 * reason.
 *
 * CORRECTION (added later, and it changes the class).  An earlier version of
 * this park said gcc ALWAYS emits the `mov r0, #0` last in an argument block.
 * That is wrong.  A control over the 3411 solved functions in this tree found
 * 15 that carry the exact interleaved shape, one of them the identical
 * `mov r1,#128 / mov r2,#128 / mov r0,#0 / lsl r1,#9 / lsl r2,#8 /
 * bl __MapActor_SetSpeed`.
 *
 * The construct is in src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_b.c: the
 * shifted constants are NAMED LOCALS assigned at the top of the function, and
 * -- this is the part that matters -- TWO EARLY RETURNS sit between the
 * assignments and the calls.  gcc will not keep seven constants live across the
 * guards, so it rematerialises each at its use, and the rematerialised sequence
 * interleaves the way the ROM's does.
 *
 * That is why the lever does not reach THIS function: it is straight-line.
 * Naming the constants at the top with no branch in between makes gcc keep them
 * live instead, which is worse, not better (33 -> 39 lines on
 * OvlFunc_911_20082b4, 26 -> 29 on OvlFunc_899_20099a4).
 *
 * So the blocker here is not "gcc cannot produce this order" but "this function
 * has no dominating block to produce it from".  Sized in docs/elevation.md:
 * 248 remaining functions carry the shape, 150 of them have a conditional
 * branch before the site and are worth trying the lever on; 98, including this
 * one, are straight-line at every site.
 *)

*/
#endif
