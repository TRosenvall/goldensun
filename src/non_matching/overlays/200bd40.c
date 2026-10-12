/* OvlFunc_952_200bd40 -- NON-MATCHING, 2 differing STREAM encodings of 72.
 *
 * ref 72 / ours 72 stream encodings; 52/52 16-bit instructions -- IN PHASE, so
 * a true DISTANCE.  objcmp --whole reports NO relocation difference (20 and 20
 * identical), so this is a plain distance, not the dirty-relocation shape.
 * Re-measured at the head of batch 339; the inherited figure reproduced exactly.
 *
 * THE RESIDUE IS ONE ADJACENT TRANSPOSITION, at stream index 9:
 *     rom   lsl r2,#0x7 / mov r0,#0x0 / lsl r1,#0x8 / bl __MapActor_SetSpeed
 *     ours  lsl r2,#0x7 / lsl r1,#0x8 / mov r0,#0x0 / bl __MapActor_SetSpeed
 * Everything else in the function, all 50 other instructions, is exact.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/200bd40.c \
 *     asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_c_c.s --func OvlFunc_952_200bd40
 *
 * EXACT WITH TWO PINS -- MEASURED, byte-identical (72 encodings and 20
 * relocations identical):
 *
 *     {
 *         register int p0 __asm__("r0") = 0;
 *         register int p1 __asm__("r1") = 0x80 << 8;
 *
 *         __MapActor_SetSpeed(p0, p1, 0x80 << 7);
 *     }
 *
 * r0 ALONE IS NOT ENOUGH: measured 2 of 72, unchanged.  The expensive argument
 * has to be pinned with the zero, exactly as rom_7c6bac's header says.
 *
 * MEASURED NEGATIVES ON THIS BODY, all pin-free, all WORSE:
 *   x/y named at the top (the rom_7c37ac spelling)      54 instrs vs 52,
 *       push {r5,r6,r7,lr} vs push {r5,lr} -- x and y GET r6/r7 because this
 *       function is straight-line to the site, so local-alloc.c:886's
 *       REG_BASIC_BLOCK < 0 is false and the substitution never fires.
 *   x/y named, order reversed                            54 instrs
 *   x/y named + `do { } while (0);` after __PlaySound    54 instrs
 *   x/y named + anchor after `base = ...`                54 instrs
 *   x/y named + -ffixed-r7                               59 instrs
 *   only y named / only x named                          53 instrs
 *   x/y/z named incl. the zero                           54 instrs
 * The anchor DOES NOT create a basic-block boundary (it emits only
 * NOTE_INSN_LOOP_BEG/END; blocks come from labels and jumps), so it cannot
 * open the local-alloc route -- tested at two placements, both 54.
 *
 * AUTOSWEEP, batch 339: flag axis CLOSED -- -ffixed-r7, -fno-strict-aliasing,
 * -fno-strength-reduce, -fno-rerun-cse-after-loop, -fno-rerun-loop-opt all
 * inert at 2; -fno-schedule-insns2 13, -fno-gcse 47, -O1 60.  The anchor axis
 * was closed by batch 338's post-pass.  DO NOT RE-SCREEN EITHER.
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
extern void __PlaySound(int id);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_952_200bd40(void)
{
    unsigned char *base;
    int m;
    int v;

    m = -0x10;
    base = iwram_3001ebc;
    __CutsceneStart();
    __PlaySound(0x9e);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __MapActor_SetAnim(0, 2);
    v = *(short *)(base + (0xb6 << 1));
    if (v == 0x20) {
        __Func_80118a8(1);
        __CutsceneWait(0xa);
        __Func_809228c(0, 0, m);
    } else if (v == 0x1e) {
        __Func_80118a8(4);
        __CutsceneWait(0xa);
        __Func_8092208(0, 3, m);
    } else {
        __Func_80118a8(2);
        __CutsceneWait(0xa);
        __Func_8092208(0, 3, m);
    }
    __CutsceneWait(0x10);
    __Func_8091e9c(*(short *)(base + (0xb6 << 1)));
    __Func_80118c0(1);
    __Func_80118c0(2);
    __Func_80118c0(4);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_952_200bd40 -- 0x0200bd40,
 * asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.s
 *
 * 74 of 74 lines, TWO differing.  Candidate at scratch/Lbd40_A.c.
 * Both are the `mov r0, #0` position at the one __MapActor_SetSpeed call.
 *
 * The batch-127 argument-order lever WORKS in this function -- but only at the
 * `neg` sites.  Naming `m = -0x10;` at the top fixed all three
 * __Func_809228c/__Func_8092208 call sites, which sit after the function's
 * three-way branch.  The SetSpeed call does not move, and the reason sharpens
 * the rule:
 *
 *   THE BRANCH MUST BE BEFORE THE SITE.  This function's SetSpeed is the third
 *   instruction group in the function, before any conditional branch at all.
 *   Naming its two shifted constants at the top then makes gcc keep them in
 *   CALLEE-SAVED registers across the two intervening calls -- ours grew to
 *   `push {r5, r6, r7, r14}` against the ROM's `push {r5, r14}` -- which is the
 *   same failure the three straight-line parks show, for the same reason.
 *   Intervening CALLS are not enough; gcc will happily save and restore across
 *   them. It is the branch that makes rematerialising the cheaper choice.
 *
 * MY OWN CLASSIFICATION WAS TOO COARSE.  The batch-127 sizing counted a
 * function as "guarded" if ANY conditional branch preceded ANY site with the
 * shape.  This function has both kinds at once: guarded `neg` sites and an
 * unguarded `lsl` site.  The split is per-SITE, not per-function, and the
 * 150/98 figures should be read as an upper bound on the workable side.
 *
 * ALSO TRIED: naming only one of the two shifted constants (75 lines, worse in
 * both directions); leaving all three inline (11 differing -- the `neg` sites
 * regress, confirming the lever is doing real work there).
 *)

*/
#endif
