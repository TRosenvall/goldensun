/*
 * Func_8024934  --  asm/rom_15000/rom_23178_a_a_a_a_a_a.s  @ 0x08024934
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * does not invent one.  The batch's reconstruction effort went to CalcStats
 * (SIZE-exact and COUNT-exact; see PARK_CalcStats.c).
 *
 * Verify the measurements with:
 *   cd /Users/timothyrosenvall/gs_project/goldensun
 *   python3 tools/showfunc.py Func_8024934 > /tmp/ref.txt
 *   grep -cE '^\t[a-z]'           /tmp/ref.txt    # 946
 *   grep -nE '\t(sub|add)\tsp,'   /tmp/ref.txt    # frame grep 1
 *   grep -nE '\tmov\tr[0-9]+, sp' /tmp/ref.txt    # frame grep 2
 *   grep -nE '\tadd\tr[0-9]+, sp' /tmp/ref.txt    # frame grep 3
 *   python3 tools/shimcount.py src/non_matching/rom_15000/Func_8024934.c
 * shimcount reports 0 shims (this file has no code).
 *
 * NO PRIOR WORK ANYWHERE.  `grep -rl Func_8024934 src/` is empty.  No
 * landed file, no park, no fakematch row, no inherited figure.
 *
 * FAMILY: this is the THIRD member of the three-function family described in
 * full in src/non_matching/rom_15000/Func_8023e70.c -- read that section first,
 * because it is the reason to do these two together and not separately.  In
 * short: asm/rom_15000/rom_23178_a_a_a_a_a_a.s holds Func_8023178 (frame
 * 0x160, three spilled arguments), Func_8023e70 (frame 0x0e0, one) and this
 * one (frame 0x174, one), all three sharing an identical eight-instruction
 * prologue and an identical opening sequence through iwram_3001e8c,
 * AllocUploadSpriteGFX and CreateUIBox.  None of the three is attempted.
 *
 * ### CORRECTED IN BATCH 313 -- TWO CLAIMS ABOVE DO NOT HOLD.
 *
 * 1. THE SHARED PROLOGUE IS GENERIC AND BUYS NOTHING.  Measured tree-wide,
 *    that exact seven-instruction sequence opens 338 OF THE 871 remaining
 *    `thumb_func_start` functions in asm/ -- 39% of the tree -- and 93 of those
 *    338 sit in GENERATED .s files, i.e. landed C already reproduces it
 *    incidentally.  It is what gcc-2.96 emits for ANY Thumb function that uses
 *    r8-r11 and makes a call: a CONSEQUENCE of register pressure in the body,
 *    not a signature of a shared source file, and not something a
 *    reconstruction spells at all.  Func_80f6440, a different bank entirely,
 *    opens with the same seven instructions.  So "whichever is solved first
 *    supplies the opening ~40 instructions of the other two" is FALSE, and the
 *    "per-function cost is divided by three" scheduling argument built on it
 *    must not be relied on.
 *
 *    What IS real is narrower: the three functions in THIS .s do all spill
 *    their arguments and then load iwram_3001e8c and call
 *    AllocUploadSpriteGFX.  That is a genuine shared opening -- but it is the
 *    iwram_3001e8c/AllocUploadSpriteGFX sequence that is shared, NOT the
 *    prologue, and it does not extend to the wider rom_23178 family
 *    (Func_8026080 loads iwram_3001e74, a different global; Func_8027114 loads
 *    no global at all before its first call).
 *
 * 2. THERE IS NO SINGLE SPLIT SERVING ALL THREE.  `split_s.py` cuts out ONE
 *    NAMED TARGET, so the shape depends on which sibling you name:
 *        first member named  -> 2-way
 *        middle member named -> 3-way
 *        last member named   -> 2-way
 *    For Func_8023178 the dry-run is 2-WAY, not the three-way asserted here.
 *    So "do it ONCE" is wrong; each conversion is its own cut, and the cheapest
 *    order is an end member first.  This is the same positional asymmetry
 *    recorded for the BaseAnim_Attack / Anim_CriticalHit pair: DRY-RUN BOTH
 *    ORDERS rather than assuming symmetry.
 *
 * The genuinely transferable family evidence is SHARED DATA, not code shape:
 * Func_8023178 and Func_8026080 both reference .L373dc, .L373e0 and .L373e4,
 * and theirs are the only two files in the tree that do (all defined and
 * `.global`-ed in asm/rom_15000/rom_23178_c_c_c_c.s).  Shared read-only tables
 * are worth naming once; an identical prologue is worth nothing.
 * One three-way split serves all three, and whichever is solved first
 * supplies the opening forty instructions of the other two.
 *
 * SPLIT SHAPE: the same three-way split as its two siblings -- prove first
 * with `tools/tryc.py <cand.c> --ref
 * asm/rom_15000/rom_23178_a_a_a_a_a_a.s`, then split ONCE, and
 * ALWAYS `tools/split_s.py --dry-run` first, because split_s.py DELETES a
 * tracked .s and this one is the only copy of three unconverted functions.
 * No per-file Makefile rule mentions this stem; production -O2 flags apply.
 *
 * SIGNATURE: THE ARGUMENT SPILLED IS r2, NOT r0.  The first instruction
 * after the frame is cut is `str r2, [sp, #0x54]`, and r0 and r1 are never
 * read before being overwritten.  So the third parameter is the one the
 * function keeps, which means the signature has at least three parameters
 * even though only one is live past the prologue -- write it with three and
 * leave the first two unused, exactly as its sibling Func_8023178 (which
 * spills all three) proves the family shape to be.  A one-parameter
 * signature will put the wrong register in the wrong place at instruction 9.
 *
 * THE FRAME, FROM ALL THREE GREPS -- and this is the batch's clearest case
 * for why grep 1 alone is not enough.
 *   1. `sub sp, #imm`  -> `sub sp, #0x174`, matched by `add sp, #0x174`.
 *      372 bytes, the largest frame of the five targets.
 *   2. `mov rX, sp`    -> THREE hits: `mov r0, sp`, `mov r3, sp`,
 *      `mov r2, sp`.  Three aggregates based at sp+0.
 *   3. `add rX, sp`    -> ELEVEN hits, of which six carry a displacement:
 *        add r6, sp, #0x60      add r5, sp, #0x70
 *        add r2, sp, #0x60      add r3, sp, #0x5c
 *        add r5, sp, #0x5c      add r5, sp, #0x60
 *        add r2, sp, #0x174     <- NOTE THIS ONE
 *      plus four bare `add rX, sp`.
 * The scalars stop at sp+0x58 (see the slot map).  Everything from sp+0x5c
 * to sp+0x173 -- 280 bytes, THREE QUARTERS OF THE FRAME -- is aggregate that
 * grep 1 cannot see, and grep 2 and 3 together name three distinct bases at
 * sp+0x5c, sp+0x60 and sp+0x70.  Had only grep 1 been run this would have
 * been written up as a 93-scalar frame; docs/elevation.md records exactly
 * that error ("one function hid three aggregates at sp+0x54/0x38/0x30 that
 * way"), and this target is the same shape with the offsets moved.
 * `add r2, sp, #0x174` is worth singling out: 0x174 is the WHOLE frame
 * size, so that register points one past the end of the frame.  A pointer to
 * the end of a stack array is either a walk-down bound (the shape MenuBar
 * uses for its 0xff fill, with the base kept live past the copy) or a
 * one-past-the-end sentinel for an ascending walk.  Which of the two it is
 * decides whether the array is written forwards or backwards, and that is
 * the first thing to settle, because it fixes the loop direction for the
 * largest object in the frame.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST.  Sorted DESCENDING, with each
 * word classified by whether it is ever LOADED BACK:
 *      sp+0x58   0 stores, 1 load    *** SEE THE WARNING BELOW ***
 *      sp+0x54   1 store,  3 loads   SPILL -- and it is THE r2 ARGUMENT
 *      sp+0x50   1 store, 15 loads   SPILL
 *      sp+0x4c   1 store, 19 loads   SPILL  (most-read word in the frame)
 *      sp+0x48   1 store,  3 loads   SPILL  (the iwram_3001e8c value)
 *      sp+0x44   2 stores, 2 loads   SPILL
 *      sp+0x40   1 store,  1 load    SPILL
 *      sp+0x3c   1 store,  2 loads   SPILL
 *      sp+0x38   1 store,  4 loads   SPILL
 *      sp+0x34   3 stores, 2 loads   SPILL
 *      sp+0x30   3 stores, 7 loads   SPILL
 *      sp+0x2c   1 store,  1 load    SPILL
 *      sp+0x28   1 store,  1 load    SPILL
 *      sp+0x24   1 store,  2 loads   SPILL
 *      sp+0x20   2 stores, 7 loads   SPILL
 *      sp+0x1c   1 store,  4 loads   SPILL
 *      sp+0x18   1 store,  7 loads   SPILL
 *      sp+0x14   1 store,  3 loads   SPILL
 *      sp+0x10   7 stores, 2 loads   SPILL
 *      sp+0xc    1 store,  2 loads   SPILL
 *      sp+0x8    1 store,  1 load    SPILL
 *      sp+0x4   10 stores, 12 loads  SPILL
 *      sp+0x0   15 stores, 0 loads   ARGUMENT STAGING, not a spill
 *
 * *** THE sp+0x58 ROW IS THE ONE TO BE CAREFUL WITH, and it is a NEW
 * instance of a trap docs/elevation.md already names.  It is LOADED ONCE
 * AND NEVER STORED anywhere in the function.  A frame word that is read
 * before it is written is not a local at all: it is the SIXTH ARGUMENT of
 * an incoming call frame being read through the callee's own sp, or -- far
 * more likely given that the frame is 0x174 and sp+0x58 sits just below the
 * aggregates at sp+0x5c -- it is a read of the FIRST WORD OF THE sp+0x5c
 * AGGREGATE at a negative-looking displacement produced by one of the four
 * bare `add rX, sp` forms.  Either way, DO NOT DECLARE A SCALAR FOR IT.
 * Declaring one is how a phantom hole becomes a phantom variable, and with
 * 280 bytes of aggregate immediately above it the odds strongly favour the
 * aggregate reading.  Resolve it by reading the two instructions around the
 * single `ldr ..., [sp, #0x58]` before writing any declaration.
 *
 * sp+0x0 again carries the brief's third-grep lesson plainly: fifteen
 * stores, zero loads -- the stack-passed fifth argument of CreateUIBox and
 * friends, staged fifteen times.  No local belongs there.
 * The ACCESS-COUNT ordering is unusually informative on this target:
 * sp+0x4c (19 loads) and sp+0x50 (15) are read far more than anything else,
 * yet they sit high in the frame where slot order would put them late in the
 * declaration list.  Prefer the access count, as docs/elevation.md records
 * (access count beat slot order by 12 on a previous target): these two are
 * almost certainly the main loop's cursor and selection, declared early.
 * 158 sp-relative instructions in 946 -- one in six -- so like its sibling
 * this is a frame-bound function and the map above is the spine.
 *
 * REGISTER PRESSURE: the ORDINARY population.  84 high-register mentions
 * (37 x r10, 26 x r11, 12 x r8, 9 x r9).  Note the inversion against every
 * other target in this brief: here r10 and r11 carry three quarters of the
 * high-register traffic and r8 is comparatively light, whereas FieldMain
 * puts 52 of 102 in r8 and MenuBar 56 of 115 in r9.  Combined with 23 frame
 * words, that says the long-lived quantities here are NOT one dominant base
 * pointer but two mid-weight ones, and that most of the pressure went to the
 * stack rather than to the high registers.
 *
 * DISPATCH CENSUS, and the brief's switch material is INERT here too.
 * ZERO jump tables (`grep -cE '\t(mov|ldr|add)\tpc'` is 0).  Five unsigned
 * branches in 946 instructions -- the most of the four unreconstructed
 * targets, but still far too few for a decision tree over any useful number
 * of cases, so they are bounds and loop tests.  No `casesi`, nothing for
 * tools/screen_missing_case.py, no Case A or Case B site.  48 calls in 946
 * instructions, one per 19.7 -- between MenuBar's one-per-34.6 and
 * FieldMain's one-per-7.7.
 * Per the batch-level finding recorded in PARK_Func_8023e70.c: all four
 * unreconstructed targets in this brief contain ZERO jump tables between
 * them, so the switch apparatus the brief leads with does not apply to any
 * of them.  The one target in this brief that DID have jump tables is
 * CalcStats, which the brief flagged as the straight-line outlier -- it has
 * three of them (27, 8 and 6 entries), all tables and no trees, and all
 * three were reproduced.  That inversion is worth carrying forward: in this
 * batch the dispatch-heavy function was the one with the LOW high-register
 * count, not the menus.
 *
 * ORACLES.  Weak by address, as for its sibling: the nearest LANDED
 * functions are src/rom_15000/rom_23178_a_a_a_a_a_b.c (Func_8025180,
 * +0x84c), src/rom_15000/rom_23178_a_a_a_a_b.c (Func_80251d4, +0x8a0) and
 * src/rom_15000/rom_21dfc_c_b.c (Func_8022a38, -0x1efc).  The first two are
 * only ~2KB away, which is closer than anything available to Func_8023e70,
 * so of the two siblings THIS one has the better neighbourhood evidence
 * even though Func_8023e70 has the simpler signature.
 * For house style use src/rom_15000/rom_21dfc_a_c_c_b.c (Func_8021e48):
 * `unsigned int` parameters named arg0/arg1/arg2, locals named for the
 * register they land in, K&R braces.  rom_15000 carries most of the tree's
 * 1096 `goto`s, so gotos at dispatch joins are in-style.
 *
 * PREDICTED BLOCKER: the aggregate layout, before reload gets a say.  Three
 * aggregates at sp+0x5c, sp+0x60 and sp+0x70 inside 280 bytes means at
 * least one of them is small and adjacent to another, and AGGREGATE ORDER IS
 * REVERSED relative to declaration order -- so the declaration sequence is
 * the sp+0x70 object first, then sp+0x60, then sp+0x5c.  Getting that order
 * wrong moves every displacement in 158 sp-relative instructions at once,
 * which is the one defect class on this target that no amount of
 * register-level work can compensate for.  Settle the layout and the
 * sp+0x58 question from the disassembly BEFORE writing a declaration, then
 * expect the remaining work to be of the same kind that paid on CalcStats:
 * changing WHICH QUANTITIES EXIST rather than where they are scoped, since
 * region-scoping cannot reach a pseudo the compiler invented.
 */
