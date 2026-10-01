/*
 * Func_8023e70  --  asm/rom_15000/rom_23178_a_a_a_a_a_a.s  @ 0x08023e70
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * does not invent one.  The batch's reconstruction effort went to CalcStats
 * (SIZE-exact and COUNT-exact; see PARK_CalcStats.c).
 *
 * Verify the measurements with:
 *   cd /Users/timothyrosenvall/gs_project/goldensun
 *   python3 tools/showfunc.py Func_8023e70 > /tmp/ref.txt
 *   grep -cE '^\t[a-z]'           /tmp/ref.txt    # 1201
 *   grep -nE '\t(sub|add)\tsp,'   /tmp/ref.txt    # frame grep 1
 *   grep -nE '\tmov\tr[0-9]+, sp' /tmp/ref.txt    # frame grep 2
 *   grep -nE '\tadd\tr[0-9]+, sp' /tmp/ref.txt    # frame grep 3
 *   python3 tools/shimcount.py src/non_matching/rom_15000/Func_8023e70.c
 * shimcount reports 0 shims (this file has no code).
 *
 * NO PRIOR WORK ANYWHERE.  `grep -rl Func_8023e70 src/` is empty: no landed
 * file, no park, no fakematch row, no inherited figure.  The only mentions
 * in the tree are two `bl` sites inside its own .s.  The largest of the five
 * targets in this brief at 1201 instructions.
 *
 * ===========================================================================
 * THE MOST USEFUL FINDING IN THIS TRIAGE: THIS IS A THREE-MEMBER FAMILY,
 * AND ALL THREE MEMBERS ARE UNATTEMPTED.
 * asm/rom_15000/rom_23178_a_a_a_a_a_a.s holds exactly THREE functions, and
 * they are not merely co-resident -- they are one construction instantiated
 * three times:
 *
 *   Func_8023178  @ 0x08023178   frame 0x160   args spilled r0->sp+0x5c,
 *                                              r1->sp+0x58, r2->sp+0x54
 *   Func_8023e70  @ 0x08023e70   frame 0x0e0   arg  spilled r0->sp+0x4c
 *   Func_8024934  @ 0x08024934   frame 0x174   arg  spilled r2->sp+0x54
 *
 * All three open with the IDENTICAL eight-instruction prologue (push
 * {r5,r6,r7,lr}; mov r7,r11; mov r6,r10; mov r5,r9; push {r5,r6,r7};
 * mov r7,r8; push {r7}; sub sp,#imm), then all three immediately spill their
 * arguments, then all three do
 *       ldr r5, =iwram_3001e8c
 *       ldr rX, [r5]            and spill it
 *       mov r0, #0x80 ... bl AllocUploadSpriteGFX
 * and then open windows with CreateUIBox using the stack-passed fifth
 * argument (`str r3, [sp]` before each call -- which is why sp+0x0 is
 * store-only in all three; see the slot map below).
 *
 * That matters for scheduling the work, not just for colour.  Whichever of
 * the three is solved first supplies the spelling for the opening ~40
 * instructions of the other two for free, and the project already has
 * tools for exactly this situation -- tools/twin_finder.py,
 * tools/family_siblings.py, tools/twin_families.py, tools/shapesib.py.
 * RUN THEM BEFORE WRITING ANY C: a family of three where none is attempted
 * is the one configuration in which the per-function cost is divided by
 * three, and this brief's census fix is what made all three visible at once.
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
 * Func_8023e70 is the member to try FIRST despite being the largest,
 * because it is the only one of the three with a SINGLE argument (one
 * spill, one signature to get right) while Func_8023178 has three.
 * ===========================================================================
 *
 * SPLIT SHAPE: A THREE-WAY SPLIT IS NEEDED and it is the same split for all
 * three targets, so do it ONCE.  Prove first with
 *     tools/tryc.py <cand.c> --ref asm/rom_15000/rom_23178_a_a_a_a_a_a.s
 * then tools/split_s.py, ALWAYS --dry-run first -- split_s.py DELETES a
 * tracked .s, and this one .s is the only copy of three unconverted
 * functions totalling well over 3,000 instructions, so a careless run here
 * loses more than any other split in this bank.  No per-file Makefile rule
 * mentions this stem; production -O2 flags apply.
 *
 * THE FRAME, FROM ALL THREE GREPS.
 *   1. `sub sp, #imm`  -> `sub sp, #0xe0`, matched by `add sp, #0xe0`.
 *      224 bytes.
 *   2. `mov rX, sp`    -> TWO hits, `mov r2, sp` and `mov r3, sp`, five
 *      instructions apart.  Two separate aggregates based at sp+0.
 *   3. `add rX, sp`    -> THREE hits: `add r3, sp, #0x50` TWICE and
 *      `add r3, sp, #0x20` once.
 * Putting them together: the scalars occupy sp+0x00..sp+0x50 and everything
 * from sp+0x54 to sp+0xdc -- 140 bytes, well over half the frame -- is
 * aggregate that grep 1 cannot see at all.  sp+0x50 is taken as an ADDRESS
 * twice AND appears in the slot map below with 1 store and 6 loads, so it is
 * the BASE of an aggregate whose members are being read, not a scalar that
 * happens to be spilled; do not declare it as an int.  sp+0x20 is the same
 * story with 9 stores and 12 loads.  AGGREGATE ORDER IS REVERSED relative
 * to scalar order, so in the source the sp+0x20 object is declared AFTER the
 * sp+0x50 one.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST.  Sorted DESCENDING, with each
 * word classified by whether it is ever LOADED BACK:
 *      sp+0x50   1 store,  6 loads   aggregate base (grep 3), not a scalar
 *      sp+0x4c   1 store,  6 loads   SPILL -- and it is THE ARGUMENT
 *      sp+0x48   1 store,  5 loads   SPILL  (the iwram_3001e8c value)
 *      sp+0x44   5 stores, 13 loads  SPILL
 *      sp+0x40   2 stores,  3 loads  SPILL
 *      sp+0x3c   1 store,   2 loads  SPILL
 *      sp+0x38   1 store,   6 loads  SPILL
 *      sp+0x34   3 stores, 18 loads  SPILL  (most-read word in the frame)
 *      sp+0x30   2 stores,  2 loads  SPILL
 *      sp+0x2c   3 stores,  5 loads  SPILL
 *      sp+0x28   3 stores,  1 load   SPILL
 *      sp+0x24   3 stores,  6 loads  SPILL
 *      sp+0x20   9 stores, 12 loads  aggregate base (grep 3)
 *      sp+0x1c   5 stores,  2 loads  SPILL
 *      sp+0x18   4 stores,  3 loads  SPILL
 *      sp+0x14   1 store,   1 load   SPILL
 *      sp+0x10   1 store,   1 load   SPILL
 *      sp+0xc    1 store,   7 loads  SPILL
 *      sp+0x8    1 store,   3 loads  SPILL
 *      sp+0x4    1 store,   5 loads  SPILL
 *      sp+0x0    21 stores, 0 loads  ARGUMENT STAGING, not a spill
 * sp+0x0 is the clearest reading of the brief's third grep in this whole
 * batch: twenty-one stores and not one load back.  That is the fifth
 * argument of a five-argument call, staged twenty-one times -- CreateUIBox
 * and its relatives -- and declaring a local for it would be a phantom.
 * 177 sp-relative instructions in 1201, so one instruction in seven touches
 * the frame; this function is frame-bound and the slot map really is the
 * spine of the reconstruction.
 * Where slot order and access count disagree, PREFER ACCESS COUNT: sp+0x34
 * with 18 loads and sp+0x44 with 13 are the two hottest quantities and are
 * almost certainly the loop variables of the main interaction loop, not the
 * eighth and fifth declarations.
 *
 * REGISTER PRESSURE: the ORDINARY population.  96 high-register mentions
 * (29 x r10, 26 x r9, 24 x r11, 17 x r8) -- the most EVENLY SPREAD of the
 * five targets, which is itself diagnostic: no single dominant base pointer
 * (contrast MenuBar, where r9 alone carries 56 of 115), but four
 * comparably-used long-lived quantities.  With 21 spill slots as well, this
 * is a function where reload has run out of registers rather than one where
 * one pointer is held across everything.
 *
 * DISPATCH CENSUS, and the brief's switch material is INERT here as well.
 * ZERO jump tables (`grep -cE '\t(mov|ldr|add)\tpc'` is 0).  Only THREE
 * unsigned branches in 1201 instructions, which is far too few to be a
 * decision tree -- a tree over N cases needs roughly N-1 comparisons -- so
 * they are ordinary unsigned loop or bounds tests.  There is no `casesi`
 * here, nothing for tools/screen_missing_case.py, and no Case A or Case B
 * site.  Taken with FieldMain (0 tables), MenuBar (0 tables, 0 unsigned
 * branches) and Func_8024934 (0 tables), THAT IS THE BATCH-LEVEL FINDING:
 * all four of this brief's unreconstructed menu/field targets contain ZERO
 * jump tables between them, so the switch apparatus the brief leads with --
 * case_values_threshold, expand_end_case, group_case_nodes, the Case B
 * `case -1:` trick -- has no purchase on any of them.  The brief's own
 * tree-wide screen already concluded BufferString was the only Case B site;
 * this batch extends that to "and these four have no switch sites at all".
 *
 * ORACLES.  Weak, and that is worth knowing in advance.  The nearest LANDED
 * functions by address are
 *   src/rom_15000/rom_23178_a_a_a_a_a_b.c  Func_8025180  at +0x1310
 *   src/rom_15000/rom_23178_a_a_a_a_b.c    Func_80251d4  at +0x1364
 *   src/rom_15000/rom_21dfc_c_b.c          Func_8022a38  at -0x1438
 * -- all roughly 5KB away, because this target sits in the middle of a long
 * unconverted run.  The two FAMILY MEMBERS above are far better evidence
 * than any of these, and after them the next best is
 * src/rom_15000/rom_21dfc_a_c_c_b.c (Func_8021e48), which settles rom_15000
 * house style: `unsigned int` parameters named arg0/arg1/arg2, locals named
 * for the register they land in, K&R braces.  rom_15000 also carries most of
 * the tree's 1096 `goto`s, so gotos for dispatch joins are in-style here.
 *
 * PREDICTED BLOCKER: reload, through the SET of spilled quantities.  With
 * 21 slots and 140 bytes of aggregate, the first thing that has to be right
 * is how many objects exist and which of them are arrays rather than
 * scalars -- and the brief's own warning applies with full force here:
 * region-scoping cannot reach a pseudo the compiler invented, and with this
 * many invented pseudos the levers that will pay are the ones that change
 * WHICH QUANTITIES EXIST.  On CalcStats every single win was of that kind
 * (gcse-visible expression removed, dividend given a second set, bit test
 * split in two, AND kept wide by an extra variable, counter given its own
 * name) and not one came from scoping.  Budget accordingly: the frame map
 * above is the deliverable that makes that work possible, and it is the
 * part that was missing before this batch.
 */
