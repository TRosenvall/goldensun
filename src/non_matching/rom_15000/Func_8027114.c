/* Func_8027114  --  asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s  @ 0x08027114
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * invents none.  1727 instructions, 139 labels, 147 calls.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/Func_8027114.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s --func Func_8027114
 * tools/shimcount.py reports 0 shims (this file has no code).
 *
 * ===========================================================================
 * NOT A FAMILY MEMBER.  The brief groups this with Func_8023178 and
 * Func_8026080 as "all in the rom_23178 family ... the family shares an
 * identical prologue and opening sequence, so solving any one supplies the
 * others' first ~40 instructions".  MEASURED, THAT IS FALSE FOR THIS ONE:
 *   - The seven-instruction prologue is GENERIC: it opens 338 of the 871
 *     remaining functions in asm/ (39% of the tree), including
 *     Func_80f6440 in a different bank.  It is what gcc-2.96 emits for any
 *     Thumb function using r8-r11 plus a call, i.e. a consequence of the
 *     body's register pressure.  It supplies NOTHING.
 *   - The opening sequence does NOT match.  Func_8023178/8023e70/8024934
 *     load iwram_3001e8c then call AllocUploadSpriteGFX; Func_8026080 loads
 *     iwram_3001e74 (a DIFFERENT global); THIS function loads NO global at
 *     all before its first call -- it spills r0/r1/r2, builds 0x100 and
 *     0x400 inline, and goes straight to AllocUploadSpriteGFX.
 *   - It shares NO undefined `.L` global with any of them.  Its only
 *     undefined-looking `.L` symbol, .L275b8, is its OWN jump table,
 *     defined in-function at line 579 of its .s.  (Func_8023178 and
 *     Func_8026080 DO share .L373dc/.L373e0/.L373e4 with each other; that
 *     is the real family evidence, and this function is outside it.)
 * Treat it as an independent target.  Its true file-mate is Func_8028194,
 * parked at src/non_matching/rom_15000/8028194.c -- read that, not the
 * rom_23178_a_a_a_a_a_a.s parks.
 * ===========================================================================
 *
 * ===========================================================================
 * THE FRAME: **ZERO AGGREGATES**, NOT NINE.  BIGGEST TRIAGE CORRECTION OF
 * THE FOUR.  The brief's "aggr 9" is a raw count of grep-2/grep-3 hits.
 * Resolved by the FIRST USE of each materialised register -- which is the
 * discriminator the brief's recipe is missing -- all nine are something
 * else.  THUMB-1 HAS NO sp-RELATIVE `ldrh`/`strh`/`ldrb`/`strb`; only word
 * `ldr`/`str` have an sp+imm form, so every sub-word access to a stack slot
 * MUST materialise its address with `add rX,sp,#K` first.  That makes
 * `add rX,sp,#K` + sub-word op on [rX] with NO displacement the signature
 * of a `u16`/`u8` LOCAL, not an aggregate.  All nine hits:
 *      sp+0x44  r1  -> SUB-WORD SCALAR      sp+0x40  r1  -> SUB-WORD SCALAR
 *      sp+0x40  r1  -> SUB-WORD SCALAR      sp+0x3c  r3  -> SUB-WORD SCALAR
 *      sp+0x38  r0  -> SUB-WORD SCALAR
 *      sp+0x64  r1  -> one-past-END-OF-FRAME (0x64 IS the frame size)
 *      sp+0x64  r2  -> one-past-END-OF-FRAME
 *      sp+0x0   r2  -> pointer deref at base+0 (argument staging)
 *      sp+0x0   r3  -> argument staging
 * So the real reading is a FLAT 100-BYTE SCALAR FRAME with FIVE sub-word
 * locals clustered at sp+0x38..sp+0x44, and NO aggregate anywhere.
 *
 * RESOLVED BY FIRST USE, THE BRIEF'S DIFFICULTY RANKING INVERTS:
 *      function        brief "aggr"   TRUE aggregates
 *      Func_8023178         11          1  (one 256-byte object at sp+0x60)
 *      Func_8026080         12          4  (sp+0x6c, 0x78, 0xac, 0xc8)
 *      Func_8027114          9          0  (5 sub-word scalars)
 *      Func_80f6440          1          0  (1 sub-word scalar)
 * Func_8026080 is the aggregate-heavy target, not this one, and this one
 * is nearly as clean as the brief's designated "easiest frame".
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
 * `add rX, sp, #0x64` TWICE is worth singling out: 0x64 is the WHOLE frame
 * size, so the register points one past the end of the frame.  Per the
 * Func_8024934 park's reading of the same shape, that is either a walk-down
 * bound or a one-past-the-end ascending sentinel, and WHICH ONE fixes the
 * loop direction.  Settle it before writing the declaration.
 * ===========================================================================
 *
 * THE FRAME, ALL FOUR GREPS.
 *   1. `sub sp, #imm`     -> `sub sp, #0x64` / `add sp, #0x64`.  100 bytes,
 *      the SMALLEST frame of the three rom_15000 targets.
 *   2. `(add|sub) sp, rN` -> ZERO hits.  Nothing hidden above the 508-byte
 *      Thumb-1 immediate cap.
 *   3. `mov rX,sp` TWO hits; `add rX,sp,#K` SEVEN hits.  Resolved above.
 *   4. `str rX,[sp]` with no matching load -> sp+0x0 has 15 STORES and ZERO
 *      loads, AND sp+0x44 has 3 STORES AND ZERO LOADS.  TWO store-only
 *      words, not one.  sp+0x0 is outgoing argument space (147 calls, many
 *      with five-plus arguments).  sp+0x44 is different: its ADDRESS is
 *      taken (`add r1,sp,#0x44`), so it is a local we initialise and hand
 *      to a callee by reference and never read back ourselves -- an OUTPUT
 *      parameter.  Declaring nothing for sp+0x44 would be wrong; declaring
 *      a plain scalar and reading it back would also be wrong.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST.  Sorted DESCENDING.
 * 216 sp-relative loads/stores in 1727 instructions -- one in eight.
 *      sp+0x58    1 st   2 ld   SPILL
 *      sp+0x54    1 st   3 ld   SPILL -- arg1 (r1)
 *      sp+0x50    8 st   5 ld   SPILL -- arg2 (r2)
 *      sp+0x4c    2 st  18 ld   SPILL  *** 2nd hottest ***
 *      sp+0x48    1 st  12 ld   SPILL
 *      sp+0x44    3 st   0 ld   &local OUTPUT PARAM (see grep 4)
 *      sp+0x40    1 st  14 ld   sub-word local, address taken twice
 *      sp+0x3c    7 st   1 ld   sub-word local
 *      sp+0x38    5 st   3 ld   sub-word local
 *      sp+0x34    1 st   3 ld      sp+0x30   1 st   2 ld
 *      sp+0x2c    3 st   5 ld      sp+0x28   1 st   2 ld
 *      sp+0x24    1 st  43 ld   SPILL  *** THE HOT QUANTITY ***
 *      sp+0x20    3 st   2 ld      sp+0x1c   3 st   6 ld
 *      sp+0x18    3 st   1 ld      sp+0x14   1 st   7 ld
 *      sp+0x10    1 st   1 ld      sp+0xc    1 st   1 ld
 *      sp+0x8     1 st   1 ld      sp+0x4    9 st  11 ld
 *      sp+0x0    15 st   0 ld   ARGUMENT STAGING, not a slot
 * arg0 (r0) spills to sp+0x58.  No holes.  No "loaded but never stored"
 * phantom.  RANK BY ACCESS COUNT: sp+0x24 at 43 loads is read more than
 * twice as often as anything else and is the main base pointer or loop
 * cursor -- declared EARLY despite sitting in the middle of the frame.
 * Note the coincidence worth not over-reading: Func_8023178's hottest slot
 * is ALSO exactly 43 loads (at sp+0x4c).  Two functions, same figure,
 * different offsets -- that is a coincidence between separate quantities,
 * not evidence of shared structure, and it is the slot-level analogue of
 * the rung-8 "exact count can be a coincidence between opcodes" trap.
 *
 * DISPATCH: ONE TABLE, 17 ENTRIES -- and the brief's "17 jump tables" is an
 * ENTRY count.  `grep -cE '\t(mov|ldr|add)\tpc'` is 1.
 *      ldr r2, =.L275b8 / lsl r3, r6, #2 / ldr r3, [r3, r2] / b .L275b4
 *      .align 2,0 / .L275a8: .word 0x60 / .pool
 *      .L275b4: mov pc, r3
 * NO `sub` before the index, so minval == 0; selector is in r6, cases 0..16
 * (17 entries).  Entries 0,1,2,3 -> distinct arms; entries 4..14 (ELEVEN of
 * them) -> .L27f82, the default; entry 15 -> .L27b5c; entry 16 -> .L278e4.
 * So SIX surviving case nodes, span 17 <= 10*6, which is why the formula
 * says TABLE, and 6 >= case_values_threshold() == 5 (re-probed and
 * confirmed this batch: 3 and 4 dense nodes give a tree, 5 and 6 give a
 * table).  Write it as SIX SEPARATE ARMS -- `case 0:` through `case 3:`,
 * `case 15:`, `case 16:`, with NO case 4..14 -- plus a default.  Do NOT
 * stack any of them: stacking lets group_case_nodes merge nodes into ranges
 * and the post-merge count then falls under the threshold, which produces a
 * decision tree and no table at all (probed on Func_8023178's value set;
 * see PARK_Func_8023178.c, where both of that function's tables are solved
 * at 6 of 6 dispatch instructions each).
 *
 * *** AND THERE IS A POOL SKIP INSIDE THE DISPATCH ITSELF. ***  gcc emitted
 * `b .L275b4` over a literal pool (`.word 0x60` + `.pool`) and put the
 * `mov pc, r3` on the far side.  That is the pool-tell class landing in the
 * middle of a jump table: the `b` is NOT control flow from the source, it
 * is the compiler stepping over data it had to place there.  Do not try to
 * reproduce it with a source-level branch.  26 `.pool` directives in the
 * file overall.  The pooled `0x60` is the constant the following block
 * needs, forced into the gap by pool_range.
 *
 * LOOP-FORM CENSUS, PER FUNCTION -- THE `!=` LEVER IS INDICATED HERE.
 *      bne 43   bge 6   ble 6   bgt 1   blt 1
 * 43 `bne` against 14 signed compares, a 3:1 ratio.  Second most favourable
 * of the four after Func_80f6440 (60:25), and the opposite of Func_8023178
 * (22:37) and Func_8026080 (34:42), where the same edit would corrupt 37
 * and 42 sites respectively.  Only ONE `blt`, so there is exactly one
 * candidate site for the `<= (0 - 1)` vs `< 0` distinction.
 *
 * OTHER MEASURED LEVERS.
 *   *** 10 x `bl` to a local `.L` label -- BY FAR THE MOST OF THE FOUR
 *     (the others have 1, 1 and 2).  These are long BRANCHES, not calls:
 *     Thumb-1 `b` reaches +/-2KB and this function's code spans ~0xd80
 *     bytes, so ten branches overrun and the assembler promotes them.
 *     Reading any of the ten as a call would invent ten phantom callees.
 *     With 147 real `bl` calls in the listing, 10 of the 157 `bl` lines are
 *     not calls -- a 6% over-count on any call census taken by grepping
 *     `bl` alone.
 *   5 unsigned branches (bcc/bcs/bhi/bls) in 1727, one of which is the
 *     table entry test -> no decision tree anywhere else.
 *   Only 2 x `ldrsb`/`ldrsh` -> the `(signed char)*p` folding lever has
 *     almost no purchase here (vs 25 sites on Func_8023178).
 *   ZERO `__modsi3`/`__divsi3` -> no division idiom.
 *   32 distinct pooled values, the FEWEST of the four.
 *   147 calls in 1727 instructions, one per 11.7 -- the call-densest of the
 *     four by a wide margin.  That, not the frame, is the character of this
 *     function: it is a long driver that mostly calls out.
 * REGISTER PRESSURE: 108 high-register mentions, the most EVENLY spread of
 * the rom_15000 three -- r11 33, r8 29, r10 25, r9 21.  No dominant base
 * pointer (contrast Func_8023178, where r9 alone takes 49 of 97), so four
 * comparably-used long-lived quantities plus 22 spill slots: reload ran out
 * of registers rather than one pointer being held across everything.
 *
 * NOT RUN, SO NOT CLAIMED: no flagcmp.py census, no -fno-* bound, no
 * aligncmp figure.  Flags are per-function and there is no candidate to run
 * them against.  `sched1` does not run in this configuration, so an inert
 * `-fno-schedule-insns` would prove nothing; sched2 does run, and the
 * prologue of Func_80f6440 shows it moving loads above `sub sp`.
 *
 * SPLIT SHAPE: TWO-WAY, confirmed by dry-run.
 *   `tools/split_s.py asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s Func_8027114
 *    --dry-run`:
 *      would write ..._c_c_b.s  1 function  (2010 lines)  <- the target
 *      would write ..._c_c_c.s  1 function  ( 419 lines)  <- Func_8028194
 *      would REMOVE the original, would rewrite stage1.ld
 *   Two-way because the target is FIRST in the file, so there is no _a part.
 *   `tools/datacheck.py` reports no data exports; the file has ZERO `.lcomm`
 *   and ZERO `.global` lines, so the brief's datacheck under-report caveat
 *   does not bite.  Install as src/rom_15000/rom_23178_a_a_a_a_c_a_c_c_b.c.
 *   Verify `make compare` green AFTER the split and BEFORE writing any .c --
 *   a layout mistake and a bad decompilation look identical at the end.
 *
 * ORACLES.  Its file-mate Func_8028194 is parked at
 * src/non_matching/rom_15000/8028194.c and its landed near-neighbour is
 * src/rom_15000/rom_23178_a_a_a_a_c_a_c_b.c, which is in the same subtree
 * and only a few hundred bytes away -- much better evidence than anything
 * available to the rom_23178_a_a_a_a_a_a.s parks, which have nothing nearer
 * than 5KB.  src/non_matching/rom_b5000/80b920c.c also mentions this
 * function.  House style for rom_15000 (src/rom_15000/rom_21dfc_a_c_c_b.c):
 * `unsigned int` parameters named arg0/arg1/arg2, locals named for the
 * register they land in, K&R braces, gotos at dispatch joins in-style.
 *
 * PREDICTED BLOCKER: reload over 22 spill slots, reached after 1727
 * instructions of call-dense driver.  The frame itself is NOT the obstacle
 * (zero aggregates), and the dispatch is solved in shape above, so the
 * honest estimate is that the work here is volume, not difficulty -- which
 * makes it a poor choice against Func_80f6440 (9 slots, 59 calls) for a
 * first full attempt despite the better oracle neighbourhood.
 */
