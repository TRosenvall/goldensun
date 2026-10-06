/* Func_801fd34 (StepOverlayAnimation)  --  0x0801fd34   [rom_15000]
 *
 * NON-MATCHING, 10 of 36 encodings, 36 against 36, 80 bytes against 80.
 *   *** RELOCATIONS NOW IDENTICAL.  The park's 14 had DIRTY RELOCATIONS and was
 *   therefore not a distance; this 10 is one. ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801fd34.c \
 *     asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_c_c.s --func Func_801fd34
 *   (and --whole, which agrees: `XX Func_801fd34  10 of 36 differ (ours 36)`)
 *
 * SPLIT SHAPE: none.  asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_c_c.s holds ONE
 * `.thumb_func_start` (Func_801fd34) and no data; `tools/datacheck.py` exits 0
 * silently, so when this lands it is a WHOLE-FILE conversion to
 * src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_c_c_c.c with no `split_s.py` step and no
 * object-path change.  PINS: 0.  No flag group (generic `asm/%.o: src/%.c`).
 *
 * ---------------------------------------------------------------------------
 * THE PARK'S 14 WAS **TWO** CAUSES, NOT ONE.  ONE IS NOW CLOSED.
 *
 * A per-index differ splits the park's 14 cleanly:
 *
 *   CAUSE A -- POOL ORDER, indices 1, 2, 33, 34.  *** CLOSED. ***
 *     Two of those four "encodings" ARE POOL WORDS: index 33/34 are the
 *     `.word` pair, and the ROM's is {iwram_3001800-reloc, 0x050001d0} against
 *     our {0x050001d0, iwram_3001800-reloc}.  Indices 1 and 2 are the two
 *     `ldr rX,[pc,#64]` that read them -- SAME offset, different destination
 *     register, because the words swapped.  Both compilations put
 *     &iwram_3001800 in r7 and 0x50001d0 in r6, so the REGISTERS were never
 *     wrong; only which pool word each `ldr` reaches.  This is also what made
 *     the park's relocations dirty.
 *
 *   CAUSE B -- ONE sched2 RUN OVER ONE BASIC BLOCK, indices 16-22 and 25-27.
 *     STILL OPEN, and these are TEN of the ten remaining.  Indices 15..29 are a
 *     SINGLE basic block (`bge` at index 12 jumps to index 15; `ble` at 29
 *     closes the loop), so the three-term interleave the park described and the
 *     store/counter swap at 25-27 are NOT separate blockers -- they are one
 *     list-scheduling pass choosing differently twice.  A park reporting one
 *     figure for this pair is why "expression scheduling inside one statement"
 *     read as the whole story.
 *
 * HOW CAUSE A CLOSED: `g = &iwram_3001800;` AS THE FIRST STATEMENT.
 *
 * The ROM loads the global's ADDRESS first and 0x50001d0 second.  In our body
 * that address load was not a source insn at all: `iwram_3001800` is read inside
 * the loop, loop.c hoists its address into the preheader, and loop.c APPENDS to
 * the preheader -- so `.19.flow2` block 0 reads
 *
 *     insn 10  r5 = 0        insn 13  r6 = 0x50001d0        insn 134  r7 = &iwram
 *
 * with the hoisted load LAST and carrying the highest INSN_LUID.  sched2 then
 * sees insn 13 and insn 134 both at `prio 2` (`ldr` costs 2; insn 10's `mov`
 * costs 1), with no `last_scheduled_insn` yet, so `rank_for_schedule`
 * (haifa-sched.c:4029) falls past priority, past the interblock rungs, past
 * CLASS and past the dependent count to its last rung, INSN_LUID -- and the
 * smaller LUID wins.  `-fsched-verbose=5` prints it verbatim:
 *
 *     ;;  Ready list (t = 0):   10  134  13
 *     ;;    --> scheduling insn <<<13>>>
 *
 * Naming the address in a local pointer makes it a REAL source insn placed where
 * the assignment is, so its LUID is the smallest of the three and the same tie
 * resolves the ROM's way.  The pool words follow the instruction order, so
 * indices 33/34 fall out with 1/2.  14 -> 10 and the relocations go clean.
 *
 * THE PARK'S CLAIM 2 IS REFUTED.  It said "`i = 0;` before `p = ...0x50001d0;`
 * is what puts `ldr r7, =0x3001800` ahead of `ldr r6, =0x50001d0`".  It does
 * not: with the park's own statement order the 0x50001d0 load comes FIRST, and
 * swapping `i` and `p` leaves the figure at 14.  `i = 0;` is irrelevant here --
 * it is the LOWEST-priority insn in the block either way.
 *
 * ITS OTHER TWO CLAIMS SURVIVED and are kept:
 *   1. THE SIGNED DIVIDE IS THE IDIOM.  `/ 0x4000` reproduces
 *      `cmp r0,#0 / bge / add r0,#0x3fff / asr r3,r0,#14`; `>> 14` never can.
 *   3. NAMING THE THREE TERMS, with the `<< 10` term written FIRST because gcc's
 *      OR accumulator is the first operand of `a | b | c` and the ROM's
 *      accumulator is r3 = c << 10.
 *
 * ---------------------------------------------------------------------------
 * WHAT CAUSE B IS, WITH THE NUMBERS, AND WHY NO SOURCE FORM REACHES IT
 *
 * Block 3's dependence table (`-fsched-verbose=5`, our build):
 *
 *   insn 52  asr r3,r0,#14   prio 7     insn 65  add r3,#20   prio 5
 *   insn 57  lsl r1,r3,#1    prio 5     insn 70  lsl r3,#10   prio 4
 *   insn 146 mov r2,r3       prio 6     insn 74  lsl r2,#5    prio 4
 *   insn 59  add r1,#22      prio 3     insn 89  strh         prio 1 (cost 2)
 *   insn 62  add r2,#16      prio 5     insn 95  add r5,#1    prio 2
 *
 * B's first half: after 52, the ready list is {95, 57, 146} and gcc takes 146
 * (`mov r2,r3`, prio 6) where the ROM takes 57 (`lsl r1,r3,#1`, prio 5).  The
 * priorities are forced by the expression's own shape: `a` is the LAST operand
 * of `(c<<10) | (b<<5) | a`, so its chain is one `orr` shorter than `b`'s and
 * `c`'s, and the ROM's two `orr` encodings (`4313 orr r3,r2`, `430b orr r3,r1`)
 * PIN which term is the accumulator and which enters which `orr`.  So raising
 * `a`'s chain means changing an encoding that is already right.  57 reaches prio
 * 5 only via its ANTI-dependence on 65 (cost 0 -- `arm_adjust_cost` returns 0 for
 * REG_DEP_ANTI); its data chain is worth 4.  There is no priority-6 spelling of
 * `a`.
 *
 * B's second half: after the last `orr`, ready = {89 (strh, prio 1), 95
 * (add r5,#1, prio 2)} and priority alone decides for 95.  `strh` cannot get
 * above 1 -- its only dependents are the `add r6,#2` (prio 1) and the jump --
 * and `add r5,#1` cannot get below 2, since 2 = 1 + the jump.
 *
 * MEASURED, and the flatness is the finding:
 *
 *   60 legal permutations of the preheader statements, 32 (preheader order x
 *   term spelling) crossed variants, then 24 more aimed at the two sub-causes:
 *   *** EVERY ONE OF THEM READS 10 ***, once `g` is present.  The off-10 rows
 *   are all strictly worse: `(c<<10)` written third 12; `i` unsigned 11; a `for`
 *   loop 20 at 32 instructions (COUNT); `c` assigned first 12; the `cba` term
 *   order 25 at 38 instructions.
 *
 *   crossfire at depth 2 over {volatile store, p[0] subscript, *p++ store,
 *   unsigned i, for loop, volatile deref of g, c-first}: NINE rows EXACTLY
 *   INERT at 10, including `volatile unsigned short *p`.  A volatile store does
 *   NOT move this -- MEM_VOLATILE_P constrains a store against OTHER MEMs, and
 *   this block contains exactly one MEM.
 *
 *   THE INSTRUMENT (not shipped): `-fno-schedule-insns2` takes this body to 4
 *   and the `gpi` preheader order to 4 as well -- it closes indices 25-27
 *   EXACTLY and reduces 16-22 to a single adjacent swap (`mov r2,r3` against
 *   `add r1,#22`, i.e. where reload inserts the base copy).  That is the proof
 *   that cause B is sched2 and nothing else, and the figure 4 is a figure ABOUT
 *   THE BLOCKER, not a result: the ROM's block 0 is NOT chain order, so
 *   -fno-schedule-insns2 regresses cause A to 2, and 4 is not 0.  I am NOT
 *   proposing SCHED2_CFLAGS for this file.  (Under the instrument a further 24
 *   term respellings were ALSO dead flat at 4.)
 *
 * WHAT WOULD CLOSE IT: a reason for the ROM's block 3 to have a shorter
 * `mov r2,r3` chain or a longer `lsl r1,r3,#1` chain than this expression gives
 * -- i.e. evidence that the ROM's packing statement is not this one statement.
 * Everything else in the function (36 of 36 instructions, both pool words, both
 * relocations, the divide idiom, both ORs, the loop and the prologue) is exact.
 
 * ===========================================================================
 * BATCH 330 BRIEF D.  FIGURE RE-DERIVED AT TEN; IT HOLDS.  AND CAUSE B IS NOW
 * BOUNDED FROM BOTH SIDES, WHICH CLOSES TWO ROUTES THIS PARK LEFT OPEN.
 *
 * *** THE ROM'S BLOCK 3 IS NEITHER CHAIN ORDER NOR PRIORITY ORDER. ***  That is
 * the whole finding, and it is decisive because the two regimes available from C
 * are exactly those two.
 *
 * SIDE ONE -- WITH sched2 (production flags), the block comes out in STRICTLY
 * DESCENDING PRIORITY, verified against this park's own recorded priorities:
 *   mov r2,r3 (6), lsl r1,r3,#1 (5), add r2,#0x10 (5), add r3,#0x14 (5),
 *   lsl r3,#0xa (4), lsl r2,#5 (4), add r1,#0x16 (3), orr (3), orr (2),
 *   add r5,#1 (2), strh (1), add r6,#2 (1).
 * The ROM's order is `... orr / orr / strh / add r6,#2 / add r5,#1`, i.e. it puts
 * the increment of the TESTED counter BELOW two priority-one insns.  `add r5,#1`
 * feeds the `cbranchsi4` directly, so its priority cannot be less than two.
 * ** The ROM's block 3 therefore cannot be the output of a priority-ordered list
 * schedule at all, whatever the expression is spelled like. **  This is a
 * stronger statement than the park's "there is no priority-6 spelling of `a`",
 * and it does not depend on the OR structure.
 *
 * SIDE TWO -- WITHOUT sched2 the figure is FOUR, not the two-plus-four this park
 * recorded, and the park's attribution of the residue is wrong.  Under
 * `-fno-schedule-insns2` the WHOLE PREHEADER MATCHES (cause A does not regress;
 * `g = &iwram_3001800;` already fixed it and the flag does not undo that).  The
 * four are two adjacent swaps:
 *   (i)  the loop top, `ldr r3,[r7,#0]` against `lsl r2,r5,#0x3`
 *   (ii) `mov r2,r3` against `add r1,#0x16`
 *
 * AND (ii) IS PROVABLY UNREACHABLE IN THAT REGIME.  `mov r2,r3` is RELOAD'S COPY
 * for the non-matching constraint on `*thumb_addsi3` -- `.19.flow2` of the best
 * body shows it as a freshly numbered insn sitting IMMEDIATELY BEFORE the add it
 * feeds, with the add's dependence list naming it:
 *     (insn 148  (set (reg/v:SI 2 r2) (reg/v:SI 3 r3))     *thumb_movsi_insn
 *     (insn  63  (set (reg/v:SI 2 r2) (plus (reg 2) (const_int 16)))  *thumb_addsi3
 *     (insn  66  (set (reg/v:SI 1 r1) (plus (reg 1) (const_int 22)))
 * Reload emits that copy directly before its insn and nothing between reload and
 * the assembler moves it, so insn 66 can only get BETWEEN 148 and 63 if sched2
 * runs.  The ROM has it between them.
 *
 *   >> SO THE ROM NEEDS sched2 TO SPLIT THE RELOAD PAIR AND NOT TO SORT THE
 *   >> REST, AND NO FLAG OFFERS THAT.  Both standing routes are closed: this
 *   >> file must NOT be proposed for the SCHED2_CFLAGS group (the flag leaves
 *   >> four, and a flag group that does not reach zero buys nothing), and no
 *   >> amount of further term respelling can reach it either. <<
 *
 * MEASURED THIS BATCH, 24 crossed variants (four spellings of the `sin`
 * argument x six term-statement shapes), every row under the flag, ref 36
 * encodings / 32 instructions:
 *   `(*g + i * 8)` or `(*g + (i << 3))`, crossed with
 *     {a-then-b; a=t*2,b=t,a+=,b+=; a=t*2,b=t+0x10,a+=; b=t,a=t*2+0x16,b+=}
 *                                            FOUR, every one of the eight
 *   the same spellings x {c first; c in the middle}      ten and twelve
 *   `(i * 8 + *g)` crossed with all six                  seven, then thirteen/fifteen
 *   `t = *g;` named as its own statement x all six       the load leaves the loop:
 *       one instruction more than the reference, relocations shift -- COUNT, so
 *       none of those six is a distance
 * The term-statement shape MOVES WHICH PAIR is swapped without changing the
 * count: `a = t*2; b = t + 0x10; a += 0x16;` puts `mov r2,r3` in the ROM's place
 * and leaves `add r1,#0x16` against `add r2,#0x10` instead.  Four either way.
 *
 * WHAT IS LEFT, honestly stated: nothing source-reachable that this batch can
 * name.  The function is 36 of 36 instructions, both pool words, both
 * relocations, the divide idiom, both ORs, the loop and the prologue exact, and
 * the residue is one scheduler decision that neither of gcc's two available
 * orders produces.  The next real move is evidence that the ROM's loop body is
 * not this one statement -- e.g. a `b` term whose add fits a three-operand
 * `add rd,rn,#imm3` so that reload needs no copy and there is no pair to split.
*/
extern int iwram_3001800;
extern int sin(int a);

void Func_801fd34(void)
{
    unsigned short *p;
    int *g;
    int i;
    int t, a, b, c;

    g = &iwram_3001800;
    p = (unsigned short *)0x50001d0;
    i = 0;
    do {
        t = sin((*g + i * 8) * 3 << 8) / 0x4000;
        a = t * 2 + 0x16;
        b = t + 0x10;
        c = t + 0x14;
        *p = (c << 10) | (b << 5) | a;
        p++;
        i++;
    } while (i <= 3);
}
