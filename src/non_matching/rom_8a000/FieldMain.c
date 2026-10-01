/*
 * FieldMain  --  asm/rom_8a000/rom_8ba38_a_c_a_a_a.s  @ 0x0808c4f8
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN.  There is therefore NO objcmp figure
 * for this target and this header does not invent one: the brief's own
 * "TWO well-understood functions beat five shallow ones" was taken at its
 * word and the batch's reconstruction effort went to CalcStats, which
 * reached SIZE-exact and COUNT-exact.  Everything below is MEASURED off the
 * disassembly, not guessed, and is written so the next session can start
 * from a frame map and an oracle rather than from zero.
 *
 * Verify the measurements with:
 *   cd /Users/timothyrosenvall/gs_project/goldensun
 *   python3 tools/showfunc.py FieldMain > /tmp/ref_FieldMain.txt
 *   grep -cE '^\t[a-z]'         /tmp/ref_FieldMain.txt      # 965
 *   grep -nE '\t(sub|add)\tsp,' /tmp/ref_FieldMain.txt      # frame grep 1
 *   grep -nE '\tmov\tr[0-9]+, sp' /tmp/ref_FieldMain.txt    # frame grep 2
 *   grep -nE '\tadd\tr[0-9]+, sp' /tmp/ref_FieldMain.txt    # frame grep 3
 *   python3 tools/shimcount.py src/non_matching/rom_8a000/FieldMain.c
 * shimcount reports 0 shims (this file has no code).
 *
 * THE CENSUS FIX IS REAL, AND SO IS THE WARNING THAT CAME WITH IT.
 * There is no prior work on this function.  Two tree files match the name
 * and NEITHER is about it:
 *   src/non_matching/rom_8a000/808a8e4.c  is the GameStart park; it only
 *     declares `extern void FieldMain(int a);` and calls `FieldMain(k)`.
 *   reports/batch-301.md mentions it in passing.
 * AND THE PARK'S DECLARATION IS WRONG.  FieldMain takes NO arguments: the
 * first thing the body does with r0 is overwrite it with 0x1b for
 * galloc_ewram, and r0 is never read before that.  The reference .s
 * annotation agrees ("Takes no arguments") even though its proposed name
 * (RunInteractionScan) does not match the symbol.  Do not inherit the
 * `int a` from the GameStart park -- writing it that way puts an unused
 * parameter in the frame and the prologue will not match.
 * The same annotation calls this a "~600-instruction body".  It is 965.
 *
 * SPLIT SHAPE: NONE NEEDED.  asm/rom_8a000/rom_8ba38_a_c_a_a_a.s holds
 * exactly one _func_start, so the candidate lands directly as
 * src/rom_8a000/rom_8ba38_a_c_a_a_a.c.  No per-file Makefile rule mentions
 * this stem, so it builds with the production -O2 flags and tryc/objcmp
 * need no flag overrides.  THIS IS THE ONLY ONE OF THE FOUR UNRECONSTRUCTED
 * TARGETS IN THIS BRIEF THAT NEEDS NO SPLIT, which is most of why it is
 * recommended as the next one to attempt.
 *
 * THE FRAME, FROM ALL THREE GREPS -- and the third one earned its keep.
 *   1. `sub sp, #imm`            -> `sub sp, #0x10`, matched by
 *      `add sp, #0x10`.  Sixteen bytes, the smallest frame of the five
 *      targets in this brief by a factor of three.
 *   2. `mov rX, sp`              -> ZERO hits.
 *   3. `add rX, sp`              -> ONE hit, `add r0, sp, #0xc`, four
 *      instructions into the body.  Grep 1 alone would have called this a
 *      four-scalar frame; grep 3 says the top word is an AGGREGATE BASE.
 *      It is: the three instructions after it are
 *          str r7, [r0]                    (r7 = 0, so the word is zeroed)
 *          ldr r3, =REG_DMA3SAD
 *          stmia r3!, {r0, r1, r2}
 *      so sp+0xc is a one-word DMA SOURCE BUFFER whose ADDRESS is handed to
 *      the DMA registers, with r1 = the galloc'd base and r2 = 0x85000333
 *      the control word.  That is a fill-with-zero DMA over the whole block,
 *      and the frame word exists only to hold the zero.  Expect it from
 *      something shaped like `int zero = 0;` plus three stores through a
 *      `volatile unsigned int *` walked over REG_DMA3SAD -- the `stmia r3!`
 *      says the three stores are CONSECUTIVE through one incrementing
 *      pointer, not three separate `REG_DMA3xxx` lvalues.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST, sorted descending, and it is
 * short.  Classifying each sp word by whether it is ever LOADED BACK (a
 * spill) or only ever STORED (argument staging):
 *      sp+0xc   AGGREGATE BASE (see above), not a scalar
 *      sp+0x8   1 store, 1 load   SPILL
 *      sp+0x4   1 store, 1 load   SPILL
 *      sp+0x0   1 store, 1 load   SPILL   <- loaded back, so NOT staging
 * Three scalars, each spilled exactly once and reloaded exactly once, and
 * ONLY SIX sp-relative instructions in 965.  Every one of the three is a
 * value carried across a call: sp+0x8 holds the pointer first seen as
 * `mov r1, r2 / str r1, [sp, #8]` just after Func_8091200 and reloaded as
 * `ldr r3, [sp, #8] / ldr r0, [r3]` before ScreenTransitionIn; sp+0x4 and
 * sp+0x0 each bracket one call in the dispatch cascade.  So the
 * declaration list the next session wants is THREE locals, not a dozen, and
 * the rest of the pressure is in registers.
 *
 * THE REGISTER PRESSURE IS THE WHOLE PROBLEM, and it is the ORDINARY
 * population: 102 high-register mentions (52 x r8, 22 x r10, 16 x r9,
 * 12 x r11) -- all four high registers in use, which is why the prologue is
 * the three-stage
 *      push {r5, r6, r7, lr}
 *      mov r7, r11 / mov r6, r10 / mov r5, r9 / push {r5, r6, r7}
 *      mov r7, r8  / push {r7}
 * form rather than CalcStats' two-stage one.  The four long-lived values
 * are identifiable by inspection and should be FOUR SOURCE LOCALS:
 *      r8  = galloc_ewram(0x1b, 0xccc), the field block.  52 mentions, and
 *            every one is `add rX, r8` or `ldr/str through r8` -- so it is a
 *            base held across all 125 calls, exactly the brief's "A POINTER
 *            READ FROM A GLOBAL AND USED ACROSS A CALL MUST BE A SOURCE
 *            LOCAL" shape, and the proof applies: a call clobbers memory, so
 *            no amount of cse can keep a re-read alive across one.
 *      r9  = gState (reloaded as `ldr r5, =gState` several times AND parked
 *            in r9 at .L8c794 as `mov r9, r4`).  NOTE THE MIX: gState is
 *            re-materialised from the pool in the init block and then held
 *            in r9 for the dispatch cascade.  Reproducing that needs the
 *            brief's "REPRODUCE THE ROM'S NUMBER OF ACCESSES" rule read
 *            strictly -- a single `unsigned char *g = gState;` at the top
 *            will NOT give the four separate `ldr r5, =gState` loads.
 *      r10 = a second pointer into the field block, set once as
 *            `mov r10, r3` where r3 = block + 0xc8 + 0xe0*2, and read back
 *            twice across calls.  This is the two-variables-where-the-ROM-
 *            has-one shape in reverse: a base and a derived pointer.
 *      r11 = the constant 0x10, set as `mov r11, r4` with r4 = 0x10 and
 *            read back as `mov r4, r11` / `mov r1, r11` / `mov r2, r11`
 *            SIX times.  A CONSTANT IN A CALL-SAVED HIGH REGISTER is the
 *            cse1 tell from CalcStats' `mov r12, r2` for 200, one rung
 *            louder: 0x10 is cheap to rematerialise (`mov r3, #0x10`, one
 *            instruction) and gcc still chose to hold it, which means the
 *            source has ONE named quantity with that value used across
 *            calls, not six literals.  Writing six `0x10` literals will not
 *            reproduce it; writing `n = 0x10;` once and using `n` will.
 *            This is the single most load-bearing guess in this triage and
 *            it is the one to test first.
 *
 * DISPATCH CENSUS -- AND THE BRIEF'S SWITCH MATERIAL DOES NOT APPLY HERE.
 * ZERO jump tables: `grep -cE '\t(mov|ldr|add)\tpc' ` is 0, so there is no
 * `casesi` anywhere in this function and no `.word` table to screen.  Only
 * TWO unsigned branches in the whole body (`bcs .L8c8c0` and `bcc .L8c894`),
 * and both are the two ends of ONE ordinary counted loop over the candidate
 * array (`add r6, #1 / add r5, #2 / cmp r6, r3 / bcc`), not a range node.
 * So: no missing case, nothing for screen_missing_case.py to find, and the
 * Case A / Case B apparatus in the brief is inert on this target.  The
 * ~15-way state cascade from .L8c838 to .L8cd76 is a chain of independent
 * `if (field) { ...; } else if (next) ...` tests on DIFFERENT halfwords of
 * the block, which is why it is a chain and not a dispatch.
 * Counted the other way: 125 `bl` in 965 instructions, one call every 7.7
 * instructions.  This is a sequencer, and most of the C is call order.
 *
 * ORACLES -- THE BEST OF THE FIVE TARGETS, and they are adjacent.
 *   src/rom_8a000/rom_8ba38_a_b.c    Func_808c4c0  at -0x38  (LANDED)
 *   src/rom_8a000/rom_8ba38_a_a_b.c  Func_808c44c  at -0xac  (LANDED)
 *   src/rom_8a000/rom_8ba38_a_a_a_c_b.c UpdatePoison at -0x154 (LANDED)
 * The first two are not merely near, they are CALLED BY FieldMain (`bl
 * Func_808c44c` and `bl Func_808c4c0` both appear in the body) and both open
 * with the SAME allocation this function opens with:
 *      base = galloc_ewram(0x1b, 0xccc);
 * so the house spelling for the r8 base is settled before a line is
 * written: a bare `unsigned char *`, with offsets spelled as shifts
 * (`*(short *)(a + (0xcf << 1))`) rather than as decimal.  Func_808c44c's
 * own landing header also records, from this exact bank, the two findings
 * that bear hardest on FieldMain's init block:
 *   - adjacent byte offsets come out as `ldr r1, =0x53a / add r3, r0, r1 /
 *     add r1, #1 / add r3, r0, r1`, i.e. gcc DERIVES the second from the
 *     first, and PLAIN ARRAY SUBSCRIPTS give that.  FieldMain's init block
 *     is forty instructions of exactly this (`add r1, #2`, `add r4, #2`,
 *     `add r1, #0xc`, `add r4, #8`, `add r1, #6`), so DO NOT reach for
 *     pointer variables there -- subscripts off gState are what produce the
 *     chaining, and reaching for pointers is what breaks it.  (In CalcStats
 *     the same cse address-chaining went the OTHER way and cost three
 *     instructions that could not be recovered; here the reference WANTS the
 *     chain, which is the easier side of that lever to be on.)
 *   - a byte that is both read and written in one block is a
 *     `signed char *` local when the read is `ldrsb`.
 *
 * WHERE TO START, concretely.  The init block from the prologue to .L8c794
 * is 180 instructions of straight-line stores with no branches except two
 * two-armed `if`s, it contains the DMA fill and the whole gState offset
 * chain, and it ends at the top of the dispatch cascade.  Write THAT first
 * and screen it with `tools/tryc.py --ref`: if the prologue's four
 * high-register saves and the `add r0, sp, #0xc` DMA triple come out, the
 * four long-lived locals above are right and the remaining 780 instructions
 * are call order.  If they do not, the r11 = 0x10 guess is the first thing
 * to vary.
 *
 * PREDICTED BLOCKER, stated in advance so it can be checked rather than
 * discovered: global.c's priority ordering over FOUR simultaneously live
 * call-saved quantities.  CalcStats, with only two, still could not be made
 * to put the right one of two loop indices in r5 (see
 * src/non_matching/rom_77000/CalcStats.c, blocker section); with four
 * competing values and 125 call sites the same lever has four times the
 * surface.  The counter-evidence that makes it worth trying anyway is the
 * frame: THREE spill slots in 965 instructions means gcc and the reference
 * already agree about which values do not fit in registers, and that
 * agreement is the part that is usually hardest to buy.
 */
