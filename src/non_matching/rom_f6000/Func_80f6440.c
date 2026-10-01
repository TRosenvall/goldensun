/* Func_80f6440  --  asm/rom_f6000/rom_f6008_c_a_e_a_c.s  @ 0x080f6440
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * invents none.  1729 instructions, 141 labels, 59 calls.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f6000/Func_80f6440.c \
 *     asm/rom_f6000/rom_f6008_c_a_e_a_c.s --func Func_80f6440
 * tools/shimcount.py reports 0 shims (this file has no code).
 *
 * ===========================================================================
 * THE HEADLINE, AND IT CORRECTS THE BRIEF: THIS FRAME HAS **ZERO**
 * AGGREGATES, NOT ONE.  The brief's "aggr 1" column is a raw count of
 * grep-2/grep-3 HITS.  There is exactly one hit here and, resolved by its
 * FIRST USE, it is not an aggregate at all:
 *      add  r1, sp, #0x20
 *      ldrh r1, [r1]
 * THUMB-1 HAS NO sp-RELATIVE `ldrh`/`strh`/`ldrb`/`strb` -- only word
 * `ldr`/`str` have an sp+imm form.  So EVERY sub-word access to a stack slot
 * MUST materialise the address with `add rX, sp, #K` first.  `add rX,sp,#K`
 * followed by a sub-word load/store of [rX] with NO displacement is
 * therefore the signature of a `u16`/`u8` LOCAL, not of an aggregate.
 * This is a general correction to the brief's four-grep recipe, which says
 * "`mov rX,sp` and `add rX,sp,#K` -- both are aggregate forms".  They are
 * not: grep 3 OVER-REPORTS aggregates by one per sub-word local, and the
 * discriminator is the FIRST USE of the register --
 *      [rX] with a sub-word op, no displacement  -> SUB-WORD SCALAR
 *      [rX, #K]  or  [rX, rY]                    -> real aggregate
 *      copied/passed to a call                   -> real aggregate or &local
 * Applied to all four of this batch's targets the ranking INVERTS; see the
 * table in PARK_Func_8027114.c.  Func_80f6440 is the CLEANEST frame of the
 * four by a wide margin and is the right place to start, which is what the
 * brief guessed, but for the wrong reason and by a bigger margin than it
 * allowed.
 * ===========================================================================
 *
 *
 * METHOD NOTE ON THE VERDICT ABOVE, because the same scan produced ONE
 * UNSOUND VERDICT elsewhere in this batch and the discipline matters.
 * A "first use of the materialised register" scan must STOP AT BASIC-BLOCK
 * BOUNDARIES.  On Func_8023178 the scan classed sp+0x64 as a sub-word
 * scalar by looking past the `.L231d6` loop head; reading the region showed
 * it was actually `&x[4]`, the walk-down start of a five-byte zero fill at
 * the base of that function's struct.  A label -- and a pool skip -- is a
 * block boundary to any such scan.  THE VERDICTS IN THIS PARK WERE RE-RUN
 * BLOCK-AWARE AND HOLD: every one resolves to `ldrh rX, [rX]` in the VERY
 * NEXT instruction, with no label crossed, which is the strongest form the
 * evidence can take.
 *
 * SPLIT SHAPE: NONE NEEDED, CONFIRMED.
 *   `tools/split_s.py asm/rom_f6000/rom_f6008_c_a_e_a_c.s Func_80f6440
 *    --dry-run` ->  "holds only Func_80f6440 and no data; convert it
 *    directly, no split needed".
 *   `tools/datacheck.py` on the file reports no data exports, and the file
 *   contains ZERO `.lcomm` lines and ZERO `.global`s, so the brief's
 *   datacheck under-report caveat does not bite.  6 `.pool` directives.
 *   No per-file Makefile rule mentions the stem; production -O2 applies.
 *   Replace the .s with src/rom_f6000/rom_f6008_c_a_e_a_c.c when it lands.
 *
 * THE FRAME, ALL FOUR GREPS.
 *   1. `sub sp, #imm`     -> `sub sp, #0x28` / `add sp, #0x28`.  40 bytes.
 *      NOTE the prologue order: the frame is cut on the THIRTEENTH
 *      instruction, AFTER two pooled globals are already loaded
 *      (iwram_3001f04 into r7, and iwram_3001f04-0x18 into r3).  sched2
 *      moved them above the `sub sp`; do not read that as a different
 *      prologue shape.
 *   2. `(add|sub) sp, rN` -> ZERO hits.  No register-built frame, so the
 *      40 bytes is the whole story and nothing is hidden above the 508-byte
 *      Thumb-1 immediate cap.
 *   3. `mov rX,sp` -> ZERO hits.  `add rX,sp,#K` -> ONE hit, sp+0x20,
 *      resolved above as a u16 local.
 *   4. `str rX,[sp]` with no matching load -> sp+0x0 has 9 STORES and ZERO
 *      loads.  Outgoing argument space for the five-argument CreateUIBox
 *      calls (7 of them) and friends.  NOT a local.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST.  Sorted DESCENDING.
 * 102 sp-relative loads/stores in 1729 instructions -- one in 17, the
 * LEAST frame-bound of the four targets by a factor of two.
 *      sp+0x24    1 st  10 ld   SPILL
 *      sp+0x20   11 st  15 ld   SPILL -- *** u16, see above ***
 *      sp+0x1c    3 st  12 ld   SPILL  (holds 0x80<<3 = 0x400 from the
 *                                       prologue `mov r1,#0x80; lsl r1,#3`)
 *      sp+0x18    1 st   9 ld   SPILL
 *      sp+0x14    1 st   2 ld   SPILL
 *      sp+0x10    2 st   4 ld   SPILL
 *      sp+0xc     2 st   8 ld   SPILL
 *      sp+0x8     5 st   5 ld   SPILL
 *      sp+0x4     1 st   1 ld   SPILL
 *      sp+0x0     9 st   0 ld   ARGUMENT STAGING, not a slot
 * NINE scalars, no holes, no "loaded but never stored" phantom -- the only
 * frame of the four that is completely clean on both traps.  Ranked by
 * ACCESS COUNT rather than slot order the hot three are sp+0x20 (26
 * touches), sp+0x1c (15) and sp+0x24 (11); sp+0x4 at 2 touches is the
 * coldest and is almost certainly the LAST declaration.
 *
 * DISPATCH: ZERO JUMP TABLES, confirmed -- `grep -cE '\t(mov|ldr|add)\tpc'`
 * is 0, and only TWO unsigned branches in 1729 instructions, which is far
 * too few for a decision tree.  The brief's whole switch apparatus
 * (case_values_threshold, group_case_nodes, the shared case-0/default arm)
 * is INERT on this target.  Its 141 labels are if/else and loop structure.
 *
 * *** THE LOOP-FORM LEVER IS STRONGLY INDICATED HERE, AND THIS IS THE ONE
 * TARGET OF THE FOUR WHERE IT IS. ***  Per-function census,
 * `grep -coE '\b(bne|blt|ble|bgt|bge)\b'`:
 *      bne 60   bge 8   bgt 8   ble 9   blt 0
 * 60 `bne` against 25 signed compares -- a 2.4:1 ratio, the highest of the
 * four.  `check_dbra_loop` reverses a `for` counter into a down-counter and
 * the reversal is directly observable, so spell the loops `!=` FIRST and
 * check.  Compare the bound the other three impose:
 *      Func_8023178  bne 22 : signed 37   <- lever would CORRUPT 37 sites
 *      Func_8026080  bne 34 : signed 42   <- lever would CORRUPT 42 sites
 *      Func_8027114  bne 43 : signed 14   <- indicated
 *      Func_80f6440  bne 60 : signed 25   <- MOST indicated
 * `blt` is entirely absent here, so no `< 0` test exists and the
 * `<= (0 - 1)` vs `< 0` distinction has no site on this function.
 *
 * UNDEFINED `.L` SYMBOLS -- FOUR OF THEM, AND THEY ARE GLOBAL VARIABLES.
 *   .Lf870c  .Lf8712  .Lf871a  .Lf8728
 * All four are defined AND `.global`-ed in asm/rom_f6000/rom_f6008_c_c_c.s
 * (lines 1007-1017), so they already link; what is missing is only the C
 * declaration.  Reach them with
 *     extern unsigned char Lf870c[] __asm__(".Lf870c");
 * src/non_matching/rom_f6000/Func_80f7f78.c line 83 already lists them as
 * that file's EXPORTS set (with .Lf86f8 and .Lf8736), so the naming work is
 * shared with that park -- do it once.  Two functions elsewhere in the tree
 * were blocked on exactly this rather than on any residue.
 *
 * OTHER MEASURED LEVERS.
 *   4 x `bl __modsi3` -> SIGNED `%`.  Four division idioms to spell; a
 *     `u32` operand would emit __umodsi3 and is therefore WRONG at all 4.
 *   2 x `bl sin`, 2 x `bl Random`, 16 x `bl _PlaySound`,
 *     11 x `bl _CloseUIBox`, 7 x `bl _CreateUIBox`, 10 x `bl _Func_801e7c0`.
 *     A long UI state machine: 59 calls in 1729, one per 29.
 *   6 x `ldrsb`/`ldrsh` -> six sites for the `(signed char)*p` folding
 *     lever; `(signed char)p[0]` will not fold.
 *   2 x `bl` to a local `.L` label -> long BRANCHES, not calls.
 *   41 distinct pooled values, including a run of consecutive sound ids
 *     (0x905, 0x90a-0x90f, 0x912, 0x913) and two message ids (0x131, 0x133).
 *     Those consecutive ids are NOT a reuse opportunity -- each is a single
 *     `ldr rX,=imm` at one site.
 * REGISTER PRESSURE: 122 high-register mentions, the HIGHEST of the four and
 * the most EVENLY spread -- r8 37, r10 37, r11 25, r9 23.  With only 9 spill
 * slots that is the inverse of Func_8023178's profile: here the long-lived
 * quantities stayed in registers rather than going to the stack, so the
 * predicted blocker is global_alloc ORDER, not reload.
 *
 * NOT RUN, SO NOT CLAIMED: no flagcmp.py census and no -fno-* bound, because
 * flags are per-function and there is no candidate to run them against.
 *
 * ORACLES.  Its file-mate Func_80f6148 is parked at
 * src/non_matching/rom_f6000/80f6148.c (20 of 76, a three-register
 * permutation) and its landed neighbour is src/rom_f6000/rom_f6008_c_a_e_b.c,
 * which CALLS Func_80f6440 and so fixes its signature from the call site --
 * read that first.  src/non_matching/rom_f6000/LuckyWheelsMain.c is the
 * other park in the same minigame.  80f6148.c also carries two levers that
 * are live in this bank: the `(u16)` cast before a mask to force a HImode
 * pool entry, and masking into the destination as two statements to defeat
 * loop-invariant motion.
 *
 * PREDICTED BLOCKER: global_alloc order over four comparably-weighted
 * high registers, reached only after 1729 instructions of straight
 * state-machine body are written.  The frame is NOT the obstacle here --
 * nine clean scalars and no aggregates -- which is what makes this the
 * cheapest of the four to START and still the longest to FINISH.
 */
