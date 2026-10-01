/* Anim_ScreenShatter (asm/rom_c9000/rom_cb1a4.s:1245, 0x080cbc0c, 1,051 ROM
 * instructions / 1132 encodings) --
 * NON-MATCHING, 1042 of 1132 encodings differ.
 *
 * READ THE FIGURE CORRECTLY.  1042 is objcmp's PRODUCTION-FLAG number and it is
 * NOT a distance: the instruction count is 1120 against 1132, TWELVE UNDER, so
 * the count axis is open and the objcmp figure saturates.  Rank this park by
 * aligncmp, never by 1042.
 *
 *   SIZE   2480 bytes against the ROM's 2508  (-28, i.e. -7 words)
 *   COUNT  1120 encodings against 1132        (-12)
 *   tools/aligncmp.py, reported SEPARATELY and never on the claim line:
 *       aligned-equal 511 of 1132 = 45.1%,  759 differing/ins/del in 188 hunks
 *       (first candidate: 437 of 1132 = 38.6%, 1003 in 165 hunks)
 *   RELOCATIONS: 69 rows ours against 69 ref, and the SYMBOL SEQUENCE is right
 *   entry for entry except (a) five veneer registers and (b) ONE adjacent pair
 *   transposed (`Func_80008d8` / `ewram_2020202`).  Every callee, every pooled
 *   data label (.Ledf90 .Ledfb1 .Ledfd2 .Lee016 .Lee037), both `gBuffer`
 *   entries, both `gDMATaskCount` entries, `_FILE_44`, `_FILE_7d`,
 *   `Task_BlitAnim`, `iwram_3001e74`, `iwram_3001ad0` and `gPtrs` are present,
 *   the right number of times, in the ROM's order.  That is the evidence the
 *   STRUCTURE is right and the residue is allocation, not reconstruction.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_ScreenShatter.c \
 *     asm/rom_c9000/rom_cb1a4.s --func Anim_ScreenShatter
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_ScreenShatter.c \
 *     asm/rom_c9000/rom_cb1a4.s Anim_ScreenShatter
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py <installed path>` prints the
 * filename and no rows -- PIN-FREE: no register pin, no barrier, no per-file
 * flag override, no fakematch.txt row.
 *
 * ================================================================
 * SPLIT SHAPE -- TEXT/DATA, AND split_s.py REFUSES UNTIL EIGHT EXPORTS EXIST
 * ================================================================
 *
 * `tools/datacheck.py asm/rom_c9000/rom_cb1a4.s` reports a `.rodata` section
 * and four functions, so converting ANY of them needs a TEXT/DATA split.
 * For Anim_ScreenShatter it names FIVE labels, not the two the batch-311
 * recon predicted:
 *
 *     Anim_ScreenShatter reads .Ledf90, .Ledfb1, .Ledfd2, .Lee016, .Lee037
 *
 * CORRECTION TO docs/RECON_b311b_four.txt section 2, which said the remainder
 * "will need at least `.global .Lee016` / `.global .Lee037`".  It needs five
 * for this function, and `tools/split_s.py --dry-run` REFUSES the split until
 * EIGHT exist, because Anim_Unused_SabreRain in the same file reads .Ledf7f,
 * .Ledf83 and .Ledf88 and they would cross the same file boundary:
 *
 *     .global .Ledf7f  .global .Ledf83  .global .Ledf88
 *     .global .Ledf90  .global .Ledfb1  .global .Ledfd2
 *     .global .Lee016  .global .Lee037
 *
 * A `.global` emits no bytes.  Add all eight, verify `make compare` is green
 * AFTER the exports and BEFORE the split, then re-run the dry run -- that is
 * the order split_s.py's own message asks for, and it keeps the two changes
 * separable.
 *
 * ================================================================
 * THE FRAME IS SIX GREPS HERE, NOT FOUR -- AND TWO OF THEM I HAD TO ADD
 * ================================================================
 *
 *     sub sp, #0x38                 total 56 bytes
 *     NO `(add|sub) sp, rN`         (no register-built frame)
 *     NO `mov rX, sp`, NO `add rX, sp, #K`, NO bare `add rX, sp`
 *     => ZERO stack aggregates, confirmed on all five address-forming greps.
 *
 * TWO INSTRUMENT DEFECTS FOUND, BOTH OF WHICH FAKE A SMALLER FRAME, AND BOTH
 * COST REAL TIME ON THIS FUNCTION.  Record them with the frame triad:
 *
 *  (i) THE REFERENCE `.s` AND gcc's OWN OUTPUT SPELL THE FRAME DIFFERENTLY.
 *      asm/ is disassembly: two-operand, hex -- `sub sp, #0x38`, `[sp, #0x28]`.
 *      gcc -S emits three-operand, DECIMAL -- `sub sp, sp, #56`, `[sp, #40]`.
 *      A grep written for one reports the other as FRAMELESS WITH NO SLOTS.
 *      Mine did exactly that: the candidate's first slot map came back with a
 *      single row and no `sub sp` at all.  Any frame or slot instrument must
 *      accept BOTH spellings or it silently lies about the side you did not
 *      write it for.  This is the same failure class as the >508-byte frame in
 *      docs/elevation.md "Frame triad corrected", with a new cause.
 *
 * (ii) `#0x..`-ONLY SLOT REGEXES DROP THE LOW SLOTS.  My first slot-map script
 *      matched `#(0x[0-9a-f]+)`, which cannot match `[sp, #4]` or `[sp, #8]`.
 *      It therefore hid TWO reference slots and manufactured a "12-byte
 *      phantom hole" that I spent a pass chasing.  Offsets below 0x10 are
 *      printed WITHOUT the 0x prefix by this disassembler.
 *
 * A SIXTH ADDRESS-FORMING IDIOM EXISTS AND NEITHER THE FOUR-GREP RECIPE NOR
 * docs/elevation.md's `mov rX, sp / add rX, #K` note covers it:
 *
 *     mov rX, #K        then        add rX, sp
 *
 * i.e. the immediate FIRST and sp added into it. BaseAnim_Tiamat uses it twice
 * (`mov r3, #0x3c / add r3, sp`, `mov r4, #0x3c / add r4, sp`), both times to
 * get an aggregate's address into a HIGH register (`mov r9, r3`), which is why
 * it cannot use `add rX, sp, #K` -- that form needs a low rd.  Grep
 * `add rX, sp$` (bare, no immediate) as a fifth form and the `mov rX,#K`
 * immediately above it as a sixth.  Anim_ScreenShatter has neither; it is
 * genuinely aggregate-free, which is now checked rather than assumed.
 *
 * ================================================================
 * THE SPILL-SLOT MAP -- AND THE FRAME SIZE MATCHING IS A COINCIDENCE
 * ================================================================
 *
 * Scalars descending, with reference access counts beside ours.  The top NINE
 * line up access-for-access, which is the strongest structural evidence in
 * this park:
 *
 *     ref slot  quantity                        ref acc   ours   our slot
 *     0x28      base  = galloc_iwram(0x27,..)      20       19     0x38
 *     0x24      buf   = galloc_iwram(0x28,..)       4        4     0x34
 *     0x20      frame (the 0x80-frame counter)     17       17     0x30
 *     0x1c      blitB = gPtrs[0xbc/4]               2        2     0x2c
 *     0x18      blitA = gPtrs[0xb8/4]               2        2     0x28
 *     0x14      view  = iwram_3001e74[0]            5        5     0x24
 *     0x10      limit = frame << 5                  2        3     0x20
 *     0x0c      rad   (the circle radius)           5        5     0x1c
 *     0x08      d     (midpoint-circle decider)     5        5     0x18
 *     --        the inner counter j                 --      10     0x14
 *     --        an x-part CSE temp                  --       2     0x10
 *     --        gBuffer spilled round a long jump   --       2     0x08
 *     0x04      outgoing argument 6                  2        2     0x04
 *     0x00      outgoing argument 5                  7        7     0x00
 *
 * THE FRAME SIZE IS 0x38 ON BOTH SIDES AND THAT IS AN ARITHMETIC COINCIDENCE,
 * EXACTLY LIKE THE SIZE COINCIDENCE docs/elevation.md RUNG 7 DESCRIBES BUT ON
 * A NEW AXIS.  The ROM has 9 live scalars plus 2 argument words = 11 words =
 * 44 bytes, and THREE WORDS AT 0x2c..0x37 THAT NOTHING EVER ADDRESSES.  We
 * have 12 live scalars plus 2 argument words = 14 words = 56 bytes and ONE
 * dead word (0x0c).  Both come to 0x38.  **Never read frame-size equality as
 * slot-map equality**; carry the slot map as its own column.
 *
 * THAT ALSO KILLS THE PHANTOM-HOLE READING, AND THE PROOF IS OUR OWN BUILD.
 * The ROM's unaddressed 0x2c..0x37 is NOT evidence of three more source
 * variables: reload allocates a slot per spilled pseudo and can then leave it
 * unused, and our own object reproduces the phenomenon at 0x0c. Every
 * address-forming form is independently ruled out above, so the gap cannot be
 * an aggregate either. Do not go looking for three more declarations.
 *
 * RANKING CORRECTION TO THE BRIEF AND TO RECON_b311b_four: this function has
 * NINE spilled scalars, not eight. `[sp, #8]` carries ldr=2/str=3 -- it is the
 * midpoint-circle decision variable, read-modify-written at `.Lcc2fa`, not
 * outgoing argument space. The max call arity in the whole function is SIX
 * (the two six-argument blits at `.Lcc368` and `.Lcc4b8` are the only users of
 * `[sp, #4]`), so the argument block is 0x00..0x07 and 0x08 is a real slot.
 * The brief's own grep-4 rule ("`str` with NO matching load") is what settles
 * it, and it settles it the other way from the figure in the triage table.
 *
 * ================================================================
 * THE COMPARISON CENSUS -- AND THE RAW CENSUS MIS-SIGNALS ON THIS FUNCTION
 * ================================================================
 *
 *     grep -coE '\b(bne|blt|ble|bgt|bge)\b'  on the reference body:
 *        bne 16   signed (blt/ble/bgt/bge) 48   beq 2   bhi 1   bls 0
 *
 * The brief leads with that 48-against-16 and says Anim_Gaia's all-`!=` rule
 * "would corrupt 48 sites here".  MEASURED, THAT IS NOT WHERE THE 48 LIVE.
 * I classified every backward edge.  ELEVEN loops close this function and
 * EVERY ONE OF THEM CLOSES ON `bne`/`beq` -- the two tilemap loops (8, 0x10),
 * the 0x21 table seed, the 0x80 palette cross-fade, the circle's OUTER loop
 * (`rad != limit`, tested `bne` at entry and `beq` at the latch), the 0x21
 * shard blit, the 0x40 palette ramp, the 0x20 debris seed, the 0x20 debris
 * update and the 0x80 frame loop.  Exactly ONE loop closes on a signed
 * compare: the midpoint-circle INNER loop, `cmp r9, r11 / blt`, which is
 * `while (i >= j)`.
 *
 * So all 48 signed compares but one are NOT loops.  They are
 *   - the 24 coordinate CLAMPS (`if (v < 0) v = 0;` / `if (v > 0xff) v = 0xff;`
 *     / `> 0x77`), two per distinct coordinate, twelve coordinates per
 *     iteration of the plot block;
 *   - the SIGNED-DIVISION bias corrections, see below;
 *   - the frame-phase tests (`frame <= 3`, `frame > 7`, `frame <= 0x32`,
 *     `frame > 3`, `frame > 0x34`, `k > 0x1f`, `k > 5`, `p->t <= 0x27`).
 *
 * THE METHOD FIX IS ONE LINE: the census must count only branches that CLOSE A
 * BACKWARD EDGE.  RECON_b311b_four already says this in a caveat aimed at
 * `bhi`/`bls`, and noticed it locally for the tilemap pair ("the census is per
 * LOOP, not per function") -- it generalises to the whole function and inverts
 * the headline. Anim_Gaia's `!=` rule TRANSFERS HERE ALMOST COMPLETELY: ten of
 * eleven loops want `!=`, and writing them that way is in the delivered body.
 * A raw mnemonic count is a count of COMPARISONS, not of loops, and on a
 * clamp-heavy or division-heavy function it points the wrong way.
 *
 * ================================================================
 * WHAT THIS FUNCTION IS
 * ================================================================
 *
 * The full-screen "shatter" transition, 0x80 frames.  `FILE_VFX_SCREEN_SHATTER`
 * is `FILE_44` in include/file_table.h, which independently confirms the
 * reading of the `LoadVFXFile` argument.
 *
 * It ALLOCATES ITS OWN BUFFERS and never reads `iwram_3001eec` -- three
 * `galloc_iwram` calls open it (tag 0x27 size 0x782c, tag 0x28 size 0x80<<7,
 * tag 0x29 size 0x302, the third's return value unused) and five `gfree` calls
 * close it, which is the shape docs/battle-animations.md tabulates.
 *
 * Phases, with the frame numbers that gate them:
 *   - setup: BG2 affine + both windows + BLDALPHA, a 0x10 x 8 halfword fill of
 *     the BG tilemap at 0x6003800, two `Func_80008d4` clears, an HBlank
 *     register-queue push, then 0x21 crack records seeded from `.Lee016` /
 *     `.Lee037` (both verified 0x21 = 33 bytes in the .rodata tail, which is
 *     what proves the loop bound and the pairing);
 *   - frames 0..3: an EXPANDING CIRCLE plotted into `gBuffer` with a midpoint
 *     (Bresenham) circle, see below, then blitted to 0x6008000;
 *   - frames 0..0x32: the 0x21 cracks blitted through `blitA`;
 *   - frame 0x33: load `FILE_7d`, ramp the palette, set BLDCNT, seed 0x20
 *     debris particles from `Random`;
 *   - frames >0x34: the debris blitted through `blitB`, staggered by `i / 4`.
 *
 * THE TWO HBlank-QUEUE PUSHES RECON PREDICTED ARE ACTUALLY THREE (setup,
 * post-seed, and teardown), with payloads {0x7741, 0x80<<19, 0x80<<10},
 * {0x1f81, REG_ADDR_BG1CNT, 0x80<<10} and {0x7541, 0x80<<19, 0x80<<10}.  They
 * are written out in full, not helper-called, per the bank's "duplicated ROM
 * code means duplicated source" rule, and the body is taken verbatim from the
 * SOLVED `QueuePush1` in src/non_matching/rom_c9000/80cd86c.c -- including
 * `SET_IO(REG_IME, REG_ADDR_IME)`, which is what explains the otherwise
 * baffling `strh r0, [r0]` (the register's own ADDRESS stored into it).
 * Do not re-derive this idiom; it is already landed in this bank.
 *
 * ================================================================
 * THE CIRCLE BLOCK, WHICH IS HALF THE FUNCTION
 * ================================================================
 *
 * `gBuffer` is a 256 x 120 8bpp TILED surface -- 0x7800 bytes, which is exactly
 * the `0xf0 << 7` length of the two `Func_8001af8` copies, and the independent
 * confirmation of the geometry.  A plot is
 *
 *     gBuffer[(x & 7) + (x / 8 << 6) + ((y & 7) << 3) + (y / 8 << 11)] = 2
 *
 * EVERY DIVISION IS A SIGNED `/`, NOT A SHIFT, AND THE ROM PROVES IT.  `x / 8`
 * compiles to `cmp x,#0 / bge L / add x,#7 / L: asr x,#3` -- the bias
 * correction is only there for signed division; `x >> 3` is a bare `asr`.
 * The same tell appears at `i / 4` (`add #3`) in the debris loop and at `i / 2`
 * in the palette ramp, where the correction takes gcc's OTHER shape
 * (`lsr r3,x,#31 / add / asr #1`) because for /2 the bias is 1.  Three
 * divisions, two spellings, all signed: any `>>` here is a reconstruction bug.
 *
 * The loop is a midpoint circle with 8-way symmetry, DOUBLED on x:
 *
 *     i = rad;  j = 0;  d = rad;
 *     while (i >= j) {
 *         ... 16 plots ...
 *         d = d - 2 * j - 1;
 *         if (d < 0) { d = d + 2 * i - 2; i--; }
 *         j++;
 *     }
 *
 * The entry test compiles to `cmp i,#0 / bge` because j is 0 there and gcc
 * constant-propagates it -- that is the evidence the source really is
 * `while (i >= j)` and not `while (i >= 0)`.
 *
 * SIXTEEN plots, in FOUR groups of four, and the plot order inside every group
 * is (x1,y1), (x1,y0), (x0,y1), (x0,y0) -- verified by following the register
 * that each `strb` addresses, in all four groups:
 *     group 1  centre x 0x60, y 0x3c, x from i and y from j
 *     group 2  centre x 0x61            (the same y parts, reused by cse)
 *     group 3  centre x 0x60, y 0x3c, x from j and y from i  (the other octants)
 *     group 4  centre x 0x61
 *
 * CLAMP ORDER IS SOURCE ORDER AND IT IS NOT THE SAME IN GROUPS 1 AND 3.
 * Group 1 clamps y0, y1, x0, x1; group 3 clamps x0, x1, y0, y1.  The invariant
 * behind it is that THE j-DERIVED PAIR IS CLAMPED FIRST in both, and the body
 * is written that way.  This is readable only because a clamp is a BRANCH:
 * sched2 runs and freely reorders the four arithmetic computations inside the
 * block (they come out x0, x1, y0, y1 in group 1, the reverse of the clamps),
 * but it cannot reorder across a conditional jump.  **Arithmetic order in a
 * basic block carries no source information here; branch order does.**
 *
 * THE UNCLAMPED VALUE IS REUSED.  The ROM saves `0x60 - i` in r14 BEFORE
 * clamping it and group 2 computes `r14 + 1`, so writing group 2's x as
 * `0x61 - i` is right and gcc recovers the shared subexpression itself.
 *
 * ================================================================
 * LEVERS, EVERY ONE WITH ITS FIGURE
 * ================================================================
 *
 * (0) THE STARTING POINT.  Written straight off the disassembly with the
 *     declaration block of the SAME-FILE park
 *     src/non_matching/rom_c9000/cb1a4_EPowerUp.c and the `Part` / `State`
 *     shapes from src/non_matching/rom_c9000/decls.h: SIZE 2548/2508, COUNT
 *     1154/1132, aligned 437 of 1132 (38.6%), 165 hunks -- and a relocation
 *     symbol sequence already correct except five veneer registers and one
 *     transposed pair.  The brief's claim that a landed or parked sibling is
 *     the strongest evidence in this project held completely: `galloc_iwram`,
 *     `CopyFn`/`DrawFn`, the `(Type *)(base + 0xNNNN)` idiom, `struct DmaQueue`
 *     and the whole `QueuePush1` body were READ off siblings, not guessed.
 *
 * (1) THE PLOT INDEX MUST BE A FLAT FOUR-TERM SUM WITH `<<`, NOT `*`, AND THIS
 *     IS THE BIGGEST SINGLE MOVE: 437 -> 511 of 1132 (38.6% -> 45.1%),
 *     COUNT 1154 -> 1120, SIZE 2548 -> 2480.
 *     Written with multiplies -- `((y) & 7) * 8 + ((x) / 8 * 0x40 + ((x) & 7))
 *     + (y) / 8 * 0x800` -- fold FACTORS OUT THE COMMON POWER OF TWO, because
 *     8, 0x40 and 0x800 are all multiples of 8, and emits a HORNER form with
 *     two extra `lsl rX, rX, #3` per plot.  Sixteen plots, so it is expensive.
 *     Writing the identical arithmetic as shifts --
 *     `((x) & 7) + ((x) / 8 << 6) + (((y) & 7) << 3) + ((y) / 8 << 11)` --
 *     leaves fold nothing to factor and gives the ROM's flat sum of four terms.
 *     THE `/ 8` MUST STAY A DIVISION (see above); only the MULTIPLIES become
 *     shifts.  That mixture is the whole lever and it is not obvious.
 *
 * (2) THE ROM'S ADD ORDER DOES **NOT** GIVE THE SOURCE'S PARENTHESISATION --
 *     MEASURED NEGATIVE, 511 -> 461 of 1132 (45.1% -> 40.7%), 182 hunks.
 *     The reference accumulates the index as `(x/8<<6) + (x&7)`, then
 *     `+ ((y&7)<<3)`, then `+ ((y/8)<<11)`, so I tried exactly that grouping
 *     with shifts: `(((y) & 7) << 3) + ((((x) / 8) << 6) + ((x) & 7)) +
 *     (((y) / 8) << 11)`.  It is FIFTY ENCODINGS WORSE than the flat sum.
 *     fold reassociates a flat sum and then cse1 rebuilds the ROM's grouping
 *     from the subexpressions that are actually shared; writing the grouping by
 *     hand instead PINS it and defeats the sharing.  **Do not read an
 *     accumulation order back out of the reference as parenthesisation** -- the
 *     same caution docs/elevation.md records for `mul` operand order, on a new
 *     construct.
 *
 * (3) `base + 0x7828` IS NOT A NAMED LOCAL, AND THE BANK RULE HOLDS HERE.
 *     Anim_Gaia's lever (1) and the Anim_Ragnarok park's "re-derive
 *     base+0x7828, never name it" rule both transfer: all three reads are
 *     written out as `(*(State **)(base + 0x7828))->ids[0]` at their sites.
 *     The reference has the matching three `ldr rX, =0x7828 / add / ldr`
 *     sequences and NO slot for it, and our slot map has none either -- which
 *     is the check that it was done right.  Anim_Ragnarok remains the
 *     documented EXCEPTION, not the pattern.
 *
 * (4) `0x24` IS `State.ids[0]`, WHICH IS WHY THE BANK'S `State` SHAPE MATTERS.
 *     `State { int f0..f20; short ids[4]; }` puts `ids` at 0x24, so the ROM's
 *     `mov r7,#0x24 / ldrsh r0,[r3,r7]` is `->ids[0]` and needs no cast.
 *     Thumb-1 has no immediate-offset LDRSH, so the register offset is
 *     automatic and carries no information -- do not read it as an index.
 *
 * (5) THE 16.16 INTEGER PART IS `((short *)&p->x)[1]`, taken from Anim_Gaia's
 *     recorded idiom (Anim_Ragnarok spells the same thing
 *     `*(short *)((char *)q + 2)`; both are in the tree).  Both blit calls read
 *     x and y this way and both come out as the ROM's `ldrsh` pair.
 *
 * (6) `ewram_2020202` IS A SYMBOL USED AS A VALUE, NOT A SMALL CONSTANT.
 *     `fill((void *)0x6008000, 0xf0 << 7, (u32)ewram_2020202)` reproduces the
 *     ROM's `R_ARM_ABS32 ewram_2020202` relocation row.  Writing the literal
 *     0x2020202 would have produced a plain pool word and LOST a relocation --
 *     this is docs/elevation.md's "a pooled small constant is a symbol"
 *     screen, and the precedent for taking the address of such a name as a
 *     value is already in src/non_matching/rom_c9000/80e6638.c
 *     (`(u32)ewram_2010002`).
 *
 * (7) THE UNSIGNED RANGE TEST IS WRITTEN AS ONE.  `if ((unsigned)(frame - 8)
 *     <= 0x2a)` gives the ROM's `sub r1,#8 / cmp r1,#0x2a / bhi`.  This is the
 *     construct Anim_Gaia's lever (7) and docs/elevation.md's "EQUALITY
 *     CHAINS, NOT RANGE COMPARES, when the ROM's branch is UNSIGNED" describe;
 *     here the body is a plain `if` rather than an if/else, so there are no
 *     arms to get the wrong way round.
 *
 * (8) `int j = 0;` INSIDE THE LOOP against a hoisted declaration: INERT ON
 *     EVERY FIGURE, AND INERT ON THE SLOT MAP TOO.  511 both, 1120 both, 2480
 *     both, 188 hunks both, and the slot map identical row for row.  This was
 *     tried BECAUSE the brief's "initialise-at-declaration LOWERS your own
 *     allocation priority" lever looked like it was firing against us -- `j`
 *     is the one hot quantity we spill that the ROM keeps in r11, and its
 *     initialiser IS live, so the lever's stated precondition holds.  It still
 *     did nothing.  The object is not byte-identical (14 lines differ) but the
 *     entire difference is a register RENAME (r4<->r5) and one store reorder in
 *     the loop preheader, at equal count.  **A BOUND ON THE LEVER: the
 *     precondition being satisfied is not sufficient.**  Kept in the delivered
 *     body only because the hoisted form's preheader order (i, then j, then d)
 *     is the ROM's `mov r1,r7 / mov r9,r7 / mov r11,r0` order.
 *
 * (9) FLAGS, EACH ACTUALLY RUN ON THIS FUNCTION -- AND gcse IS RULED OUT.
 *     The brief says "run `-fno-gcse` before naming gcse", so it was run
 *     before anything was attributed, and it is as well that it was:
 *       -fno-gcse               frame 60, 14 slots, 16 gBuffer pool loads
 *                               -- IDENTICAL to production.  gcse DOES NOT
 *                               TOUCH this function.  Nothing here may be
 *                               attributed to gcse or PRE.
 *       -fno-cse-follow-jumps   identical to production, also inert.
 *       -fno-rerun-cse-after-loop  NEGATIVE: frame 60 -> 68, 14 slots -> 15,
 *                               and gBuffer's pool loads drop 16 -> 11, i.e. it
 *                               converts rematerialisation into pressure.
 *                               NOTE THE PRECONDITION: this function has
 *                               eleven loops, so docs/elevation.md's
 *                               "`has a loop` is NOT the precondition" finding
 *                               gets its complement here -- the precondition
 *                               HOLDS and the flag is still negative.
 *     sched1 does not run in this project; sched2 does, and lever (2)'s
 *     clamp-order argument depends on exactly that.
 *
 * (10) A CONCLUSION I DREW AND THEN MEASURED FALSE, RECORDED BECAUSE THE
 *      MISREADING IS THE REUSABLE PART.  Seeing a `gBuffer` address in a spill
 *      slot, I wrote down that cse1 had hoisted it function-wide where the ROM
 *      re-derives it -- the brief's "deny cse1 a hoisted address" lever,
 *      apparently firing.  Then I counted the pool loads: OURS 16, REFERENCE
 *      16.  `gBuffer` is re-derived exactly as often as the ROM does it, and
 *      slot 0x08 is ONE spill around ONE long jump, not a hoist.  **Count the
 *      construct on both sides before naming a pass**; a single spilled copy
 *      of an address is not evidence of commoning, and this is the same trap
 *      docs/elevation.md records as "MEASURE COMMONING BY THE COPY COUNT, NOT
 *      BY THE BUILD REGISTER".
 *
 * ================================================================
 * BLOCKERS, EACH ATTRIBUTED, AND THE RESIDUE ADDS UP
 * ================================================================
 *
 * 1. THREE SURPLUS LIVE SPILL SLOTS, which is the whole slot-map offset of
 *    0xc and therefore most of the 759 differing encodings: EVERY `[sp, #imm]`
 *    field in the function reads 12 too high.  `local-alloc`/`global.c` give
 *    the inner plot block one register less than the ROM had, so `j` (10
 *    accesses) is spilled where the ROM holds it in r11, and an x-part cse temp
 *    and one `gBuffer` copy take slots too.  THE STRUCTURE IS NOT IN QUESTION
 *    -- the nine quantities the ROM spills are all present with the right
 *    access counts -- so this is pressure, not reconstruction.  The untried
 *    move is to LOWER a competing allocno rather than try to raise `j`;
 *    lever (8) shows the obvious spelling of that is not it.
 *
 * 2. TWELVE ENCODINGS SHORT AND SEVEN POOL WORDS SHORT.  Reported on SEPARATE
 *    axes, per rung 3: -12 instructions and -28 bytes are NOT one defect, and
 *    their sum is meaningless.  Both are consequences of item 1 -- a spilled
 *    quantity costs instructions AND moves pool distances, which is why the
 *    pool-word count moves with it.
 *
 * 3. FIVE VENEER REGISTERS, the same mechanism Anim_Gaia's blocker 2(b) and
 *    blocker 3 record.  The ROM calls `_call_via_r5` x2 (the two
 *    `Func_80008d4` clears), `_call_via_r3` (the first `Func_8001af8`),
 *    `_call_via_r5` (the `Func_80008d8` palette fill), `_call_via_r3` (the
 *    second `Func_8001af8`) and `_call_via_r7` (the debris blit); we produce
 *    r6, r6, r4, r4, r6 and r6.  Anim_Gaia's finding -- assign the function
 *    pointer AFTER any intervening call so it need not survive one -- is
 *    already followed for `clear`, `copy` and `fill`; what differs is which
 *    allocno wins the low scratch register, i.e. item 1 again.
 *    RECON_b311b_four predicted "r3, r4, r5 and r7 veneers, four distinct
 *    registers, so expect one pointer local per region, not one for the
 *    function".  MEASURED, that is only half right: the ROM's four distinct
 *    veneers come from THREE pointer values (`Func_80008d4` twice on r5,
 *    `Func_8001af8` twice on r3, `Func_80008d8` once on r5 and the blit on r7),
 *    and the two `Func_8001af8` calls are 2,000 bytes apart yet share r3.  One
 *    local per region is not what the reference shows.
 *
 * 4. ONE TRANSPOSED POOL PAIR: the ROM dumps `Func_80008d8` then
 *    `ewram_2020202`; we dump them the other way.  This is
 *    docs/elevation.md's "gcc evaluates arguments RIGHT-TO-LEFT" pool-order
 *    item, 1 relocation row's worth of offsets and 0 encodings of count.  Not
 *    chased: it moves with item 1.
 *
 * ================================================================
 * AGGREGATE COUNTS RE-RESOLVED BY REGION READING (coordinator correction)
 * ================================================================
 *
 * AGGREGATE RE-CLASSIFICATION -- every `add rX, sp, #K` / `mov rX, sp` RESOLVED BY
 * READING ITS REGION, BLOCK-AWARE (stopping at any label or branch).
 *
 * The mechanism behind the inflation: Thumb-1 has NO sp-relative ldrh/strh/ldrb/
 * strb, so every SUB-WORD stack scalar must materialise a base with
 * `add rX, sp, #K` and is indistinguishable at the grep from a real aggregate.
 *
 *   function            brief  TRUE  resolution of each site
 *   Anim_ScreenShatter    0      0   EXACT, and exact for a stronger reason than
 *                                    "the bound is 0": the function has ZERO
 *                                    address-forming forms of ANY kind (no
 *                                    `mov rX,sp`, no `add rX,sp,#K`, no bare
 *                                    `add rX,sp`, no register-built frame). All
 *                                    71 of its `[sp]` accesses are word-width
 *                                    ldr/str. Because no base register is ever
 *                                    materialised, NO STORE CAN HIDE BEHIND ONE
 *                                    -- so the slot census is complete and the
 *                                    phantom-hole re-check the coordinator asks
 *                                    for is vacuously satisfied here.
 *   BaseAnim_Tiamat       1      1   EXACT. sp+0x3c: `add r6,sp,#0x3c` then
 *                                    `mov r1,r6 / bl GetBattleActorPos2` -- the
 *                                    address is PASSED AS AN ARGUMENT, and it is
 *                                    used at two offsets ([r6] and [r6,#4]).
 *                                    Genuine aggregate, a vec3_t-style out-param.
 *                                    The other two sites are the SIXTH idiom
 *                                    (`mov r3,#0x3c / add r3,sp`, same for r4),
 *                                    both at the SAME offset 0x3c, so they are
 *                                    extra references to the one object, not
 *                                    extra objects. They exist because the
 *                                    address goes into a HIGH register (`mov
 *                                    r9,r3`), which `add rX,sp,#K` cannot target.
 *   BaseAnim_ParticleSpray 3     3   EXACT. sp+0x8c and sp+0x88: addresses stored
 *                                    to `[sp]` and `[sp,#4]` as outgoing
 *                                    arguments 5 and 6 of `Anim_Djinni` -- two
 *                                    address-taken out-params. sp+0x90: `mov r1,r7
 *                                    / bl Func_80e3944` then `ldr/str [r7]` -- a
 *                                    vec3_t out-param (Func_80e3944's signature is
 *                                    already declared in the EPowerUp park).
 *                                    All three pass the "passed as an argument"
 *                                    test.
 *   Anim_Neptune          5      4   *** CORRECTED, AND THE BRIEF'S TABLE IS ONE
 *                                    TOO HIGH. ***
 *                                    sp+0x50  AGG. `mov r2,sp / add r2,#0x50`
 *                                             (the documented fifth idiom), then
 *                                             `str r2,[sp,#0x18]` -- the address
 *                                             is stored INTO ANOTHER SLOT, so slot
 *                                             0x18 (10 accesses) is a DECLARED
 *                                             POINTER TO A LOCAL. Also `str
 *                                             r3,[r2,#4]`.
 *                                    sp+0x64  AGG. `str r3,[r2,#4]` and
 *                                             `str r3,[r2,#0xc]` -- varying
 *                                             offsets, >= 0x10 bytes.
 *                                    sp+0x58  AGG. `mov r1,r6 / bl
 *                                             GetBattleActorPos2` -- argument.
 *                                    sp+0x48  AGG. `mov r2,r7` -- argument.
 *                                             *** AND THIS SITE IS THE TRAP. ***
 *                                             Three blocks later r7 is REASSIGNED
 *                                             (`ldr r7,=gBuffer`) and then walked
 *                                             (`[r7]`, `[r7,#4]`, `[r7,#0xc]`,
 *                                             `[r7,#0x10]`, `[r7,#0x18]`,
 *                                             `add r7,#0x1c`). A first-use scan
 *                                             that is not block-aware reads those
 *                                             six offsets as the STACK object and
 *                                             reports a 0x1c-byte aggregate at
 *                                             sp+0x48. It is a gBuffer particle
 *                                             walk and has nothing to do with sp.
 *                                    sp+0x28  NOT AN AGGREGATE. `add r4,sp,#0x28
 *                                             / ldrh r4,[r4] / strh r4,[r3,#4]`.
 *                                             One sub-word load, base register
 *                                             overwritten by its own result on the
 *                                             next instruction. This is a u16
 *                                             SCALAR and it is the exact shape the
 *                                             coordinator described.
 *
 * CONSEQUENCE FOR THE SELECTION PREMISE. The group was picked as the family's
 * "simplest frames" on the aggregate column. After resolution the column reads
 * 0 / 1 / 3 / 4 instead of 0 / 1 / 3 / 5 -- so the ORDERING the brief gave is
 * unchanged and the premise survives, with Neptune one step less bad than stated.
 * ScreenShatter leading is confirmed on the strongest possible version of the
 * test, which is why it is the one taken to depth.
 *
 * A SEVENTH FORM, for the frame recipe: an address-taken SCALAR (ParticleSpray's
 * 0x88/0x8c, and any `&local` passed to a callee) also surfaces at the
 * `add rX, sp, #K` grep and is neither an aggregate nor a sub-word scalar. The
 * discriminator that actually works is not "aggregate vs scalar" but
 * "does a base register get materialised at all" -- and when one does, only
 * reading the region settles what it is.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

typedef void (*ClearFn)(void *dst, s32 len);
typedef void (*FillFn)(void *dst, u32 size, u32 value);
typedef void (*CopyFn)(void *dst, void *src, int len);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

struct DmaTransfer {
    const void *src;
    void *dest;
    u32 control;
};

struct DmaQueue {
    u16 count;
    struct DmaTransfer tasks[32];
};

extern struct DmaQueue gDMATaskCount;
extern unsigned char gBuffer[];
extern void *gPtrs[];
extern char *iwram_3001e74[];
extern unsigned short iwram_3001ad0[];
extern unsigned char ewram_2020202[];

extern unsigned char Ledf90[] __asm__(".Ledf90");
extern unsigned char Ledfb1[] __asm__(".Ledfb1");
extern unsigned short Ledfd2[] __asm__(".Ledfd2");
extern unsigned char Lee016[] __asm__(".Lee016");
extern unsigned char Lee037[] __asm__(".Lee037");

extern void *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void WaitFrames(unsigned int n);
extern int Random(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern int Func_80cd508(void);
extern void Func_80cd52c(void);
extern void Func_80d6888(int t, int a, int b, int c, int d);
extern void Func_80e3908(Part *p, int a, int b);
extern void _AnimTransitionIn(int a, int file, int c);
extern void _Func_80c0774(int a, unsigned short b, int c);
extern int _Func_80c0cec(int a, int b, int c, int d);
extern void Func_80008d4(void *dst, s32 len);
extern void Func_80008d8(void *dst, u32 size, u32 value);
extern void Func_8001af8(void *dst, void *src, int len);

#define PUT(x, y) \
    gBuffer[((x) & 7) + (((x) / 8) << 6) + (((y) & 7) << 3) + (((y) / 8) << 11)] = 2

void Anim_ScreenShatter(void *context)
{
    unsigned char *base;
    void *buf;
    int frame;
    DrawFn blitB;
    DrawFn blitA;
    char *view;
    int limit;
    int rad;
    int d;
    int *hi;
    ClearFn clear;
    FillFn fill;
    CopyFn copy;
    Part *p;
    int i, j, k, idx;
    int x0, x1, y0, y1;
    u32 saved;
    int count;
    u32 *task;

    base = galloc_iwram(0x27, 0x782c);
    buf = galloc_iwram(0x28, 0x80 << 7);
    galloc_iwram(0x29, 0x302);
    view = iwram_3001e74[0];
    hi = (int *)iwram_3001e74[35];
    *(void **)(base + 0x7828) = context;
    Func_80cd508();
    hi[3] = 1;
    iwram_3001ad0[3] = 0x20;
    _Func_80c0774(1, *(unsigned short *)(view + 0xc9 * 8), 0);
    REG_BG2CNT = 0x784;
    _Func_80c0cec(0, 0, 0, 0x64);
    hi[3] = 0;
    REG_BG2X = 0;
    REG_BG2Y = 0xfffff000;
    REG_BG2PA = 0x80;
    REG_BG2PB = 0;
    REG_BG2PC = 0;
    REG_BG2PD = 0x100;
    REG_WIN0H = 0xf0;
    REG_WIN0V = 0x1088;
    REG_WIN1H = 0xf0;
    REG_WIN1V = 0x1088;
    REG_WININ = 0x3537;
    REG_WINOUT = 0x3f21;

    idx = 0;
    for (i = 0; i != 0x10; i++) {
        for (k = 0; k != 8; k++) {
            *(vu16 *)(0x6003800 + idx) =
                (short)((0x100 + i * 0x1000 + k * 0x200) | (i * 0x10 + k * 2));
            idx += 2;
        }
    }

    clear = Func_80008d4;
    clear(buf, 0x80 << 7);
    clear((void *)0x6004000, 0x80 << 7);

    saved = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = gDMATaskCount.count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)&gDMATaskCount + 4);
        *(u16 *)&gDMATaskCount = count + 1;
        *task++ = 0x7741;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, saved);

    REG_BLDALPHA = 0x1010;
    REG_BLDCNT = 0;
    LoadVFXFile(FILE_44, base, 1, 1);
    *(int *)(base + 0xef * 0x80) = 1;
    *(int *)(base + 0x7784) = 0;
    StartTask(Task_BlitAnim, 0x90 * 8);

    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    blitA = (DrawFn)gPtrs[0xb8 / 4];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    blitB = (DrawFn)gPtrs[0xbc / 4];
    rad = 0;
    p = (Part *)(base + 0xe1 * 0x80);
    for (i = 0; i != 0x21; i++) {
        p->x = Lee016[i] << 16;
        p->y = Lee037[i] << 16;
        p->vx = (p->x - 0x200000) >> 2;
        p->vy = (p->y - 0x3c0000) >> 2;
        p++;
    }
    copy = Func_8001af8;
    copy(gBuffer, (void *)0x6008000, 0xf0 << 7);
    fill = Func_80008d8;
    fill(gBuffer, 0xf0 << 7, 0x1010101);
    hi[4] = 1;
    *(int *)(base + 0x77a0) = iwram_3001ad0[2];
    *(int *)(base + 0x77a4) = iwram_3001ad0[3];
    iwram_3001ad0[2] = 0;

    saved = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = gDMATaskCount.count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)&gDMATaskCount + 4);
        *(u16 *)&gDMATaskCount = count + 1;
        *task++ = 0x1f81;
        *task++ = REG_ADDR_BG1CNT;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, saved);

    fill((void *)0x50000c0, 0x80 * 2, 0x7fff7fff);
    _PlaySound(0xd4);
    Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 3, 0, 0x1e);
    frame = 0;
    do {
        if (frame == 2)
            _PlaySound(0xd4);
        if (frame == 3)
            _PlaySound(0xd4);
        if (frame == 0x1c)
            Func_80d6888((*(State **)(base + 0x7828))->ids[0], -1, 3, -1, 0);
        if (frame == 0x20)
            _PlaySound(0x95);
        if (frame == 5) {
            _PlaySound(0x91);
            iwram_3001ad0[2] = *(int *)(base + 0x77a0);
            _AnimTransitionIn(1, *(unsigned short *)(view + 0xc9 * 8), -1);
        }
        if (frame > 7) {
            vu16 *pal = (vu16 *)0x50000c0;
            unsigned short *src = (unsigned short *)(view + 0x544);
            int mask = 0x1f;
            for (i = 0; i != 0x80; i++) {
                int c = *pal;
                int r = mask & c;
                int g = (c << 16) >> 21 & mask;
                int b = (c << 16) >> 26 & mask;
                int c2 = *src;
                int r2 = mask & c2;
                int g2 = (c2 << 16) >> 21 & mask;
                int b2 = (c2 << 16) >> 26 & mask;
                src++;
                if (r < r2)
                    r++;
                else if (r > r2)
                    r--;
                if (g < g2)
                    g++;
                else if (g > g2)
                    g--;
                if (b < b2)
                    b++;
                else if (b > b2)
                    b--;
                *pal = b << 10 | g << 5 | r;
                pal++;
            }
        }
        if (frame == 4)
            fill((void *)0x6008000, 0xf0 << 7, (u32)ewram_2020202);
        if (frame <= 3) {
            k = frame * 4 + 8;
            limit = frame << 5;
            *(vu16 *)0x5000004 = k << 10 | k << 5 | k;
            while (rad != limit) {
                i = rad;
                j = 0;
                d = rad;
                while (i >= j) {
                    y0 = 0x3c - j;
                    if (y0 < 0)
                        y0 = 0;
                    y1 = j + 0x3c;
                    if (y1 > 0x77)
                        y1 = 0x77;
                    x0 = 0x60 - i;
                    if (x0 < 0)
                        x0 = 0;
                    x1 = i + 0x60;
                    if (x1 > 0xff)
                        x1 = 0xff;
                    PUT(x1, y1);
                    PUT(x1, y0);
                    PUT(x0, y1);
                    PUT(x0, y0);
                    x0 = 0x61 - i;
                    if (x0 < 0)
                        x0 = 0;
                    x1 = i + 0x61;
                    if (x1 > 0xff)
                        x1 = 0xff;
                    PUT(x1, y1);
                    PUT(x1, y0);
                    PUT(x0, y1);
                    PUT(x0, y0);
                    x0 = 0x60 - j;
                    if (x0 < 0)
                        x0 = 0;
                    x1 = j + 0x60;
                    if (x1 > 0xff)
                        x1 = 0xff;
                    y0 = 0x3c - i;
                    if (y0 < 0)
                        y0 = 0;
                    y1 = i + 0x3c;
                    if (y1 > 0x77)
                        y1 = 0x77;
                    PUT(x1, y1);
                    PUT(x1, y0);
                    PUT(x0, y1);
                    PUT(x0, y0);
                    x0 = 0x61 - j;
                    if (x0 < 0)
                        x0 = 0;
                    x1 = j + 0x61;
                    if (x1 > 0xff)
                        x1 = 0xff;
                    PUT(x1, y1);
                    PUT(x1, y0);
                    PUT(x0, y1);
                    PUT(x0, y0);
                    d = d - 2 * j - 1;
                    if (d < 0) {
                        d = d + 2 * i - 2;
                        i--;
                    }
                    j++;
                }
                rad++;
            }
            copy((void *)0x6008000, gBuffer, 0xf0 << 7);
        }
        if (frame <= 0x32) {
            p = (Part *)(base + 0xe1 * 0x80);
            for (i = 0; i != 0x21; i++) {
                blitA(buf, base + Ledfd2[i], ((short *)&p->x)[1],
                      ((short *)&p->y)[1], Ledf90[i], Ledfb1[i]);
                if (frame > 3)
                    Func_80e3908(p, 0x40, 0x80 << 7);
                p++;
            }
        }
        if ((unsigned)(frame - 8) <= 0x2a) {
            k = frame - 8;
            if (k > 0x1f)
                k = 0x1f;
            *(vu16 *)0x5000002 = k << 10 | k << 5 | k;
        }
        if (frame == 0x33) {
            vu16 *pal;
            int mask;
            LoadVFXFile(FILE_7d, base, 1, 0);
            pal = (vu16 *)0x5000002;
            for (i = 1; i != 0x40; i++) {
                k = i / 2;
                if (k < 0)
                    k = 0;
                *pal = k << 10 | k / 2 << 5 | k;
                pal++;
            }
            REG_BLDCNT = 0x3f44;
            p = (Part *)(base + 0xe1 * 0x80);
            mask = 0x1f;
            for (i = 0; i != 0x20; i++) {
                p->x = (Random() & mask) + 0x20 << 16;
                p->y = (Random() & mask) + 0x50 << 16;
                p->vx = (0x1ff & Random()) - 0x100 << 12;
                p->vy = 0;
                p->t = 0;
                p++;
            }
            *(int *)(base + 0xef * 0x80) = 2;
            *(int *)(base + 0x7784) = 0x32;
        }
        if (frame > 0x34) {
            p = (Part *)(base + 0xe1 * 0x80);
            for (i = 0; i != 0x20; i++) {
                if (frame >= i / 4 + 0x34 && p->t <= 0x27) {
                    k = p->t / 4;
                    if (k > 5)
                        k = 5;
                    blitB(buf, base + (k << 11), ((short *)&p->x)[1] - 0x10,
                          ((short *)&p->y)[1] - 0x20, 0x20, 0x40);
                    Func_80e3908(p, 0x3c, 0xfffff000);
                    p->t++;
                }
                p++;
            }
        }
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x80);

    gfree(0x2f);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    Func_80d6888((*(State **)(base + 0x7828))->ids[0], -1, 1, -1, 0);
    iwram_3001ad0[2] = *(int *)(base + 0x77a0);
    iwram_3001ad0[3] = 0x20;
    _Func_80c0774(2, *(unsigned short *)(view + 0xc9 * 8), 0);
    WaitFrames(1);

    saved = REG_IME;
    SET_IO(REG_IME, REG_ADDR_IME);
    count = gDMATaskCount.count;
    if (count <= 0x1f) {
        task = (u32 *)(count * 12 + (u32)&gDMATaskCount + 4);
        *(u16 *)&gDMATaskCount = count + 1;
        *task++ = 0x7541;
        *task++ = 0x80 << 19;
        *task = 0x80 << 10;
    }
    SET_IO(REG_IME, saved);

    gfree(0x29);
    gfree(0x28);
    gfree(0x27);
}
