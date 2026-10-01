/* Func_8026080  --  asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s  @ 0x08026080
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * invents none.  1585 instructions, 159 labels (the most of the four
 * targets), 69 calls.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/Func_8026080.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s --func Func_8026080
 * tools/shimcount.py reports 0 shims (this file has no code).
 *
 * ===========================================================================
 * THIS IS THE HARD ONE OF THE FOUR, AND THE BRIEF RANKED IT MIDDLE.
 * It is the ONLY one of the four with real aggregates, and it has FOUR.
 * Resolved by the FIRST USE of each `add rX,sp,#K` register -- the
 * discriminator the brief's four-grep recipe is missing -- the true counts
 * across the batch are:
 *      function        brief "aggr"   TRUE aggregates
 *      Func_8023178         11          1   (one 256-byte object, sp+0x60)
 *      Func_8026080         12        **4** (sp+0x6c, 0x78, 0xac, 0xc8)
 *      Func_8027114          9          0   (5 sub-word scalars)
 *      Func_80f6440          1          0   (1 sub-word scalar)
 * The brief's column is a raw grep-hit count.  THUMB-1 HAS NO sp-RELATIVE
 * `ldrh`/`strh`/`ldrb`/`strb`, so every sub-word stack access must
 * materialise its address with `add rX,sp,#K` first -- which means grep 3
 * over-reports aggregates by one per sub-word local.  The test is the first
 * use: a sub-word op on [rX] with NO displacement is a `u16`/`u8` scalar; a
 * displaced or register-indexed access, or being passed onward, is a real
 * aggregate.  All four bases here pass that test, EVIDENCED:
 *      add r5, sp, #0xc8 ; mov r0,r10 ; mov r1,r5 ; bl _Func_80b84c0
 *      add r4, sp, #0xac ; ... ; mov r10, r4
 *      add r5, sp, #0x6c ; mov r8,r0 ; mov r1,r5 ; (ldrh) 
 *      add r6, sp, #0x78 ; mov r1,r6 ; mov r2,#0xe ; bl Func_801965c
 * Each is handed to a callee as a POINTER (r1), so each is a genuine
 * stack object, and the sp+0x78 one is passed with a length of 0xe.
 *
 * AGGREGATE ORDER IS REVERSED relative to the scalars, so the DECLARATION
 * SEQUENCE is sp+0xc8 first, then sp+0xac, then sp+0x78, then sp+0x6c.
 * Getting that order wrong moves every displacement in 175 sp-relative
 * instructions at once, and it is the one defect class that no amount of
 * register-level work can compensate for.  Settle it before any declaration.
 *
 * `add r0, sp, #0x144` -- 0x144 IS THE WHOLE FRAME SIZE, so that register
 * points ONE PAST THE END of the frame, and its use is the tell:
 *      add r0, sp, #0x144 ; ldr r1, [sp, #0x40] ; add r3, r6, r0
 * It is not a pointer that gets dereferenced -- it is the BASE of an index
 * computation `r6 + (sp+0x144)`, i.e. a WALK-DOWN with a negative index
 * into the region ending at the top of the frame.  Per the Func_8024934
 * park, which found the same shape at its own 0x174: which of walk-down or
 * ascending-sentinel it is decides the loop DIRECTION for the largest
 * object in the frame.  Here the `add r3, r6, r0` form settles it as the
 * walk-down, so the array is indexed from the TOP downwards.
 * ===========================================================================
 *
 * FAMILY: THE REAL EVIDENCE IS SHARED DATA, NOT THE PROLOGUE.
 *   - The brief says this shares "an identical prologue and opening
 *     sequence" with Func_8023178 and Func_8027114, so that solving one
 *     supplies the others' first ~40 instructions.  MEASURED, the
 *     seven-instruction prologue opens 338 of the 871 remaining functions
 *     in asm/ -- 39% of the tree, including Func_80f6440 in another bank.
 *     It is gcc-2.96's output for ANY Thumb function using r8-r11 plus a
 *     call: a consequence of the body's register pressure, not a signature,
 *     and not something a reconstruction spells.  It supplies NOTHING.
 *   - The OPENING SEQUENCE DOES NOT MATCH EITHER.  Func_8023178/8023e70/
 *     8024934 load **iwram_3001e8c**; this function loads **iwram_3001e74**,
 *     a DIFFERENT global.  Both existing family parks
 *     (src/non_matching/rom_15000/Func_8023e70.c and .../Func_8024934.c)
 *     describe the iwram_3001e8c opening as the family's shared shape; this
 *     function is NOT part of that, and the brief was wrong to group it in.
 *   - WHAT *IS* REAL: this function and Func_8023178 share three undefined
 *     `.L` globals -- **.L373dc, .L373e0, .L373e4** -- and those two files
 *     are the ONLY two in the whole tree that reference them.  All three are
 *     defined and `.global`-ed in asm/rom_15000/rom_23178_c_c_c_c.s.  Shared
 *     read-only tables are the strong family signal here.  Declare them with
 *         extern unsigned char L373dc[] __asm__(".L373dc");
 *     and do that work ONCE for both functions -- two functions elsewhere in
 *     the tree were blocked on naming such symbols rather than on any
 *     residue.  .L2667c is NOT a global: it is this function's own jump
 *     table, defined in-function at line 2569 of its .s.
 *
 * SIGNATURE: FOUR PARAMETERS, and two of them never touch the stack.
 * The prologue does `str r2,[sp,#0x54]`, `str r3,[sp,#0x50]`, `mov r10,r0`,
 * `mov r8,r1`.  So arg0 and arg1 go straight to HIGH REGISTERS and arg2/arg3
 * spill.  A three-parameter signature would put the wrong register in the
 * wrong place by instruction 10.  Write four.
 *
 * THE FRAME, ALL FOUR GREPS.
 *   1. `sub sp, #imm`     -> `sub sp, #0x144` / `add sp, #0x144`.  324 B.
 *   2. `(add|sub) sp, rN` -> ZERO hits.  Nothing hidden above the 508-byte
 *      Thumb-1 immediate cap, so 324 bytes is the whole frame.
 *   3. `mov rX,sp` FIVE hits; `add rX,sp,#K` SEVEN displaced + EIGHT bare.
 *      Resolved above: four real aggregates, one one-past-end bound, one
 *      sub-word scalar, the rest argument staging at sp+0x0.
 *   4. `str rX,[sp]` with no matching load -> sp+0x0 has 11 STORES and ZERO
 *      loads.  Outgoing argument space for five-or-more-argument calls.
 *      NOT a local; a declaration there is a phantom.
 * Scalars occupy sp+0x04..sp+0x58; sp+0x5c..sp+0x143 -- 232 bytes, 72% of
 * the frame -- is the four-aggregate region that grep 1 cannot see at all.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST.  Sorted DESCENDING.
 * 175 sp-relative loads/stores in 1585 instructions -- one in nine.
 *      sp+0x58    1 st   2 ld   SPILL
 *      sp+0x54    2 st   9 ld   SPILL -- arg2 (r2)
 *      sp+0x50    1 st   5 ld   SPILL -- arg3 (r3), and the TABLE SELECTOR
 *      sp+0x4c    1 st   5 ld   SPILL -- the iwram_3001e74 value
 *      sp+0x48    1 st   2 ld   SPILL
 *      sp+0x44   11 st   9 ld   SPILL  (most-WRITTEN word in the frame)
 *      sp+0x40    7 st  18 ld   SPILL  *** THE HOT QUANTITY ***
 *      sp+0x3c    1 st   3 ld      sp+0x38   5 st   5 ld
 *      sp+0x34    1 st   2 ld      sp+0x30   2 st   2 ld
 *      sp+0x2c    1 st   1 ld      sp+0x28   1 st   1 ld
 *      sp+0x24    1 st   9 ld      sp+0x20   1 st   6 ld
 *      sp+0x1c    5 st  16 ld   SPILL  *** 2nd hottest ***
 *      sp+0x18    2 st   6 ld      sp+0x14   1 st   2 ld
 *      sp+0x10    1 st   1 ld      sp+0xc    2 st   2 ld
 *      sp+0x8     3 st   3 ld      sp+0x4    1 st   3 ld
 *      sp+0x0    11 st   0 ld   ARGUMENT STAGING, not a slot
 * 22 real scalar slots.  NO HOLES and NO "loaded but never stored"
 * phantom -- unlike Func_8023178 (sp+0x68) and Func_8024934 (sp+0x58),
 * this frame is clean on both traps, so every word listed is a real
 * quantity.  RANK BY ACCESS COUNT, not slot order: sp+0x40 (25 touches) and
 * sp+0x1c (21) are the two hottest and are almost certainly the main loop's
 * cursor and selection, declared EARLY despite sp+0x40 sitting high.
 * sp+0x44 with ELEVEN STORES against 9 loads is unusual -- a quantity
 * rewritten more often than read, i.e. an accumulator or a running index,
 * not a value computed once.
 *
 * DISPATCH: ONE TABLE, 7 ENTRIES -- the brief's "7 jump tables" is an ENTRY
 * count.  `grep -cE '\t(mov|ldr|add)\tpc'` is 1.
 *      ldr r3, [sp, #0x50] / sub r3, #1 / cmp r3, #6 / bls .L26672
 *      b .L26b8c
 *   .L26672:
 *      ldr r2, =.L2667c / lsl r3, #2 / ldr r3, [r3, r2] / mov pc, r3
 * `sub r3, #1`, so minval == 1; 7 entries for cases 1..7, ALL SEVEN
 * DISTINCT targets, no default-fill.  7 >= case_values_threshold() == 5
 * (re-probed and confirmed this batch: 3 and 4 dense nodes give a tree, 5
 * and 6 give a table), and span 7 <= 10*7, so the formula says TABLE.
 * Write SEVEN SEPARATE ARMS, `case 1:` through `case 7:`, plus a default.
 * Do NOT stack any of them -- stacking lets group_case_nodes merge nodes
 * into ranges and the post-merge count then falls below the threshold,
 * giving a decision tree and no table (probed; see PARK_Func_8023178.c,
 * where both of that function's tables are solved at 6 of 6 dispatch
 * instructions each).
 * There is no `case 0` and the entry test is NOT a `bcc`, so the brief's
 * "a `bcc` entry test means a fourth lowest case you have not written"
 * lever has NO site here.  The real tell for a shared case-0/default arm is
 * the ABSENCE of the `sub` (minval == 0); this table HAS a `sub #1`, so
 * case 1 is genuinely the lowest case.
 *
 * *** THE INVERTED ENTRY TEST IS A BRANCH-RANGE ARTIFACT, NOT A SOURCE
 * DIFFERENCE. ***  Every other dispatch in this batch tests `bhi <default>`
 * directly; this one tests `bls <table>` and falls through to an
 * unconditional `b <default>`.  MEASURED: .L26668 -> .L26b8c spans 539
 * instructions, about 1078 bytes of Thumb code.  A Thumb-1 CONDITIONAL
 * branch reaches only +/-256 bytes, so `bhi .L26b8c` is out of range and
 * the assembler/compiler must invert the condition and route through an
 * unconditional `b` (+/-2KB).  Do NOT try to reproduce this with a
 * source-level inversion -- it falls out of the distance, and it will
 * appear or vanish on its own as the body's length changes.
 *
 * LOOP-FORM CENSUS, PER FUNCTION -- *** THE `!=` LEVER IS CONTRA-INDICATED
 * HERE, THE WORST OF THE FOUR. ***
 *      bne 34   bge 21   ble 9   bgt 8   blt 4
 * 34 `bne` against **42 SIGNED** compares -- the only target of the four
 * where signed compares OUTNUMBER `bne`.  Spelling every loop `!=` was
 * worth 23 encodings on an all-`bne` function; here it would corrupt 42
 * sites.  DO NOT APPLY IT.  The spread across the four:
 *      Func_80f6440  bne 60 : signed 25   <- most indicated
 *      Func_8027114  bne 43 : signed 14   <- indicated
 *      Func_8023178  bne 22 : signed 37   <- contra-indicated
 *      Func_8026080  bne 34 : signed 42   <- MOST contra-indicated
 * 21 `bge` is also the highest count of the four by a factor of two, so the
 * dominant loop idiom here is an ascending `for (i = 0; i <= n; i++)` or a
 * descending `while (i >= 0)`, NOT a `!=` loop.  With 4 `blt` there are
 * four candidate sites for the `<= (0 - 1)` vs `< 0` distinction, which are
 * DIFFERENT INSTRUCTIONS -- check each.
 *
 * OTHER MEASURED LEVERS.
 *   17 x `ldrsb`/`ldrsh` -> 17 sites for the `(signed char)*p` folding
 *     lever.  `(signed char)p[0]` is identical C and does NOT fold, so the
 *     wrong spelling costs at every one of the 17.
 *   3 x `bl __modsi3` -> SIGNED `%`, three sites.  A `u32` operand would
 *     emit __umodsi3 and is therefore wrong at all three.
 *   1 x `bl` to a local `.L` label -> a long BRANCH, not a call.  With 68
 *     real calls in the listing, reading it as a call invents one phantom
 *     callee.
 *   7 unsigned branches (bcc/bcs/bhi/bls) in 1585, two of which are the
 *     table entry test pair -> five others, too few for any decision tree.
 *   35 distinct pooled values.  `ldr r2, =0xffff` in the prologue, spilled
 *     immediately to sp+0x38 -- a mask held in a frame slot across the whole
 *     function, which is a LEVER-1 (reuse) candidate: merging it with
 *     another range could buy back the slot.
 *   69 calls in 1585 instructions, one per 23.
 * REGISTER PRESSURE: 94 high-register mentions, r9-leaning -- r9 34, r8 22,
 * r11 20, r10 18.  r10 and r8 are written in the prologue from arg0 and
 * arg1, so two of the four high registers are PARAMETERS held in registers
 * for the function's whole length; that is why 22 spill slots coexist with
 * a middling high-register count.
 *
 * NOT RUN, SO NOT CLAIMED: no flagcmp.py census, no -fno-* bound, no
 * aligncmp figure, no per-flag table.  Flags are per-function and there is
 * no candidate to run them against.  `sched1` does not run in this
 * configuration, so an inert `-fno-schedule-insns` would prove nothing;
 * sched2 does run.
 *
 * SPLIT SHAPE: THREE-WAY, confirmed by dry-run -- the only genuinely
 * three-way split of this batch's four.
 *   `tools/split_s.py asm/rom_15000/rom_23178_a_a_a_a_c_a_a_a.s Func_8026080
 *    --dry-run`:
 *      would write ..._c_a_a_a_a.s  2 functions (1765 lines)
 *                                   <- Func_8025200, Func_802592c
 *      would write ..._c_a_a_a_b.s  1 function  (1797 lines)  <- the target
 *      would write ..._c_a_a_a_c.s  1 function  ( 152 lines)  <- Func_8026e80
 *      would REMOVE the original, would rewrite stage1.ld
 *   `tools/datacheck.py` reports no data exports; the file has ZERO `.lcomm`
 *   and ZERO `.global` lines, so the brief's datacheck under-report caveat
 *   does not bite here.  24 `.pool` directives.  No per-file Makefile rule
 *   mentions the stem; production -O2 flags apply.  Install as
 *   src/rom_15000/rom_23178_a_a_a_a_c_a_a_a_b.c.
 *   Verify `make compare` is still GREEN after the split and BEFORE writing
 *   any .c -- a layout mistake and a bad decompilation look identical at the
 *   end.  Note split_s.py DELETES the tracked .s, and this one holds FOUR
 *   unconverted functions, so --dry-run first, always.
 *
 * ORACLES.  Its file-mate Func_802592c is parked at
 * src/non_matching/rom_15000/802592c.c -- read that first; it is in the same
 * original source file and is the single best evidence available.  The
 * nearest landed functions are src/rom_15000/rom_23178_a_a_a_a_a_b.c
 * (Func_8025180) and src/rom_15000/rom_23178_a_a_a_a_b.c (Func_80251d4),
 * both within ~1KB, which is a far better neighbourhood than the
 * rom_23178_a_a_a_a_a_a.s parks have (nothing nearer than 5KB).
 * House style for rom_15000 (src/rom_15000/rom_21dfc_a_c_c_b.c):
 * `unsigned int` parameters named arg0/arg1/arg2, locals named for the
 * register they land in, K&R braces, gotos at dispatch joins in-style.
 *
 * NEXT, IN ORDER.  (1) Lay out the FOUR aggregates from their callee
 * signatures -- _Func_80b84c0 takes the sp+0xc8 one, Func_801965c takes the
 * sp+0x78 one with length 0xe -- and get the REVERSED declaration order
 * right (0xc8, 0xac, 0x78, 0x6c); that fixes 175 sp-relative instructions
 * at once.  (2) Name .L373dc/.L373e0/.L373e4 with the __asm__ form, shared
 * with Func_8023178.  (3) Settle the sp+0x144 walk-down direction.
 * (4) Drop in the 7-arm switch.  (5) Only then the body, respecting the
 * signed-compare bound above.
 *
 * PREDICTED BLOCKER: the aggregate layout, BEFORE reload gets a say.  This
 * is the one target of the four where the frame itself is the hard part,
 * and it is why it should be attempted LAST of the four despite having the
 * best oracle neighbourhood.
 */
