/*
 * MenuBar  --  asm/rom_15000/rom_21dfc_a_c_c_c_a.s  @ 0x08021e6c
 *
 * TRIAGE ONLY, NO CANDIDATE WRITTEN, and so NO objcmp figure.  This header
 * does not invent one.  The batch's reconstruction effort went to CalcStats
 * (SIZE-exact and COUNT-exact; see PARK_CalcStats.c); everything here is
 * measured off the disassembly.
 *
 * Verify the measurements with:
 *   cd /Users/timothyrosenvall/gs_project/goldensun
 *   python3 tools/showfunc.py MenuBar > /tmp/ref_MenuBar.txt
 *   grep -cE '^\t[a-z]'           /tmp/ref_MenuBar.txt    # 1037
 *   grep -nE '\t(sub|add)\tsp,'   /tmp/ref_MenuBar.txt    # frame grep 1
 *   grep -nE '\tmov\tr[0-9]+, sp' /tmp/ref_MenuBar.txt    # frame grep 2
 *   grep -nE '\tadd\tr[0-9]+, sp' /tmp/ref_MenuBar.txt    # frame grep 3
 *   python3 tools/shimcount.py src/non_matching/rom_15000/MenuBar.c
 * shimcount reports 0 shims (this file has no code).
 *
 * NO PRIOR WORK, and the one file that matches the name is not about it.
 * src/non_matching/rom_15000/rom_28e54.c is the YesNoMenu2 park; it matches
 * only because it declares `extern void AddMenuBarOption(int a);`.  A dozen
 * landed files in src/rom_15000/ call AddMenuBarOption too.  Nothing in the
 * tree carries a figure, a blocker or a spelling for MenuBar itself, so
 * there is no inherited number to beat and none to trust.
 * THE .s ANNOTATION IS PARTLY WRONG and should not be inherited either: it
 * proposes the name RunShopScreen and says "THE LARGEST FUNCTION IN THE
 * MODULE at 1157 lines", but 1157 is the LINE count of the annotated dump,
 * not the instruction count, which is 1037.  Its claim that the function
 * "prompts with .gcc2_compiled." and "closes with ... .gcc2_compiled." is
 * disassembler noise -- `.gcc2_compiled.` is a label gcc emits at the top of
 * every translation unit, so a `bl` resolved to it means the real callee was
 * not named, not that there is a call to that symbol.
 *
 * SPLIT SHAPE: A SPLIT IS NEEDED.  asm/rom_15000/rom_21dfc_a_c_c_c_a.s holds
 * TWO functions -- MenuBar at 0x08021e6c and Func_8022768 after it -- so a
 * candidate cannot be built in place.  Screen with
 *     tools/tryc.py <cand.c> --ref asm/rom_15000/rom_21dfc_a_c_c_c_a.s
 * (which is what --ref exists for: PROVE FIRST, SPLIT SECOND) and only then
 * run tools/split_s.py, ALWAYS with --dry-run first, since split_s.py
 * DELETES a tracked .s.  No per-file Makefile rule mentions this stem, so
 * production -O2 flags apply and no flag override is needed.
 *
 * THE FRAME, FROM ALL THREE GREPS.
 *   1. `sub sp, #imm`  -> `sub sp, #0x3c`, matched by `add sp, #0x3c`.
 *      Sixty bytes.
 *   2. `mov rX, sp`    -> ONE hit, `mov r2, sp` at instruction ~163.
 *   3. `add rX, sp`    -> TWO hits, `add r5, sp, #0x38` and
 *      `add r6, sp, #0x24`.
 * Greps 2 and 3 together say the top of the frame is NOT scalars: sp+0x38
 * and sp+0x24 are taken as ADDRESSES, and `mov r2, sp` takes sp+0 as one
 * too.  So of the fifteen words in this frame, at least three are aggregate
 * or out-parameter bases and the rest are spills.  Running grep 1 alone
 * would have declared fifteen scalars and gone looking for fifteen locals.
 *
 * THE SPILL-SLOT MAP IS THE DECLARATION LIST.  Sorted DESCENDING, with each
 * word classified by whether it is ever LOADED BACK:
 *      sp+0x34   1 store,  7 loads   SPILL -- and it is THE ARGUMENT.
 *      sp+0x30   1 store, 10 loads   SPILL  (most-read slot in the frame)
 *      sp+0x2c   2 stores, 1 load    SPILL
 *      sp+0x28   2 stores, 2 loads   SPILL
 *      sp+0x24   2 stores, 2 loads   SPILL *and* an aggregate base (grep 3)
 *      sp+0x20   1 store,  1 load    SPILL
 *      sp+0x1c   1 store,  1 load    SPILL
 *      sp+0x18   1 store,  6 loads   SPILL
 *      sp+0x14   1 store,  1 load    SPILL
 *      sp+0x10   1 store,  8 loads   SPILL
 *      sp+0xc    2 stores, 2 loads   SPILL
 *      sp+0x8    2 stores, 2 loads   SPILL
 *      sp+0x4    2 stores, 2 loads   SPILL
 *      sp+0x0    2 stores, 0 loads   ARGUMENT STAGING, not a spill
 * sp+0x38 does not appear in this table at all, which is the proof that it
 * is the aggregate grep 3 says it is and not a scalar.
 * Read the map as the brief instructs -- descending order dates the pass
 * that made each pseudo -- and the FIRST declaration is the parameter:
 * `str r0, [sp, #0x34]` is the second instruction after the frame is cut,
 * and nothing else is stored there.  So MenuBar HAS at least one argument,
 * the reference spills it immediately, and it is read back seven times.
 * That is the single most useful fact in this triage: a `void` signature
 * cannot match, and neither can one that keeps the parameter in a register.
 * The access-count ordering (0x30 with 10 loads, 0x10 with 8, 0x34 with 7,
 * 0x18 with 6) is the ordering to prefer over slot order where the two
 * disagree -- docs/elevation.md records access count beating slot order by
 * 12 on a previous target.
 *
 * REGISTER PRESSURE: the ORDINARY population, and the most lopsided of the
 * five.  115 high-register mentions -- 56 x r9, 38 x r8, 13 x r10,
 * 8 x r11 -- so all four are live and r9 alone carries half of it.  r9 is
 * identifiable: `bl Func_8004970 / mov r9, r0` makes it the scratch block
 * returned by the 0x1e0-byte allocation, and it is then the base of
 * essentially every access in the body.  It is the same shape as CalcStats'
 * r6, and the same rule applies: a pointer returned by a call and used
 * across later calls MUST be a source local, because a call clobbers memory
 * and cse can never recover it.
 * r12 (ip) IS ALSO USED, as `mov r12, r9` immediately after the allocation,
 * and it is the BOUND of the opening clear loop:
 *      mov r3, r9 / mov r2, #0xff / add r3, #0xff / mov r12, r9
 *   L: strb r2, [r3] / sub r3, #1 / cmp r3, r12 / bge L
 * That is a DOWNWARD byte fill of 0x100 bytes with 0xff, written as a
 * pointer walked down to a saved copy of the base -- the brief's "TWO
 * VARIABLES WHERE THE ROM HAS ONE" lever, with the base kept live past the
 * copy so coalescing cannot eat it.  A `for (i = 0; i < 0x100; i++)`
 * ascending fill will not produce it; a descending `do { *--p = 0xff; }
 * while (p >= base)` shape is what to try, and getting the constant 0xff
 * into r2 ONCE outside the loop is part of the same test.
 *
 * DISPATCH CENSUS, and the brief's switch material is INERT here too.
 * ZERO jump tables (`grep -cE '\t(mov|ldr|add)\tpc'` is 0) and -- uniquely
 * among the five targets -- ZERO unsigned branches of any kind: no `bcc`,
 * no `bcs`, no `bhi`, no `bls` in 1037 instructions.  So there is no
 * `casesi`, no decision tree, and no range node anywhere in this function.
 * Everything is signed comparisons, which per the brief's own
 * READ-THE-BRANCH-MNEMONIC rule means ordinary splits throughout and no
 * hidden case to screen for.  `tools/screen_missing_case.py` has nothing to
 * do on this target, and the Case A / Case B apparatus does not apply.
 * Only 30 calls in 1037 instructions, one per 34.6 -- the inverse of
 * FieldMain's one-per-7.7 -- so this is where the arithmetic is, and the
 * work will be in expressions and allocation, not in call order.
 *
 * ORACLES, ADJACENT AND LANDED:
 *   src/rom_15000/rom_21dfc_a_c_c_b.c  Func_8021e48  at -0x24
 *   src/rom_15000/rom_21dfc_a_c_b.c    Func_8021e28  at -0x44
 *   src/rom_15000/rom_21dfc_a_c_b.c    Func_8021e14  at -0x58
 * Func_8021e48 is the function IMMEDIATELY before MenuBar in the ROM and it
 * is landed, so it is the strongest available proof of what gcc-2.96 does
 * with this bank's idioms.  Note what it settles about house style in
 * rom_15000, which differs from rom_77000's: parameters are named
 * `arg0, arg1, arg2` and typed `unsigned int`, locals are named after the
 * register they land in (`unsigned int r5;`), and the brace style is
 * K&R-on-the-same-line.  rom_15000 also has 1096 `goto`s across the tree,
 * so a goto for the dispatch joins is in-style and not a last resort.
 *
 * WHERE TO START.  The 0x100-byte 0xff fill is the first 12 instructions
 * after the allocation and it exercises three separate levers at once
 * (the r12 base copy, the descending walk, the hoisted 0xff).  Get that and
 * the `str r0, [sp, #0x34]` argument spill, screen with `tryc.py --ref`, and
 * the prologue plus the first thirty instructions will tell you whether the
 * four high-register assignments are right before any of the remaining
 * thousand is written.
 *
 * PREDICTED BLOCKER: the fourteen-slot spill map itself.  Fourteen spilled
 * scalars in a 1037-instruction function with only 30 calls means reload is
 * making most of these decisions, and reload's choices follow from the exact
 * SET of pseudos -- which is the one thing the brief says region-scoping
 * cannot reach.  Expect to spend the effort on WHICH QUANTITIES EXIST
 * (CalcStats' levers 1, 2, 4, 5 and 6 were all of that kind and all of them
 * paid) rather than on where they are scoped.
 */
