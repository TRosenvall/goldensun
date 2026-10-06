/* FieldMove_NoTarget -- 7 differing encodings of 122.  PARKED, PIN-FREE.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/8096810.c asm/rom_8a000/rom_944ec_a_c_c_a_a_a_a_a.s --func FieldMove_NoTarget
 *
 * RE-DERIVED batch 326, brief D: 7 differing encodings of 122, and `--whole`
 * adds no SIZE, no INSTRUCTION COUNT and no RELOCATIONS line -- so size equal,
 * 122 instructions against 122, relocations clean.  The park's 7 is CORRECT.
 *
 * The reference holds ONE function and no data (grep -c thumb_func_start = 1),
 * so when this lands it converts the whole file with NO SPLIT, to
 * src/rom_8a000/rom_944ec_a_c_c_a_a_a_a_a.c (path currently free).
 *
 * ===== THE RESIDUE IS TWO RUNGS IN TWO PASSES.  RUNG 1 IS SOLVED AND ITS
 * ===== PREREQUISITE IS NOT IN THIS BODY.  RUNG 2 IS NOW ONE NUMBER WIDE.
 *
 * RUNG 1 -- the ldrsh scratch register.  Both halfword reads are
 * `*thumb_extendhisi2_insn` with `(clobber (scratch:SI))`; thumb has no
 * immediate-offset ldrsh, so reload fills each scratch, and the FIRST ldrsh in
 * RTL order takes r2 (reload1.c:821 sets last_spill_reg = -1 and :5003 starts
 * allocate_reload_reg's round robin there).  The ROM gives the KIND read r2, so
 * THE KIND READ MUST COME FIRST IN SOURCE ORDER.  This body reads style first,
 * which is why it is 7.
 *
 * Measured in `.19.flow2`, not `.18.greg` -- batch 325's rule that a `Using reg`
 * line is not the register you get applies here, the greg dump prints `Using reg
 * 3` for both reads:
 *     this body  insn 25 = 0x1a read -> r7, (clobber r2);  insn 30 = 0x1e -> r6, (clobber r3)
 *     ord C      insn 25 = 0x1e read -> r7, (clobber r2);  insn 30 = 0x1a -> r6, (clobber r3)
 *     ROM                 0x1e read -> r6, scratch r2;              0x1a -> r7, scratch r3
 *
 * THE BATCH-325 ESCAPE FROM RUNG 1 IS CLOSED, with evidence.  The idea would be
 * to ban r2 at the first read so the cursor hands it to the second.
 * `order_regs_for_reload` (reload1.c:1527-1537) puts into `bad_spill_regs` only
 * `fixed_reg_set` plus the hard registers in `chain->live_throughout` and
 * `chain->dead_or_set`.  At the first read the only live hard registers are r1
 * (`m`) and r5 (`p`): r0, r2 and r3 are all free.  Making r2 live there needs an
 * extra live value, which changes the push set and the instruction count.  So
 * rung 1 really does force the order, and rung 2 is the only thing left.
 *
 * RUNG 2 -- ONE ADJACENT TRANSPOSITION IN global.c's allocno_order, AND IT IS
 * NOW A 22-UNIT ARITHMETIC GAP.  Put the kind read first ("ord C") and every
 * instruction and both scratch registers are the ROM's; the only defect is that
 * `kind` and `style` have swapped r6 and r7, which reads 9.
 *
 *     this body   ;; 10 regs to allocate: 45 71 46 32 33 43 34 44 54 35   -> 7
 *     ord C       ;; 10 regs to allocate: 45 71 46 32 33 43 44 34 54 35   -> 9
 *
 * pseudo 34 is `kind`, pseudo 44 is the gState slot pointer.  r5 is taken by `p`
 * and r8 by pseudo 54, so r6 goes to whichever of 34 and 44 is allocated FIRST
 * and r7 to the other; they conflict, so they cannot share.  The two inputs,
 * READ OUT OF `.17.lreg`'s flow info and never counted in the C (batch 325's
 * rule -- all four REG_N_REFS increment sites in flow.c add loop_depth + 1):
 *
 *     pseudo 34 kind   this body 3 refs / live 18     ord C 3 refs / live 19
 *     pseudo 44 slot   both      4 refs / live 50
 *
 * allocno_compare (global.c:597-620) is
 * `(double)(floor_log2(n_refs) * n_refs) / live_length * 10000 * size`,
 * truncated to int, ties on the allocno number:
 *
 *     this body  kind (1*3/18)*10000 = 1666  >  slot (2*4/50)*10000 = 1600  -> kind first, kind r6 (ROM)
 *     ord C      kind (1*3/19)*10000 = 1578  <  slot 1600                   -> slot first, slot r6 (defect)
 *
 * SO THE WHOLE OF RUNG 2 IS ONE INSN OF LIVE LENGTH.  It needs
 * live_length(kind) <= 18 (1666 > 1600) OR live_length(slot) >= 51
 * (80000/51 = 1568 < 1578).  A tie is unreachable: with slot at 50 a tie needs
 * live_length(kind) = 18.75.
 *
 * WHY IT RESISTS -- THE TWO LIVE RANGES ARE NESTED.  `slot` is defined inside
 * case 9 and dies at `*slot = a`; `kind` is defined in the entry block and dies
 * at the `Func_808df1c` SECOND-ARGUMENT MOVE, which is INSIDE slot's range.
 * Therefore:
 *   - shortening kind means deleting an insn from [kind def, slot def) -- the
 *     entry-block tail and the switch dispatch -- where the only movable insn is
 *     THE STYLE READ, which rung 1 requires to be after kind; or from
 *     [slot def, kind last use], which shortens slot by the same amount and
 *     cannot change the sign.
 *   - lengthening slot means adding an insn to (kind last use, slot last use],
 *     a window holding only the Func_809ae3c / Func_808d5a4 / Func_80970f8 /
 *     Field_Halt_Target / Func_809ad90 calls, and moving any of them changes an
 *     emission order the ROM already matches.
 * That is a GEOMETRIC fact about the two ranges, not a spelling question, which
 * is why forty-plus inert rows were inert.  DO NOT RE-SWEEP SPELLINGS.
 *
 * MEASURED batch 326 on top of ord C, with the allocator inputs beside each
 * figure (figure | slot refs/live | kind refs/live):
 *     ord C itself                                         9 | 4/50 | 3/19
 *     `p, kind, m, style`                                  9 | 4/50 | 3/21  worse: m's 2 insns enter kind's range
 *     `slot = (short *)(gState + 0x24a)` before `g`   BROKEN 328 bytes, 5/50 -- folds gState+0x24a into its own pool word
 *     same but after `g`                                   9 | 4/50 | 3/19  cse folds it back
 *     `v` read through `g` not `slot`                      9 | 4/50 | 3/19  confirms the park: cse restores the ref
 *     `*slot = inval` through `g`                          9 | 4/50 | 3/19
 *     `*slot = a` through `g`                              9 | 4/50 | 3/19
 *     `inval = 0xffff` moved between the two reads         9 | 4/50 | 3/20  worse
 *     `slot`/`g` declaration order swapped                 9 | 4/50 | 3/19
 *     `Func_808df1c` hoisted above the guard          BROKEN 324 bytes, kind 3/15
 *     `short v`                                            9 | 4/50 | 3/19
 *     the guard's two statements swapped                  11 | 4/52 | 3/20
 *     one local for both `g + 0x1f4` loads                72 | 4/48 | 3/19
 *     `Func_809ad90(a)` after `*slot = a`                 11
 *     `Field_Halt_Target(a)` after `*slot = a`            11
 *     `slot` at function scope                             9 | 4/50 | 3/19
 *     `v != -1` written `v + 1 != 0`                      11 | 4/50 | 3/19
 * The swapped-guard row DEMONSTRATES the nesting: it is the one edit that did
 * lengthen slot (50 -> 52) and it lengthened kind by exactly the same 2.
 *
 * ORD C IS THE BETTER BASE despite reading 9: it already holds both scratch
 * registers and the entire prologue schedule.  Its body is this body with the
 * two reads transposed:
 *     kind = *(short *)(p + 0x1e);
 *     style = *(short *)(p + 0x1a);
 * `p, kind, m, style` and `p, m, kind, style` are byte-identical.  Order sweep,
 * objcmp figures: style-then-kind 7, kind-then-style 9, kind-then-style-then-m 22.
 *
 * ***  DO NOT USE `register ... __asm__` AS AN INSTRUMENT IN THIS FUNCTION. ***
 * Pinning a call-crossing local to a callee-saved register changes the prologue's
 * push set and so the INSTRUCTION COUNT.  On ord C: kind to r6 reads 106 of 122
 * at 116 instructions; both pinned the same; style to r7 reads 14.  A figure
 * obtained that way is not a distance.
 *
 * ALSO MEASURED EXACTLY INERT ON ORD C in earlier batches (not to be re-swept):
 * declaration order of kind and style; `short kind`; `short style`; `short *slot`
 * as `unsigned short *`; `char *g`; the 0x24a load written through `g`; a
 * VOLATILE cast on either 0x24a store or the load (all three CSE back onto
 * `slot`, so slot's reference count is NOT reachable from the source); naming the
 * Func_808d5a4 result; naming the g+0x1f4 word; copying kind into a temp for the
 * case-9 argument; moving the case-9 block-scoped locals to function scope;
 * declaring Func_808df1c, Func_809ade8 or Field_Halt_Target int-returning.
 * WORSE ON ORD C: inverting the case-9 if/else, 17 with RELOC; reading 0x1a
 * inline in case 2, 96; dropping the style read, 98 at 120 instructions.
 *
 * TWO BANK FACTS, established elsewhere and not re-derived:
 *   - THE DECLARATION LEVER IS CONTRAINDICATED FOR gState.  A typed member at
 *     offset 500 folds a pool word the ROM builds at run time (a measured 51-line
 *     regression).  This function reads g+0x1f4 = 500.  Left as array indexing.
 *   - iwram_3001f30 is declared eleven different ways across the tree against an
 *     unused struct whose layout does not fit its users' offsets.  Left as
 *     `extern char *` here.
 *
 * WHAT THE PARK GOT RIGHT, REPRODUCED AND KEPT.  Instructions 12 through 136 --
 * the range check, the jump table, all sixteen arms, both big cases and the
 * epilogue -- are identical including every register, because of:
 *   THE NEGATIVE-OFFSET GLOBAL.  `*(T **)((unsigned char *)&iwram_3001f30 -
 *   0x74)` reproduces `ldr r3, =iwram_3001f30 / sub r3, #0x74 / ldr r1, [r3]`
 *   with no extra pool word.
 *   THE `ldr rN, =0xffff / strh` SHAPE NEEDS AN int LOCAL ASSIGNED IN A
 *   DOMINATING BLOCK -- the function's FIRST statement.  Assigned inside the arm
 *   or inside the guarded body, gcc folds it to a HImode const_int -1, commons it
 *   with the `mov #1 / neg` from the `!= -1` test and stores a register instead,
 *   three instructions short.  Hoisting it was worth 27 down to 9 on its own.
 *   Corpus template: src/rom_9000/rom_ea54_c_b.c.  This is the counter-example to
 *   the halfword-pool blocker note: the word-sized pool load for a halfword store
 *   IS reachable, by a dominating-block int local.
 *   The case-body order in the source is the emission order
 *   (1,7,11,4,5,14,6,3,12,13,9,2,8,10,15,16); the epilogue's `pop {r0}` confirms
 *   `void`; three callees take NO arguments, declared `extern int f();` and called
 *   bare, because the ROM sets up no argument registers for them.
 *
 * ============ BATCH 327, BRIEF F: THE BOUND ABOVE IS REFUTED AS A BOUND ============
 * RE-DERIVED: 7 differing encodings of 122 (ref 122, ours 122), first at index 5,
 * size equal, relocations clean.  ord C re-derived at 9 with the park's allocator
 * inputs confirmed EXACTLY:
 *     ;; 10 regs to allocate: 45 71 46 32 33 43 **44 34** 54 35
 *     Register 34 (kind) used 3 times across 19 insns; crosses 1 call; pref LO_REGS
 *     Register 44 (slot) used 4 times across 50 insns; crosses 7 calls; pref LO_REGS
 * `allocno_compare` read in full -- the transcription above is right:
 *     pri = ((double)(floor_log2 (n_refs) * n_refs) / live_length) * 10000 * size
 * **BUT THE BOUND ABOVE ("needs live(kind) <= 18 OR live(slot) >= 51") HOLDS
 * `n_refs` FIXED, AND `n_refs` IS THE OTHER NUMERATOR -- WITH A STEP IN IT:**
 *     floor_log2(3)*3 = 3   but   floor_log2(4)*4 = 8     -- a 2.67x jump at 4
 *     kind at 4 refs / 19 live -> (8/19)*10000 = 4210  against slot's 1600
 *     slot at 3 refs / 50 live -> (3/50)*10000 =  600  against kind's 1578
 * Either one wins by a mile, and NEITHER needs live_length to move at all.  The
 * "that is a GEOMETRIC fact about the two ranges, not a spelling question"
 * conclusion is correct ABOUT live_length and is NOT a closure of rung 2.
 *
 * PROVED BY PROBE (instrument, labelled -- NOT a result).  Adding `case 0: break;`
 * makes the switch's low value 0, so gcc skips the index subtraction and reads
 * `kind` directly in the dispatch:
 *     Register 34 used **4** times across **18** insns
 *     ;; 9 regs to allocate: 45 46 32 **34** 33 43 44 35 54
 * kind moves from EIGHTH to FOURTH, ahead of slot (now seventh).  The figure is
 * 110 because the extra case changes the jump table -- which is exactly why this
 * is an instrument and its number is a figure about the blocker.
 *
 * WHERE n_refs COMES FROM (so the next reader does not have to find it again).
 * `REG_N_REFS` has exactly four increment sites in flow.c and they are
 * `sets + uses`, each weighted `pbi->bb->loop_depth + 1`:
 *     :4435  the SET site (also does REG_N_SETS += 1 and REG_LIVE_LENGTH += 1)
 *     :5115  the USE site
 *     :4948  AUTO-INCREMENT ("Count an extra reference to the reg.  When a reg is
 *            incremented, spilling it is worse, so we want to make that less
 *            likely.")  -- also REG_N_SETS++
 *     :5556  AUTO-INCREMENT, adding a reference for an increment insn it DELETES
 * arm.h:1740 makes HAVE_POST_INCREMENT 1 UNCONDITIONALLY (unlike
 * HAVE_PRE_INCREMENT and HAVE_POST_DECREMENT, both TARGET_ARM), so the two
 * auto-increment sites are nominally live on thumb -- but `kind` is not a
 * pointer, so they are unreachable FOR IT.  For `kind`, 4 refs therefore means a
 * SECOND SET, or any reference at `loop_depth >= 1`.
 *
 * MEASURED batch 327 on ord C (figure | slot refs/live | kind refs/live):
 *   ALL THREE slot accesses written through `g`, `slot` deleted  9 | 4/50 | 3/19
 *       relocations clean.  cse recreates the address as COMPILER TEMP pseudo 48
 *       with the IDENTICAL 4 refs / 50 insns / crosses 7 calls.  The rows above
 *       tested these three edits ONE AT A TIME; **CROSSED, they are still exactly
 *       inert**, which is what properly closes "slot's n_refs from the source".
 *   `kind | (kind & 0)` at the Func_808df1c argument                9 | 4/50 | 3/19
 *       fold kills algebraic duplications: a second textual OCCURRENCE is not a
 *       second reference, just as a second NAME is not.
 *   the guard's two statements swapped                             11 | 4/52 | 3/20
 *       reproduces the row above.  Worth keeping as a PREREQUISITE: with slot at
 *       52 the requirement relaxes from live(kind) <= 18 to <= 19.
 *   `g = gState` hoisted above the switch                          91 | 4/25 | 3/18
 *       RELOCDIFF.  It HALVES slot's live length and shortens kind -- i.e. it
 *       moves both allocator inputs hard -- at the cost of the entry-block
 *       emission order.  Not inert, and not to be dismissed either.
 *   `g` AND `slot` both hoisted                           BROKEN 114, dsize +12
 *   `*slot = 0xffff` with `inval` deleted                 BROKEN  93, dsize -4
 *   crosses: guardswap x each of the four above           114, 90, 9, 93
 *       no cross beats its better half.
 *
 * ==> THE OPEN QUESTION IS NOW ONE SENTENCE, AND IT IS NOT THE ONE ABOVE:
 *     is there a ZERO-INSTRUCTION way to give `kind` a FOURTH reference -- a
 *     second SET, or any reference at loop_depth >= 1?
 *     If yes, rung 2 falls by a factor of 2.67 and live_length never has to move.
 * "DO NOT RE-SWEEP SPELLINGS" still stands.  The n_refs axis is not a spelling.
 * -- scratch_elev/b327/F
 *
 * ===== BATCH 330, BRIEF B: HALF THE OPEN QUESTION IS ANSWERED -- NO =====
 * RE-DERIVED: seven differing encodings of 122, index 5, ref 221e against ours
 * 3b74, counts equal, size equal, relocations clean.  Ord C re-derived too, with
 * its allocator inputs confirmed verbatim out of `.17.lreg` and `.18.greg`:
 *     Register 34 used 3 times across 19 insns; crosses 1 call; pref LO_REGS
 *     Register 44 used 4 times across 50 insns; crosses 7 calls; pref LO_REGS
 *     ;; 10 regs to allocate: 45 71 46 32 33 43 44 34 54 35
 * So the two numerators and the ordering are exactly as recorded.
 *
 * THE OPEN QUESTION WAS: is there a zero-instruction way to give kind a FOURTH
 * reference -- a second SET, or any reference at loop_depth >= 1?
 *
 * **THE loop_depth HALF IS DEAD, AND IT IS DEAD IN THE COMPILER SOURCE.**
 * loop_depth is not a note-driven quantity and no bracket-shaped construct can
 * raise it.  flow.c:7619-7634 recomputes it from scratch, zeroing every block
 * and then counting only genuine BACK EDGES found by dominator test:
 *     BASIC_BLOCK (b)->loop_depth = 0;
 *     for (e = BASIC_BLOCK (b)->pred; e; e = e->pred_next)
 *       { basic_block latch = e->src;
 *         if (latch != ENTRY_BLOCK_PTR && TEST_BIT (dom[latch->index], b))
 *           num_loops++; }
 * and the increment itself is in `find_loop_nodes_find` (flow.c:7295, :7301,
 * :7322), which walks predecessors from the latch and does `->loop_depth++` per
 * node of an actual natural loop.  THIS FUNCTION HAS NO BACK EDGE -- the switch
 * dispatch and all sixteen arms branch forward to the common exit -- so every
 * block is at depth zero and stays there.  Raising the weight on any reference
 * therefore requires a real loop, which is instructions.  That is a GEOMETRIC
 * fact about the CFG, like the live-range nesting above, and it should not be
 * probed again.
 *
 * THE SECOND-SET HALF survived everything I could aim at it, and the dimension I
 * aimed was the TYPE CONSTRUCTOR, which no earlier row varied (it is the axis
 * that paid for an unrelated park last batch: promote_mode, explow.c:895-902,
 * skips RECORD_TYPE).  IT IS EXACTLY INERT HERE, and not merely at the same
 * figure -- at the same ALLOCATOR INPUTS.  Measured on ord C, allocator inputs
 * read out of `.17.lreg` beside each figure (figure | slot refs/live | kind
 * refs/live):
 *     ord C itself                                      9 | 4/50 | 3/19
 *     unsigned int kind                                 9 | 4/50 | 3/19
 *     kind wrapped in a one-member STRUCT               9 | 4/50 | 3/19
 *     kind wrapped in a one-member UNION                9 | 4/50 | 3/19
 *     all three slot accesses through g, slot deleted   9 | 4/50 | 3/19
 *     the guard's two statements swapped               11 | 4/52 | 3/20
 *     guardswap + slot accesses through g              11
 * The struct and union rows are byte-for-byte the same object as ord C: the
 * pseudo numbering shifts by one and every ref/live pair is identical.  A
 * one-member aggregate is an ALIAS escape, not a reference-count lever.
 * NOTE for the two guardswap rows: both now also report differing RELOCATIONS,
 * which the earlier record of that row did not say.  A row with dirty
 * relocations is not a clean distance, so do not use it as a prerequisite
 * without re-checking that.
 *
 * WHY I THINK THE SECOND-SET HALF IS ALSO DEAD, stated as evidence and not as a
 * bound: REG_N_REFS counts SETs and CLOBBERs at flow.c:4435-4436 and USEs at
 * :5115, the two auto-increment sites (:4948, :5556) require the register to be
 * a MEM address register that is incremented and kind is neither, fold removes
 * every algebraic duplication (the recorded inert row above), and a second
 * textual occurrence or a second NAME commons back onto the one use.  What is
 * left is a genuine second SET or CLOBBER of kind's pseudo, and every C
 * construct that produces one also produces an instruction.  I did not find a
 * counter-example and I am not claiming none exists -- I am recording that the
 * loop_depth escape is closed, so the question is now ONE clause, not two.
 *
 * AND A DIRECTION NOBODY HAS STATED: the SAME factor-of-2.67 step can be taken
 * from the OTHER side.  Dropping slot from four references to three gives it
 * (floor_log2(3)*3/50)*10000 = 600 against kind's 1578, which is a far wider
 * margin than kind-at-four-refs needs and also leaves live_length alone.  It is
 * not obviously reachable either -- all three slot accesses are real reference
 * instructions and cse recreates the address pseudo with four references when
 * the local is deleted -- but it has never been written down as the symmetric
 * half of the open question, and a future reader should see both halves.
 * -- scratch_elev/b330/B
 */
extern char *iwram_3001f30;
extern unsigned char gState[];

extern void Field_Move(void);
extern void Field_Lift(void);
extern void Field_Carry(void);
extern void Field_Force(void);
extern void Field_Douse(void);
extern void Field_Whirlwind(void);
extern void Field_Frost(void);
extern void Field_Ply(void);
extern void Field_Growth(void);
extern void Field_Catch(void);
extern void Field_Reveal(void);
extern void Field_Cloak(void);
extern void Field_Retreat(void);
extern void Field_Avoid(void);
extern void Field_Halt(void);
extern void Field_Halt_Target(int a);
extern void Field_MindRead(int id, int style);
extern void Func_809ade8(int id);
extern void Func_808df1c(int a, int b);
extern int Func_809ae3c();
extern int Func_808d5a4();
extern void Func_80970f8(int a, int b);
extern void Func_809ad90(int a);
extern void Func_80984c0(void);

void FieldMove_NoTarget(void)
{
    char *p;
    char *m;
    int kind;
    int style;
    int inval;

    inval = 0xffff;
    p = iwram_3001f30;
    m = *(char **)((unsigned char *)&iwram_3001f30 - 0x74);
    style = *(short *)(p + 0x1a);
    kind = *(short *)(p + 0x1e);
    switch (kind) {
    case 1:
        Field_Move();
        break;
    case 7:
        Field_Lift();
        break;
    case 11:
        Field_Carry();
        break;
    case 4:
        Field_Force();
        break;
    case 5:
        Field_Douse();
        break;
    case 14:
        Field_Whirlwind();
        break;
    case 6:
        Field_Frost();
        break;
    case 3:
        Field_Ply();
        break;
    case 12:
        Field_Growth();
        break;
    case 13:
        Field_Catch();
        break;
    case 9:
        {
            unsigned char *g;
            short *slot;
            int v;
            int a;

            g = gState;
            slot = (short *)(g + 0x24a);
            v = *slot;
            if (v != -1) {
                Func_809ade8(v);
                *slot = inval;
            }
            Func_808df1c(*(int *)(g + 0x1f4), kind);
            a = Func_809ae3c();
            if (Func_808d5a4()) {
                Func_80970f8(*(int *)(g + 0x1f4), a);
                Field_Halt_Target(a);
                Func_809ad90(a);
                *slot = a;
            } else {
                Field_Halt();
            }
        }
        break;
    case 2:
        if (*(short *)(m + 0xcb8) != 0)
            Func_80984c0();
        Field_MindRead(*(short *)(p + 0x18), style);
        break;
    case 8:
        Field_Reveal();
        break;
    case 10:
        Field_Cloak();
        break;
    case 15:
        Field_Retreat();
        break;
    case 16:
        Field_Avoid();
        break;
    }
}
