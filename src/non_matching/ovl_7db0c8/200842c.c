/* OvlFunc_954_200842c -- NOT MATCHING
 *
 * NON-MATCHING, 9 of 44 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7db0c8/200842c.c \
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c_c.s --func OvlFunc_954_200842c
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c.s
 * Best screen: 9 differing of 43, streams the same length.
 *
 * BLOCKER CLASS: register allocation on a shifted field read, cascading.
 *
 * The ROM reads the coordinate into one register and shifts it into ANOTHER:
 *
 *     rom   ldr r3, [r0, #0x10] / asr r0, r3, #0x14 / cmp r0, #8 / str r0, [sp, #4]
 *     ours  ldr r3, [r0, #0x10] / asr r3, #0x14     / cmp r3, #8 / str r3, [sp, #4]
 *
 * Both are the same instruction -- thumb's immediate ASR is always the
 * three-operand `asr rd, rs, #n`, and ours simply has rd == rs. The ROM reuses
 * r0, which held the actor pointer and is dead by then, for the result;
 * REG_ALLOC_ORDER prefers r3 and that is what we get. The choice propagates to
 * the `cmp`, the `str` and the second copy of the same sequence, which is six
 * of the nine.
 *
 * The other three are argument-setup ordering: `mov r3, #1 / mov r5, #0x40`
 * against our reverse, and `mov r1, #0x18 / mov r0, #0x40` against ours.
 *
 * WHAT WAS TRIED, all six byte-identical at 9 except where noted:
 *   - naming the loaded field first (`t = a->f10; y = t >> 20;`) so that the
 *     load and the shift are separate statements -- this is the spelling that
 *     usually separates two values into two registers, and here it does nothing
 *   - `dir = (y > 8) ? -0x30 : 0x30` instead of the assign-then-override form
 *   - `y < 9` instead of `y <= 8`
 *   - the shared 0x40 as a literal at both call sites instead of a local
 *     (batch 94's non-signal -- correctly makes no difference)
 *   - withholding __Func_8010704's prototype, with and without the named field:
 *     WORSE, 13 of 43 both ways
 *
 * The prototype lever is the interesting negative here. Three of the nine
 * differences are exactly the argument-move rotation it addresses, and the ROM
 * wants r0 LATER in the final call, which is the direction that usually
 * responds to withholding the declaration. It does not: it costs four more.
 * That is consistent with the softened statement of the lever in
 * docs/elevation.md -- the direction is not predictable -- and it is the second
 * function where the rotation has some other cause.
 *
 * The gState base IS correctly a local here; the `mov r2, #0xfa / lsl r2, #1 /
 * add r3, r2` sequence matches, so the fold rule from batch 94 is satisfied and
 * is not what is wrong.
 

 *
 * ===== BATCH 329 BRIEF I: RE-DERIVED AT 9; ALL NINE ARE ONE REGISTER CHOICE =====
 *
 * Re-measured unfiltered: 9 differing encodings of 44, ref 44 ours 44, first at
 * index 10, no SIZE and no POOL WORD line, so the streams are aligned.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7db0c8/200842c.c asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_c_c_a_c_c.s --func OvlFunc_954_200842c
 *
 * REFUTED: the header splits the figure into "six of the nine" allocation plus
 * "the other three are argument-setup ordering". The three are not a separate
 * cause. In the ROM the shift result lives in r0 and is stored from r0, so
 * `mov r3, #0x1` carries no dependence against `str r0, [sp, #0x4]` and the
 * scheduler is free to put it first; in ours the result lives in r3, so
 * `mov r3, #0x1` has an ANTI-DEPENDENCE on `str r3, [sp, #0x4]` and cannot
 * move. Same for the second pair: `mov r0, #0x40` is blocked in the ROM and
 * free in ours, in the opposite direction. BOTH swaps are consequences of the
 * one register choice, so the figure is 5 + 4 with one cause, not 6 + 3 with
 * two. (Also: the figure is 9 of 44, not the "9 differing of 43" in the note --
 * 43 is the instruction count, 44 the encoding count.)
 *
 * THE MECHANISM IS A PREFERENCE, NOT REG_ALLOC_ORDER DIRECTLY. The header says
 * "REG_ALLOC_ORDER prefers r3". It is right about the register and wrong about
 * the decider, and the distinction is what tells you where to push:
 *   `.18.greg` reads `;; 2 regs to allocate: 34 35` and `;; 34 preferences: 3`.
 *   34 is `y`, the only global allocno in the shift; `;; Register 41 in 3.` in
 *   `.17.lreg` is the loaded field, a BLOCK-LOCAL pseudo local_alloc put in r3.
 *   `global.c:set_preference` (:1595-1667) takes the single_set's SRC and, when
 *   its RTX format[0] is 'e', DESCENDS into the first operand -- so an
 *   `ashiftrt` qualifies -- then converts both regnos through `reg_renumber`,
 *   which makes the already-placed local pseudo count as hard reg r3. The
 *   preference is recorded there, and `find_reg` (:1103-1110) honours
 *   `hard_reg_preferences` BEFORE falling through to REG_ALLOC_ORDER (:1183).
 *   It is NOT `expand_preferences` (:828): that path requires the DYING
 *   register to be a global allocno itself, and reg 41 is local.
 * The second copy of the shape is decided by a different pass: there both the
 * load and the result are block-local (`;; Register 43 in 3.`,
 * `;; Register 44 in 3.`) and `local-alloc.c:combine_regs` (:1593) TIES the
 * destination to the dying input. Its one source-visible escape is the guard
 * `ureg >= FIRST_PSEUDO_REGISTER && reg_qty[ureg] < 0`, which the comment there
 * glosses as "not local to this block OR DIES MORE THAN ONCE" -- so a loaded
 * value with a second death breaks the tie. The ROM's program gives the load
 * exactly one use, so there is nowhere to put a second death.
 *
 * WHY r0 IS NOT REACHABLE FROM EITHER ROUTE, with the evidence attached. For
 * the preference to name r0 something allocated r0 must die in the insn that
 * defines `y`; the only thing in r0 is the actor pointer, and it dies one insn
 * earlier at the load (`REG_DEAD (reg 33)` is on insn 27, not insn 29). For
 * exhaustion to reach r0, r3, r2 and r1 would all have to conflict with `y`,
 * and `;; 34 conflicts: 34 35 36 5 13` says only r5 and sp do -- the live set
 * over `y`'s range is `y` and `dir` and nothing else. Both routes are closed by
 * measurements in the dump rather than by argument, which is how this should be
 * re-tested if a later batch disagrees.
*/
struct A { unsigned char pad00[0x10]; int f10; };

extern unsigned char gState[];
extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_954_200833c(int a, int b, int c);

void OvlFunc_954_200842c(void)
{
    unsigned char *g;
    struct A *a;
    int y;
    int dir;
    int e;

    g = gState;
    a = __MapActor_GetActor(*(int *)(g + 0x1f4));
    y = a->f10 >> 20;
    dir = -0x30;
    if (y <= 8)
        dir = 0x30;
    e = 0x40;
    __Func_8010704(0x43, 8, 3, 1, e, y);
    OvlFunc_954_200833c(0x11, 0, dir);
    __Func_8010704(0x40, 0x18, 3, 1, e, __MapActor_GetActor(0x11)->f10 >> 20);
}
