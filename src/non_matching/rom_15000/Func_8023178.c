/* Func_8023178  --  asm/rom_15000/rom_23178_a_a_a_a_a_a.s  @ 0x08023178
 * TRIAGE + A SOLVED DISPATCH SPELLING.  NO FULL CANDIDATE WRITTEN, and so
 * NO objcmp figure.  This header invents none.  1392 instructions, 109
 * labels and 91 calls is not a one-batch reconstruction; what this batch
 * DID close is both of the function's jump-table sites, exactly, measured.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/Func_8023178.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_a_a.s --func Func_8023178
 * (that recipe is for the day a candidate exists; today there is no code
 * below this header, so tools/shimcount.py reports 0 shims.)
 *
 * ===========================================================================
 * CORRECTION 1 -- THE "SHARED FAMILY PROLOGUE" IS GENERIC AND BUYS NOTHING.
 * Both existing family parks (src/non_matching/rom_15000/Func_8023e70.c and
 * .../Func_8024934.c) build their scheduling argument on an "IDENTICAL
 * eight-instruction prologue", concluding that "whichever of the three is
 * solved first supplies the opening forty instructions of the other two".
 * MEASURED, TREE-WIDE: that exact seven-instruction sequence
 *     push {r5,r6,r7,lr} / mov r7,r11 / mov r6,r10 / mov r5,r9 /
 *     push {r5,r6,r7}    / mov r7,r8  / push {r7}
 * opens 338 of the 871 remaining `thumb_func_start` functions in asm/ -- 39%
 * of the tree.  It is what gcc-2.96 emits for ANY Thumb function that uses
 * r8-r11 and makes a call; it is a CONSEQUENCE of register pressure in the
 * body, not a signature of a shared source file, and it is not something a
 * reconstruction "spells" at all.  93 of those 338 sit in GENERATED .s
 * files, i.e. landed C already reproduces it incidentally.
 * Func_80f6440 -- a different bank, a different brief target, 0x28 frame --
 * opens with the same seven instructions.  So the prologue gives the other
 * family members EXACTLY NOTHING, and the "divide the cost by three" claim
 * in both parks should not be relied on for scheduling.
 *
 * CORRECTION 2 -- THE "IDENTICAL OPENING SEQUENCE" IS REAL, BUT ONLY FOR
 * THE THREE FUNCTIONS IN THIS ONE .s.  Func_8023178, Func_8023e70 and
 * Func_8024934 do all spill their arguments and then load iwram_3001e8c and
 * call AllocUploadSpriteGFX.  Func_8026080 loads iwram_3001e74 -- a
 * DIFFERENT global -- and Func_8027114 loads no global at all before its
 * first call.  Batch 313's brief grouped all of 8023178/8026080/8027114 as
 * "one family sharing an identical opening sequence"; measured, that is
 * false for the latter two.
 *
 * CORRECTION 3 -- THE REAL FAMILY EVIDENCE IS SHARED DATA, NOT CODE SHAPE.
 * Func_8023178 references seven undefined `.L` symbols; three of them,
 * .L373dc, .L373e0 and .L373e4, are referenced by EXACTLY ONE other file in
 * the whole tree -- rom_23178_a_a_a_a_c_a_a_a.s, which holds Func_8026080.
 * All seven are defined and `.global`-ed in asm/rom_15000/rom_23178_c_c_c_c.s.
 * Shared read-only tables are the strong family signal here; reach them with
 *     extern unsigned char L373dc[] __asm__(".L373dc");
 * (the brief's undefined-.L lever, and it applies to all four targets).
 * The other four (.L37328, .L373a8, .L373b8, .L373d8) are referenced ONLY
 * from here, so they are this function's private tables.
 * .L23320 and .L23bf0 are NOT globals -- they are its own two jump tables,
 * defined inside the function.
 * ===========================================================================
 *
 * ===========================================================================
 * THE DELIVERABLE: BOTH JUMP TABLES ARE SOLVED, 6 OF 6 DISPATCH
 * INSTRUCTIONS EXACT EACH, AND THE ENTRY VECTORS MATCH ENTRY-FOR-ENTRY.
 * Probes in docs/repro-switch-table-vs-tree/{p1..p6}.c, compiled with the production
 * flags through docs/repro-switch-table-vs-tree/asmof.sh.
 *
 * FIRST, THE BRIEF'S COLUMN HEADER IS MISLEADING AND COST A BAD PREMISE.
 * "jump tables = 33" is an ENTRY count, not a table count.  Measured:
 *   `grep -cE '\t(mov|ldr|add)\tpc'`  ->  Func_8023178 2, Func_8026080 1,
 *                                         Func_8027114 1, Func_80f6440 0.
 * TWO tables here, of 22 and 11 entries = 33.  Func_8026080 has 7 and
 * Func_8027114 17, likewise entries.  So this function is in the dispatch
 * population, but with two sites, not thirty-three.
 *
 * case_values_threshold() == 5, RE-PROBED AND CONFIRMED.  N dense nodes with
 * one arm each: N=3 -> tree (3 cmp), N=4 -> tree (4 cmp), N=5 -> TABLE,
 * N=6 -> TABLE.  The brief's figure holds.
 *
 * *** THE BRIEF'S STATED DISCRIMINATOR IS WRONG, AND THIS IS THE FINDING
 * MOST WORTH CARRYING FORWARD. ***  The brief says: "Cases 1 and 2 need
 * SEPARATE arms even with identical bodies, but `case 8: case 9:` MUST stay
 * stacked; the discriminator is purely numeric."  It is not numeric in the
 * case VALUES at all.  The mechanism is:
 *   - STACKED case labels with one body share ONE rtl label, so
 *     group_case_nodes MERGES them into a single RANGE node.
 *   - The table/tree decision then counts nodes AFTER that merge against
 *     case_values_threshold() == 5.
 * So stacking is what destroys a table.  MEASURED (probe p1): the Table-1
 * value set written with stacked labels --
 *     case 10: case 11: case 12: case 13:  ArmA(); break;
 *     case 14: ... case 21:                ArmB(); break;
 * collapses to 2 range nodes and gcc emits a THREE-COMPARE DECISION TREE
 * (`cmp #10 / bcc`, `cmp #13 / bls`, `cmp #21 / bhi`) and NO table.
 * Written with ONE ARM PER CASE (probe p2) the same values give a table.
 * AND THE DISASSEMBLY CANNOT TELL THE TWO SPELLINGS APART: separate arms
 * with identical bodies produce COINCIDENT labels (`.L8:` `.L9:` on one
 * block), which a disassembler -- knowing only addresses -- prints as the
 * SAME name repeated.  That is why the ROM's table shows entries 10-13 all
 * naming `.L23390`.  The table-vs-tree shape is therefore the ONLY evidence
 * for the spelling, and it is decisive; the repeated label name is not
 * evidence of stacking.
 *
 * *** AND THE BRIEF'S `bcc` TELL IS THE WRONG TELL. ***  The brief says "a
 * `bcc` entry test on an unsigned selector means a fourth lowest case you
 * have not written".  Measured, the signal for a case sharing the default
 * arm at the bottom of the range is THE TABLE'S BASE, i.e. the ABSENCE of a
 * `sub` before the index:
 *   - no `sub` => minval == 0 => a case node with value 0 exists.
 *   - with minval == 0 an unsigned selector needs only an UPPER bound, so
 *     the entry test is a bare `bhi` -- NOT a `bcc`.  A `bcc` is what the
 *     TREE form emits (probe p1), which is the opposite situation.
 * Both of this function's tables are entered by `bhi`, and the missing-case
 * reading still holds for Table 1 via the base-0 evidence.
 *
 * TABLE 1 @ .L23320 -- SOLVED (probe p4).  ROM:
 *     ldr r3, [sp, #0x68] / cmp r3, #0x15 / bhi .L23378
 *     ldr r2, =.L23320 / lsl r3, #2 / ldr r3, [r3, r2] / mov pc, r3
 * 22 entries, 3 distinct targets; entries 0-9 -> .L23378 (the default),
 * 10-13 -> .L23390, 14-21 -> .L2339c.  No `sub`, so minval == 0.
 * The spelling, 6 of 6 dispatch instructions exact after tryc.py's
 * documented destructive-shorthand normalisation, and 22/22 entries with
 * 3 distinct targets:
 *     switch (sel) {
 *     default:
 *     case 0:  <default body> break;      <-- ONE block, both labels
 *     case 10: <A> break;   case 11: <A> break;
 *     case 12: <A> break;   case 13: <A> break;
 *     case 14: <B> break;   case 15: <B> break;
 *     case 16: <B> break;   case 17: <B> break;
 *     case 18: <B> break;   case 19: <B> break;
 *     case 20: <B> break;   case 21: <B> break;
 *     }
 * `default:` and `case 0:` STACKED ON ONE BLOCK is what makes entry 0 and
 * the out-of-range target the same label, which is the ROM's shape; with
 * `case 0:` given its own identical body (probe p3) gcc does NOT cross-jump
 * them and entry 0 gets a separate label, which is one encoding wrong.
 * 13 nodes survive grouping, comfortably over the threshold of 5.
 *
 * TABLE 2 @ .L23bf0 -- SOLVED (probe p6).  ROM:
 *     ldr r3, [sp, #0x20] / sub r3, #8 / cmp r3, #0xa / bhi .L23c4e
 *     ldr r2, =.L23bf0 / lsl r3, #2 / ldr r3, [r3, r2] / mov pc, r3
 * 11 entries, 9 distinct targets, minval 8 (hence the `sub r3, #8`),
 * maxval 18.  Entries for 15 and 16 are default-fill; 13 and 14 SHARE one
 * target (.L23c40).  The spelling -- 6 of 6 dispatch instructions exact,
 * 11/11 entries, 9 distinct targets, which is what p5 (13 and 14 separate,
 * 10 distinct) got WRONG and p6 fixed:
 *     switch (sel) {
 *     case 8:  ... break;   case 9:  ... break;
 *     case 10: ... break;   case 11: ... break;
 *     case 12: ... break;
 *     case 13:                            <-- 13 and 14 STACKED, one body
 *     case 14: ... break;
 *     case 17: ... break;   case 18: ... break;
 *     }                                   <-- NO case 15, NO case 16
 * Eight nodes after grouping, still over the threshold, so stacking 13/14
 * is safe here -- which is exactly why the rule has to be read as
 * "stack only where the ROM shows a shared target AND the post-merge node
 * count stays >= 5", not as anything about the case values.
 * The arms of Table 2 are already legible from the ROM and are a sign
 * selector over two quantities held in r10 and r11 (`mov r4,r10` /
 * `neg r4,r10` / `mov r4,r11` / `neg r4,r11` / `mov r4,r7` / `neg r4,r7`)
 * plus two byte loads at +0x141 and +0x137 off r9.
 * ===========================================================================
 *
 * SPLIT SHAPE -- AND THIS CORRECTS BOTH EXISTING FAMILY PARKS.
 * `tools/split_s.py asm/rom_15000/rom_23178_a_a_a_a_a_a.s Func_8023178
 *  --dry-run` reports a TWO-WAY split, not three:
 *     _b.s  1 function  (1579 lines)   <- the target
 *     _c.s  2 functions (2374 lines)
 *     would REMOVE the original, would rewrite stage1.ld
 * because Func_8023178 is FIRST in the file, so there is no _a part.
 * Func_8023e70.c's park says "A THREE-WAY SPLIT IS NEEDED and it is the same
 * split for all three targets, so do it ONCE."  Both halves are wrong:
 * split_s.py cuts out ONE named target, so the shape depends on WHICH
 * sibling you name (first -> 2-way, middle -> 3-way, last -> 2-way), and
 * there is no single split that serves all three.  Converting all three
 * means splitting progressively, re-running `make compare` green between
 * each.  `tools/datacheck.py` reports NO data exports for this file, and
 * there are no `.lcomm` lines in it (0), so the brief's under-report caveat
 * does not bite here.  18 `.pool` directives.  No per-file Makefile rule
 * mentions the stem; production -O2 flags apply.
 *
 * THE FRAME, ALL FOUR GREPS.
 *   1. `sub sp, #imm`      -> `sub sp, #0x160` / `add sp, #0x160`.  352 B.
 *   2. `(add|sub) sp, rN`  -> ZERO hits.  No register-built frame.
 *   3. `mov rX,sp` TWO hits (`mov r2,sp`, `mov r1,sp`, three apart);
 *      `add rX,sp,#K` -> 8 x #0x60, 1 x #0x64, plus 4 BARE `add rX,sp`.
 *   4. `str rX,[sp]` with no matching load -> sp+0x0 has 22 STORES and
 *      ZERO loads.  Outgoing argument space for five-or-more-argument
 *      calls, staged 22 times.  NOT a local; declaring one is a phantom.
 * So the brief's "aggr 11" is the raw grep-2+3 HIT COUNT (2 + 9), not a
 * count of aggregates.  Resolved, there are TWO bases: sp+0x0 (argument
 * staging) and sp+0x60.  ONE real aggregate.
 * Scalars occupy sp+0x04..sp+0x5c; sp+0x60..sp+0x15f -- 256 bytes, 73% of
 * the frame -- is that ONE aggregate, which grep 1 cannot see.
 *
 * *** THE PHANTOM IS RESOLVED, AND IT IS THE TRAP Func_8024934.c WARNED
 * ABOUT.  sp+0x68 has 0 STORES AND 3 LOADS by the sp-relative census --
 * read before ever written.  It is NOT a scalar and NOT an incoming
 * argument: it is the MEMBER AT +0x08 of the sp+0x60 aggregate, and the
 * census missed its store only because the store is written through a
 * register:  `add r2, sp, #0x60` then `str r3, [r2, #8]`.  It is the
 * SELECTOR OF TABLE 1.  Do not declare an int for it.  That also removes
 * the two "frame holes" at sp+0x60 and sp+0x64.
 * (METHOD NOTE: a word-offset census keyed on `[sp, #imm]` CANNOT see a
 * store made through a materialised base register, so every "loaded but
 * never stored" row it reports must be re-checked against the `add rX,sp`
 * sites before it is called a phantom.  And grep for `#8`, not `#0x8`: this
 * listing writes small immediates in decimal, which made a first check for
 * the store come back empty and nearly reversed this finding.)
 *
 * *** THE sp+0x60 OBJECT'S LAYOUT IS LEGIBLE, AND IT IS A STRUCT, NOT AN
 * ARRAY.  Read directly from the region rather than inferred:
 *      mov  r6, #4
 *      mov  r2, #0
 *      add  r3, sp, #0x64
 *    .L231d6:
 *      sub  r6, #1  /  strb r2, [r3]  /  sub r3, #1
 *      cmp  r6, #0  /  bge .L231d6
 *      mov  r3, #0  /  add r2, sp, #0x60
 *      str  r3, [r2, #8]  /  str r3, [r2, #0xc]  /  str r3, [r2, #0x10]
 * That is a FIVE-BYTE zero fill walking DOWNWARD from sp+0x64 to sp+0x60
 * (r6 = 4,3,2,1,0 with the decrement before the test, so five iterations),
 * immediately followed by three word members being zeroed.  So:
 *      offset +0x00   u8  [5]        <- zeroed backwards, 5 bytes
 *      offset +0x08   word           <- TABLE 1's SELECTOR (sp+0x68)
 *      offset +0x0c   word
 *      offset +0x10   word           <- also the INDEX into the u8 array:
 *                                       `ldr r3,[r2,#0x10]; strb r5,[r2,r3]`
 *      offset +0x14   word
 * The +0x08 start for the first word member is exactly what C alignment
 * gives after `u8 x[5]` (5 rounded up to 4-byte alignment), which
 * independently corroborates the array length as 5 and not 8.
 * `add r3, sp, #0x64` is therefore `&x[4]` -- the LAST element, a walk-down
 * start -- and NOT a separate sub-word scalar.  That answers, for this
 * function, the question the Func_8024934 park raised about its own
 * `add r2, sp, #0x174`: here the shape is unambiguously a walk-down over a
 * byte array, not a one-past-the-end ascending sentinel.
 * (A first-use scan originally classed sp+0x64 as a sub-word scalar; that
 * verdict was UNSOUND because the scan crossed the `.L231d6` loop head.
 * A pool skip or a label is a basic-block boundary to any such scan, and
 * the verdict has to be discarded and the region READ.  The same scan's
 * verdicts for Func_8027114 and Func_80f6440 were re-run block-aware and
 * DO hold -- each resolves to `ldrh rX,[rX]` in the very next instruction.)
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST.  Sorted DESCENDING.
 * 201 sp-relative loads/stores in 1392 instructions, one in seven.
 *      sp+0x68    0 st   3 ld   *** NOT A SLOT: member +8 of sp+0x60 ***
 *      sp+0x5c    1 st   4 ld   SPILL -- arg0 (r0)
 *      sp+0x58    1 st   3 ld   SPILL -- arg1 (r1)
 *      sp+0x54    3 st   8 ld   SPILL -- arg2 (r2)
 *      sp+0x50    1 st   1 ld   SPILL -- the iwram_3001e8c value
 *      sp+0x4c    1 st  43 ld   SPILL -- *** THE HOT QUANTITY ***
 *      sp+0x48    1 st   4 ld
 *      sp+0x44    6 st   5 ld   (holds -1 from the prologue `neg r3,r3`)
 *      sp+0x40    1 st   2 ld      sp+0x3c   2 st  1 ld
 *      sp+0x38    2 st   1 ld      sp+0x34   1 st  2 ld
 *      sp+0x30    2 st   4 ld      sp+0x2c   1 st  5 ld
 *      sp+0x28    7 st   8 ld      sp+0x24   1 st  5 ld
 *      sp+0x20    2 st   2 ld      sp+0x1c   1 st  5 ld
 *      sp+0x18    1 st   1 ld      sp+0x14   1 st  8 ld
 *      sp+0x10    1 st   3 ld      sp+0xc    1 st  5 ld
 *      sp+0x8     1 st   5 ld      sp+0x4    5 st  7 ld
 *      sp+0x0    22 st   0 ld   ARGUMENT STAGING, not a slot
 * 23 real scalar slots.  RANK BY ACCESS COUNT, not slot order: sp+0x4c at
 * 43 loads is read four times more often than anything else in the frame
 * and is almost certainly a base pointer or the main loop cursor, declared
 * EARLY despite sitting high.  Nothing else is close (next: 8, 8, 8, 7).
 *
 * REGISTER PRESSURE.  97 high-register mentions, r9-DOMINANT: r9 49,
 * r8 20, r10 18, r11 10.  One long-lived quantity in r9 carries half the
 * high-register traffic -- the MenuBar shape, not the evenly-spread shape
 * Func_8023e70.c reports for its own target (29/26/24/17).  Table 2's arms
 * read bytes at +0x137 and +0x141 off r9, so r9 is a STRUCT POINTER with a
 * displacement over 0x100; r10, r11 and r7 hold the three signed
 * quantities Table 2 selects between.
 *
 * LOOP-FORM CENSUS, PER FUNCTION, and this is a NEGATIVE BOUND.
 * `grep -coE '\b(bne|blt|ble|bgt|bge)\b'`: bne 22, bge 11, bgt 8, ble 14,
 * blt 4 -- 22 unsigned-shaped against 37 SIGNED compares.  This function is
 * SIGNED-COMPARE-DOMINANT, so the brief's "spell every loop `!=`" lever,
 * which was worth 23 encodings on a all-`bne` function, would CORRUPT 37
 * sites here.  DO NOT APPLY IT.  (Contrast Func_80f6440: 60 bne / 25
 * signed, where it is indicated.)  Only 5 unsigned branches
 * (bcc/bcs/bhi/bls) in 1392, two of which are the two table entry tests --
 * so there are just three other unsigned compares in the whole function and
 * NO decision tree anywhere.
 *
 * OTHER MEASURED LEVERS.
 *   25 x `ldrsb`/`ldrsh` -> the `(signed char)*p` folding lever has 25
 *     sites; `(signed char)p[0]` will not fold and would cost at each.
 *   1 x `bl` to a local `.L` label -> a long BRANCH, not a call (Thumb-1
 *     `b` is +/-2KB and this function spans 0xaf0 bytes of code, so exactly
 *     one branch overruns).  Do not read it as a call.
 *   42 distinct pooled values.  ZERO `__modsi3`/`__divsi3`, so no division
 *     idiom to reconstruct.
 *   91 calls in 1392 instructions, one per 15.3.
 * NOT RUN, SO NOT CLAIMED: no per-flag census (flagcmp.py) and no
 * -fno-* bound, because flags are per-function and there is no candidate to
 * run them against.  `sched1` does not run in this configuration at all, so
 * an inert `-fno-schedule-insns` would have proved nothing.
 *
 * ORACLES.  Nearest LANDED neighbours are
 *   src/rom_15000/rom_23178_a_a_a_a_a_b.c  Func_8025180
 *   src/rom_15000/rom_23178_a_a_a_a_b.c    Func_80251d4
 * both ~5KB away.  House style for rom_15000 (per
 * src/rom_15000/rom_21dfc_a_c_c_b.c): `unsigned int` parameters named
 * arg0/arg1/arg2, locals named for the register they land in, K&R braces,
 * and gotos at dispatch joins are in-style (rom_15000 carries most of the
 * tree's gotos).
 *
 * NEXT, IN ORDER.  (1) Name the seven `.L37xxx` globals and declare them
 * with the __asm__ form -- Func_8026080 shares three of them, so this is
 * shared work.  (2) The sp+0x60 struct's first five members are now laid out above;
 * extend that layout to the rest of its 256 bytes from the remaining
 * [r2,#K] displacements.  That fixes 201 sp-relative instructions at once
 * and is the one defect class no register-level work can compensate for.  (3) Drop in
 * the two solved switches.  (4) Only then start on the body, with the
 * signed-compare bound above respected.
 */
