/* OvlFunc_955_2008160 -- asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a_a.s
 *
 * VERDICT: NOT ELEVATED.  BLOCKED at 17 differing of 107.  objcmp, run against
 * the ORIGINAL asm/ path, prints:
 *
 *     XX ENCODINGS differ in 17 place(s) (ref 107, ours 107)
 *        first at index 7: ref 6d06  ours 6d07
 *
 * No RELOCATIONS line and a present "first at index" line, so neither of the
 * two recorded objcmp false-negative shapes applies: the 17 is real.  Byte
 * length is identical (248 = 248) and the instruction stream is 1:1 with the
 * ROM -- every mnemonic, every constant, every branch, in order.  What is left
 * is TWO independent residues, both in register/insn PLACEMENT, and both
 * traced to a named gcc-2.96 pass with the compiler's own source and dumps.
 *
 * `tools/solved_twins.py`: "REMAINING FUNCTIONS WITH A SOLVED TWIN: 0 across 0
 * templates".  A twin miss is not a family miss -- the family was found by
 * grepping an IDIOM (below) and it produced the lever that closed site 2.
 *
 * ---------------------------------------------------------------- RESIDUE A
 * THIRTEEN LINES: the pointer read from actor+0x50 wants r6 and gets r7; the
 * loop accumulator wants r7 and gets r6.  Everything downstream rotates.
 *
 *     rom ldr r6, [r0, #0x50]        ours ldr r7, [r0, #0x50]
 *     rom mov r7, #0x0               ours mov r6, #0x0
 *     rom ldrh r3, [r6, #0x1e] ...   ours ldrh r3, [r7, #0x1e] ...   (x2 loops)
 *     rom strh r5, [r6, #0x1e]       ours strh r5, [r7, #0x1e]
 *
 * MECHANISM, read off gcc's own -da dumps rather than guessed.  global.c's
 * `allocno_compare` orders allocation by
 * floor_log2(n_refs) * n_refs / live_length, and flow.c:4948 weights each
 * reference by `loop_depth + 1`.  From the .17.lreg dump of this candidate:
 *
 *     reg 33  env    10 refs / 51 insns  ->  3*10/51 = 0.588
 *     reg 72  giv1    7 refs / 22 insns  ->  2* 7/22 = 0.636
 *     reg 76  giv2    7 refs / 22 insns  ->            0.636
 *     reg 34  i      14 refs / 40 insns  ->            1.050   (takes r5)
 *
 * so the two loop accumulators are allocated BEFORE env and take r6.  env's
 * ten references are 1 (the `ldr`) + 4 per loop (`ldrh` and `strh`, each
 * doubled by loop depth) + 1 (the closing `strh`).  The ROM needs env to win,
 * which takes EITHER one more depth-0 reference (n_refs 11 keeps
 * floor_log2 = 3, giving 33/51 = 0.647 > 0.636) OR four fewer RTL insns inside
 * its live range (30/47 = 0.638 > 0.636).  Neither is spendable: the ROM's own
 * instruction stream is what fixes both numbers, and every extra reference or
 * deleted insn is an extra or missing instruction.
 *
 * The priority reading is CONFIRMED, not inferred: deleting the two head stores
 * `*(int *)(actor + 0x34) = 0x1999; *(int *)(actor + 0x30) = 0x13333;` -- two
 * insns that sit inside env's live range and change nothing else about the
 * allocno set -- flips the assignment to the ROM's `ldr r6, [r0, #0x50]`.  A
 * cut-down probe of just the pointer, the two loops and the closing store
 * reproduces the ROM's r6/r7/r8 assignment exactly; adding those two stores
 * back is what loses it.
 *
 * ---------------------------------------------------------------- RESIDUE B
 * FOUR LINES: at the FIRST `__Actor_TravelTo` the ROM puts the r0 copy INSIDE
 * the split builds of the other three arguments; we put it last.
 *
 *     rom  mov r1,#0xa8 / mov r2,#0xa0 / mov r3,#0x84 / mov r0,r8
 *          / lsl r1,#0x11 / lsl r2,#0xc / lsl r3,#0x11 / bl
 *     ours mov r1,#0xa8 / mov r2,#0xa0 / mov r3,#0x84
 *          / lsl r1,#0x11 / lsl r2,#0xc / lsl r3,#0x11 / mov r0,r8 / bl
 *
 * This is the recorded argument-interleave class, in the half the notebook
 * already calls out of reach: docs/elevation.md "Argument-setup order: the zero
 * interleaved into a shifted build" (the single-instruction argument need not
 * be a zero -- "Widening the interleave detector: r0 need not be zero") and
 * tools/guarded_interleave.py's own docstring, "the constant is an ARGUMENT
 * TEMPORARY at a STRAIGHT-LINE site ... Nothing reaches these."
 *
 * MECHANISM, confirmed from calls.c in the build image and from
 * `-fsched-verbose=6`.  `precompute_register_parameters` (calls.c) copies any
 * argument with `rtx_cost(value, SET) > 2` into a pseudo BEFORE any hard
 * register is loaded; arm.c's `arm_rtx_costs` gives a thumb CONST_INT with
 * outer SET a cost of 0 when it is under 256 and COSTS_N_INSNS(2) = 6 when it
 * is `thumb_shiftable_const`.  All three of 0xa8<<17, 0xa0<<12 and 0x84<<17 are
 * shiftable, so all three are precomputed and their builds are emitted ahead of
 * the r0 copy that `load_register_parameters` emits afterwards.  The scheduler
 * cannot repair it: the sched dump gives the three `mov #imm` priority 34 (they
 * feed an `lsl`) and gives BOTH the three `lsl` and `mov r0, r8` priority 33,
 * and `rank_for_schedule`'s last tiebreak is `INSN_LUID`, i.e. the original
 * order -- in which the r0 copy is last.  So the ONLY fix is emission order,
 * and that needs the constants to still be pseudos at `precompute` time.
 *
 * THAT IS EXACTLY THE DOMINATING-BLOCK NAMING LEVER, AND IT WORKS -- AT THE
 * SECOND SITE.  See the levers section: site 2 is dominated by loop 1's `bge`
 * and its three constants, named, are rematerialised at the use, drop out of
 * `precompute` (a rematerialised pseudo has low rtx_cost -- the notebook
 * already states this under the `OvlFunc_898_20084a0` note) and the ROM's order
 * falls out.  Site 1 sits in the entry block.  There is no branch anywhere
 * before it, only three calls, so naming there does not rematerialise: gcc
 * allocates r9/r10/r11 and the function grows three push/pop pairs -- measured
 * 113 instructions against the ROM's 107.  That is the notebook's own
 * measurement on OvlFunc_967_2008308 and OvlFunc_911_20082b4 reproduced.
 *
 * NOTE FOR THE NOTEBOOK, a correction rather than a finding: the argument-order
 * table's remark "Note also what the table does NOT contain: r0 in the middle
 * ... nothing moves it to the middle of a three-argument call" is superseded --
 * by the later `OvlFunc_898_20084a0` note and now by this function's SITE 2,
 * which lands `mov r0, r8` in the middle of a FOUR-argument call.
 *
 * ------------------------------------------------------------------- LEVERS
 * All of these are in the shipped spelling and each was re-measured, not
 * inherited.  The named template src/overlays/rom_7b4558/ovl_30_a_a_c_c_c_b.c
 * is marked `// fakematch` and reaches its shape with `register __asm__("rN")`
 * pins and empty volatile asms.  NONE of that was needed here and none of it is
 * present -- its levers were treated as sufficient, not necessary, and every
 * one was re-measured on this function.  No pins, no barriers, no scaffolding.
 *
 * 1. THE GUARDED-INTERLEAVE NAMING, AND WHERE TO PUT IT.  `x2/y2/z2` named for
 *    the second `__Actor_TravelTo` takes 21 differing to 17 and reproduces the
 *    ROM's second site byte for byte, including the pool load sitting between
 *    two `lsl`s:
 *        mov r1,#0xa5 / mov r3,#0x84 / mov r0,r8 / lsl r1 / ldr r2 / lsl r3
 *    The dominating branch is a LOOP BACK-EDGE, not an `if` guard; the recorded
 *    lever asks only for a conditional branch and a loop supplies one.
 *    PLACEMENT IS LOAD-BEARING AND IS THE REFINEMENT WORTH KEEPING (grepped
 *    first under "dominating block", "commoned constant", "constant-CSE"):
 *    assigning the three at the TOP of the function still dominates site 2 and
 *    still rematerialises, but `0x84 << 17` is then live at site 1 as well and
 *    cse commons the two into r5 -- `mov r5,#0x84 / lsl r5,#0x11` once and
 *    `mov r3, r5` at site 1 -- for 87 differing.  Assigning them immediately
 *    AFTER the first site's call, still ahead of loop 1, dominates site 2 and
 *    leaves nothing to common: 17.  Naming only two of the three (leaving the
 *    third a literal) does not fire at all -- 23, worse than the literals --
 *    because `precompute` is per-argument and one precomputed build is enough
 *    to put a build ahead of the r0 copy.
 *
 * 2. THE STACK-ARG PAIR IN ITS SHARED FORM, ONE PAIR PER SITE.  Both
 *    `__Func_8010704` calls pass a value that is BOTH the second argument and
 *    the `[sp,#4]` slot, exactly as the .ld neighbour
 *    src/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a_b.c does.  Per "Each
 *    stack-argument SITE needs its own pair of locals" the two sites get two
 *    fresh pairs, in their own blocks, assigned adjacent to their call.
 *
 * 3. THE SHIFTED-CONSTANT SPELLINGS ARE THE ROM'S OWN ARITHMETIC.
 *    `0xcc << 2`, `0xa8 << 17`, `0xa0 << 12`, `0x84 << 17`, `0xb4 << 1` are
 *    written as shifts and gcc emits the ROM's `mov`+`lsl` pairs.  0xfff00000
 *    and 0xfff80000 are left as plain literals and gcc pools them, which is
 *    what the ROM does.
 *
 * 4. THE LOOPS ARE PLAIN COUNTED `for`s AND THE ACCUMULATOR IS A GIV.  Writing
 *    `-= i * 0x24` lets loop.c build the ROM's ascending `add r7,#0x24`
 *    accumulator alongside the descending `sub r5,#1 / cmp r5,#0 / bge`
 *    counter.  Spelling the accumulator out as a source variable is THREE
 *    INSTRUCTIONS LONG (110 against 107) whichever way it is written.  The
 *    second loop restarting at `0xb4 << 1` rather than carrying the first
 *    loop's final value is the ROM's own tell: it REBUILDS 360 where a carried
 *    variable would already hold it.
 *
 * 5. STORE ORDER IS THE ROM'S.  `actor+0x28` before `actor+0x24` -- descending,
 *    and not an oversight: the ascending order is 19 differing against 17.
 *
 * 6. THE ZERO NEEDS NO NAME.  `mov r5, #0` lands before `bl __PlaySound` and is
 *    reused by the closing `strh` and the two `str`s from plain literals; gcc
 *    commons it into the pushed r5 by itself.  A named `zero` local is exactly
 *    inert (17 either way), so per "screen the unnamed spelling first" the name
 *    is scaffolding and is not here.
 *
 * ------------------------------------------------- MEASURED WORSE (of 107)
 *   spelling / flag                                            differing  insns
 *   ----------------------------------------------------------  -------  -----
 *   SHIPPED (this file)                                              17    107
 *   literals at both TravelTo sites (no naming at all) .......       21    107
 *   x2/y2/z2 named at the TOP of the function ................       87    107
 *   only x2/y2 named, z2 left a literal ......................       23    107
 *   only x2 named ............................................       23    107
 *   all six constants named at the top (both sites) ..........      108    113
 *   `actor+0x24` stored before `actor+0x28` ..................       19    107
 *   named `zero` local for the four zero stores ..............       17    107  (inert)
 *   named pointer for the head stores / for the tail stores ..       17    107  (inert)
 *   `p = actor;` pinning the first argument, 1 or 2 locals ....       17    107  (inert)
 *   `actor` as `unsigned int` / `env` as `unsigned int` .......       17    107  (inert)
 *   `env` as `unsigned short *`, `env[15]` ...................       21    107  (inert on c1)
 *   `env` declared before `actor` ............................       21    107  (inert on c1)
 *   explicit accumulator, one across both loops ..............       62    110
 *   explicit accumulator, one per loop .......................       62    110
 *   explicit accumulator + explicit down-counter .............       59    110
 *   `for (i = 9; i >= 0; i--)` with `(9 - i) * 0x24` .........       18    107
 *   separate counters i and j for the two loops ..............       65    108
 *   `unsigned int i` .........................................       25    107
 *   `env` loaded after `__SetFlag` ...........................       30    107
 *   closing `env` store moved above the three tail calls .....       21    107
 *   `env` re-derived from actor for the closing store ........       68    109
 *   every callee's return type `int`, one at a time (8 runs) ..   17 x6, 19, 25
 *   tools/protolever.py, all prototypes and each alone (9 runs)   17 x7, 19, 25
 *   -O1 ......................................................       89    110
 *   -fno-rerun-cse-after-loop ................................       50    110
 *   -fno-schedule-insns2 .....................................       39    107
 *   -fno-schedule-insns, -fno-regmove, -fno-caller-saves,
 *     -fno-peephole, -fno-gcse, -fno-expensive-optimizations,
 *     -fno-cse-follow-jumps, -fno-force-mem, -fno-thread-jumps,
 *     -fno-delayed-branch, -fno-function-cse, -fno-defer-pop,
 *     -fno-rerun-loop-opt, -fmove-all-movables, -freduce-all-givs,
 *     -fno-cse-skip-blocks, -fno-optimize-register-move,
 *     -fno-delete-null-pointer-checks, -fno-inline ...........  all leave
 *                                            `ldr r7, [r0, #0x50]` unchanged
 *
 * ------------------------------------------------------------------ LANDING
 * The .s holds EXACTLY ONE function: its only `.thumb_func_start` is
 * OvlFunc_955_2008160, and its 117 lines contain no `.section`, `.data`,
 * `.data1`, `.word`, `.byte`, `.hword`, `.space`, `.lcomm`, `.comm` or
 * `.global` line at all.  So this would be a WHOLE-FILE .c at
 * src/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a_a.c, the hand .s deleted, and
 * NO LINKER EDIT IS NEEDED.  Cited by content and matched on FULL PATH, the
 * one and only .ld line naming this object is
 *
 *     asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a_a.o(.text)
 *
 * in overlays/rom_7ddb88/overlay.ld, sitting in the `.text` group between
 * ovl_30_c_c_b.o(.text) and ovl_30_c_c_c_a_a_a_b.o(.text).  That .ld's `.data`
 * group (which begins at ovl_30_c_c_c_c_a_b.o(.data)) names NO object from this
 * stem, so there is nothing to add or narrow there either.  BEWARE THE
 * BASENAME: four other overlay.ld files -- rom_7fa4ec, rom_7e3e08 (twice, once
 * for .data), rom_7a37f0 -- carry a line for an object also called
 * `ovl_30_c_c_c_a_a_a_a.o`.  None of them is this object; only the full path
 * distinguishes them.
 *
 * FLAG GROUP: NONE.  This is plain GCC296_CFLAGS through the generic cross-dir
 * rule `asm/%.o: src/%.c`.  Grepping the Makefile for both `rom_7ddb88` and
 * `ovl_30_c_c_c_a_a_a_a` returns nothing, so no explicit or pattern rule bites
 * this unit and there is no flag group to add.  THE MATCH DOES NOT DEPEND ON A
 * FLAG -- the default flags are the best of everything measured above, and
 * `tools/tryc.py --quiet` confirms the shipped file screens the same under the
 * default group as it does under the objcmp run.  A green screen here would
 * therefore also build green.  (Screened from scratch_elev/, which does not
 * match any Makefile source path; since the answer is "no rule", that is not a
 * hazard here.)
 *
 * -------------------------------------------------------------- NEW / GREPPED
 * NEW (operational).  A search for a SOLVED example of an idiom does not have
 * to be a search of the ROM's hand asm.  `asm/**\/*.s` beside a `src/**\/*.c`
 * is gcc-2.96's OWN output for 3765 already-matched translation units, and it
 * is grep-able.  Scanning it for a low-register copy landing inside a split
 * `mov`+`lsl` build -- the residue-B shape -- returns 20 hits, which is how
 * src/overlays/rom_7f2f14/ovl_30_c_c_a_a_a_b.c and the same-overlay
 * src/overlays/rom_7ddb88/ovl_30_c_c_c_c_c_c_c_a.c were found, and they are
 * what proved the shape reachable before any lever was tried.  Checked first
 * under the notebook's own vocabulary -- "grep for an IDIOM, not a position",
 * "LOOK UP A SHAPE IN THE SOLVED CORPUS", "Match against the SOLVED corpus" --
 * all of which say to do this and none of which name the generated tree as the
 * thing to grep.  `solved_twins.py` returning 0 is not the end of the search.
 *
 * NEW (refinement to the residue-A class).  docs/elevation.md's "A recurring
 * residue class: the parameter pointer one register too low" says of that
 * rotation "It is not the priority formula ... The cause is in `find_reg`'s
 * conflict/preference pass."  For THIS rotation it demonstrably IS the priority
 * formula: two `str` instructions inside the long pointer's live range are the
 * whole difference between the ROM's assignment and ours, and the .17.lreg
 * numbers above account for the flip exactly.  The cheap test that separates
 * the two classes is one compile: delete a couple of instructions from inside
 * the losing allocno's live range and see whether the register moves.  If it
 * does, the class is arithmetic and the question is whether the ROM's own
 * instruction stream leaves any slack -- here it leaves none, four insns or one
 * reference short, which is why this is still a park and not a match.
 *
 * GREPPED AND ALREADY ON RECORD, claimed as nothing new: the
 * `precompute_register_parameters` / `rtx_cost > 2` mechanism (the notebook
 * cites calls.c:805 and the same threshold); "a rematerialised pseudo has low
 * rtx_cost and drops out of precompute_register_parameters"; the
 * dominating-block naming lever and its straight-line failure mode; the
 * stack-arg-pair lever and its one-pair-per-site refinement; `allocno_compare`
 * and the `floor_log2(n_refs) * n_refs / live_length` priority; REG_ALLOC_ORDER
 * r5, r6, r7, r8, r10, r9, r11; the prototype-withholding lever moving `mov r0`
 * only LATER.
 */
extern void *__MapActor_GetActor(int slot);
extern void __SetFlag(int flag);
extern void __PlaySound(int id);
extern void __Actor_TravelTo(void *actor, int x, int y, int z);
extern void __WaitFrames(int n);
extern void __Actor_WaitMovement(void *actor);
extern void __CutsceneWait(int n);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_2008160(void)
{
    unsigned char *actor;
    unsigned char *env;
    int i;
    int x2, y2, z2;

    actor = (unsigned char *)__MapActor_GetActor(0x1e);
    env = *(unsigned char **)(actor + 0x50);
    __SetFlag(0xcc << 2);
    *(int *)(actor + 0x34) = 0x1999;
    *(int *)(actor + 0x30) = 0x13333;
    __PlaySound(0xe3);
    __Actor_TravelTo(actor, 0xa8 << 17, 0xa0 << 12, 0x84 << 17);
    x2 = 0xa5 << 17;
    y2 = 0xfff00000;
    z2 = 0x84 << 17;
    for (i = 0; i < 10; i++) {
        *(unsigned short *)(env + 0x1e) -= i * 0x24;
        __WaitFrames(1);
    }
    __Actor_TravelTo(actor, x2, y2, z2);
    for (i = 10; i < 32; i++) {
        *(unsigned short *)(env + 0x1e) -= i * 0x24;
        __WaitFrames(1);
    }
    __Actor_WaitMovement(actor);
    __CutsceneWait(2);
    __PlaySound(0xf0);
    *(unsigned short *)(env + 0x1e) = 0;
    __MapActor_SetAnim(0x1e, 4);
    *(int *)(actor + 8) = 0xa8 << 17;
    *(int *)(actor + 0xc) = 0xfff80000;
    *(int *)(actor + 0x10) = 0x84 << 17;
    *(int *)(actor + 0x28) = 0;
    *(int *)(actor + 0x24) = 0;
    {
        int m = 0x14, n = 0x10;
        __Func_8010704(0x13, n, 1, 1, m, n);
    }
    {
        int m = 0x15, n = 0x50;
        __Func_8010704(0x14, n, 1, 1, m, n);
    }
}
