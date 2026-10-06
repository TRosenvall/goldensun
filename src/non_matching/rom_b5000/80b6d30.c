/* Func_80b6d30 (AssignBattlePositions)  --  0x080b6d30
 *
 * STILL NON-MATCHING, **4 differing encodings of 119** (ref 119 / ours 119,
 * 118 real instructions on both sides, first differing index 23).  PIN-FREE,
 * SHIM-FREE, FLAG-FREE.  RE-DERIVED batch 326 brief F -- objcmp --func AND
 * --whole both read 4 of 119 with SIZE, INSTRUCTION COUNT and RELOCATIONS all
 * silent, so no pad is absorbing a length difference.  Batch 321 brief E
 * RE-MEASURED and CONFIRMED the figure, ran 37 crossed variants over the lever
 * class the park had never touched (declarations and signatures), and
 * independently DERIVED residue (1)'s impossibility from cse.c rather than
 * inferring it.  THE BODY BELOW IS UNCHANGED from the parked one.
 *
 * The only function in asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s and no data section
 * (datacheck.py prints nothing), so landing would be a plain whole-file
 * conversion with no export and no split.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80b6d30.c asm/rom_b5000/rom_b5a0c_c_c_c_a_c.s --func Func_80b6d30
 *
 * THE RESIDUE IS TWO PLACES, FOUR ENCODINGS, and both are confirmed:
 *
 *   (1) index 23:   rom `mov r4, sl`              ours `movs r4, #0`
 *   (2) indices 82-84:
 *       rom  `lsl r3, r5, #12 / orr r3, r7 / mov sl, r3`
 *       ours `lsl r2, r5, #12 / orr r2, r7 / mov sl, r2`
 *
 * ===== BATCH 321: RESIDUE (1) IS NOW PROVED, NOT ARGUED =====
 *
 * The park argued from COST and notreg_cost that the constant wins on ties.
 * That is correct as far as it goes but it is not the whole decision, and the
 * missing half makes the result STRONGER rather than weaker.  Read in gcc-2.96's
 * own cse.c, the full chain at `j = ret` is:
 *
 *   a. `ret = 0` records BOTH `(const_int 0)` and ret's pseudo in ONE
 *      equivalence class, and `insert` keeps a class sorted by CHEAPER with the
 *      cheapest FIRST.  Its own comment says it: "a constant is the only thing
 *      that can be cheaper than a register".  So the class head is the constant.
 *
 *   b. At `j = ret`, cse_insn walks that class and PRUNES every candidate that
 *      is already in the table -- `src = 0` for the register and then
 *      `src_folded = 0` for the constant ("Prefer items not in the hash table
 *      to ones that are when they are equal cost").  BOTH are pruned, so
 *      src_cost and src_folded_cost both stay at 10000 and NEITHER is what
 *      decides anything.
 *
 *   c. The substitution therefore comes from the hash-table entry, and `elt` was
 *      set to `elt->first_same_value` -- the class HEAD -- which by (a) is the
 *      constant, at `src_elt_cost == 0`.  The fold is unconditional.
 *
 * So the escape is not "make the register cheaper than the constant", it is
 * "keep the register off the head of its own equivalence class", and the only
 * thing in CHEAPER that can beat a `(const_int 0)` at cost 0 is CHEAP_REG, which
 * needs `REG_USERVAR_P && REGNO < FIRST_PSEUDO_REGISTER` -- a HARD-REGISTER USER
 * VARIABLE.  That is the pin the park already measured at 102 of 119.  **There
 * is no pin-free C source that reaches residue (1) inside one basic block**, and
 * the ROM's own layout puts `ret = 0` (indices 18/21) and `j = ret` (index 23)
 * in one block with only `bl Func_80c2384` between them -- and a call does not
 * invalidate a pseudo in cse's table, which the park verified from the other
 * direction by moving `j = ret` across it (still 4).
 *
 * The documented escape remains a CONTROL-FLOW BOUNDARY, as in the corpus
 * exemplar src/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_b.c whose `for (i = ret; ...)`
 * sits in an ELSE arm at .L3.  This function has no boundary to put there.
 *
 * ===== BATCH 321: THE DECLARATION LEVER CLASS IS MEASURED, AND IT IS FLAT =====
 *
 * docs/humanization.md section 3 says a pin is evidence about a DECLARATION.
 * This park's body is a textbook instance of that signature -- `*(short *)(s +
 * a)` raw-offset arithmetic four times, and `extern unsigned char
 * ewram_2018000[];` with no dimensions -- and NOTHING in the class had ever been
 * tried here.  37 crossed variants via tools/crossfire.py, depth 2, two edit
 * sets.  Reference memory profile ldr=7 ldrb=4 ldrsh=4 str=1 strh=2.
 *
 * EXACTLY INERT at 4 of 119, ref 119 / ours 119 (so these are distances, and
 * each is a candidate prerequisite that pays nothing on its own OR in any pair):
 *
 *   * `extern unsigned char ewram_2018000[][0x4000];` WITH the use rewritten as
 *     `(int)ewram_2018000[i]` -- pattern 4, the inner dimension included.  This
 *     is the dividend case the brief asks to be reported: BETTER-TYPED SOURCE,
 *     IDENTICAL BYTES.  (Either half ALONE is 5 of 119, i.e. one worse -- the
 *     declaration and the use have to move together.)
 *   * splitting `if (v == 0x1dc || v == 0x1e3) continue;` into two sequential
 *     `if`s -- pattern 1.
 *   * `i * 0x4000` for `i << 14`; dropping the `(int)` cast on the ewram
 *     argument; `_PreloadSpriteGFX`'s second parameter declared `void *`;
 *     `_GetUnit`'s prototype withheld; `ret` declared `unsigned int`; `v`
 *     declared `unsigned int`.  And every PAIR of the above.
 *
 * FAR WORSE, all with a COUNT flag (so the figure measures misalignment, not
 * distance) -- recorded so nobody repeats them:
 *   * `short *s` with the four accesses as `s[i+2]` / `s[i+3]`: 79-94 at 115-123
 *     instructions, with MEM divergence.  Pattern 2 does NOT apply here: the
 *     ROM's `ldrsh r3, [r6, r2]` register-offset form is what the `off`/`a`
 *     idiom produces, and narrowing the pointer type replaces it with scaled
 *     addressing.  The landed sibling Func_80b6cdc's idiom is correct as written.
 *   * hoisting the arg-4 call `Func_80c23a0(u[0x128])` into a temp: 101 at 123.
 *
 * READ THE FLATNESS AS THE FINDING (brief 321): 29 of 37 rows tie the base
 * exactly, across three different dimensions.  The lever is not in declarations,
 * types or callee signatures.  Given the derivation above, that is expected:
 * residue (1) is decided on COST inside cse1's equivalence class, which no type
 * written in C can move, and residue (2) is a reload-register INDEX.
 *
 * ===== BATCH 326 BRIEF F: RESIDUE (1)'s OPEN ROUTE IS REFUTED, AND ITS
 * ===== SHAPE IS REPRODUCED
 *
 * OBSERVED rather than derived: `.02.jump` insn 54 is
 * `(set (reg/v:SI 38) (reg/v:SI 37))` and `.03.cse` insn 54 is
 * `(set (reg/v:SI 38) (const_int 0))` with a REG_EQUAL note (reg 37 = `ret`,
 * reg 38 = `j`).  cse1 does fold it, exactly as the park derived.
 *
 * **BUT THE SAME FOLD ALSO DELETES `jump_insn 308`, THE LOOP'S ENTRY GUARD**
 * `(if_then_else (gt (reg 38) (const_int 1)) ...)`, which `stmt.c` emits ahead
 * of `NOTE_INSN_LOOP_BEG` for every `for`.  The ROM has NO entry guard: it goes
 * `mov r4, sl / mov r7, r0` straight into `.Lb6d68`.  Measured -- every edit
 * that blocks the fold puts the guard back and pays two encodings for it:
 *
 *   * `ret = _DEVICE_ZERO` (a volatile extern, INSTRUMENT): `cmp r1,#1 / bgt
 *     .L4` appears, **121 instructions**, figure 105.
 *   * `register int ret asm("r10")` (the park's pin): `cmp r7,#1 / bgt .L4`,
 *     **121 instructions**, figure 102.
 *
 * So the park's "residue (1) needs a control-flow boundary that cse1 SEES and a
 * later pass REMOVES" is necessary and **NOT SUFFICIENT**: a boundary that hides
 * `ret`'s value also stops the guard folding.  **Any solution must ALSO make the
 * outer loop bottom-tested in RTL**, and that cross is measured and it works:
 *
 *   * `j = ret; do { ... } while (++j <= 1);` is **EXACTLY INERT at 4 of 119,
 *     118 instructions, with a bit-identical `.18.greg` reload trace.**  The
 *     bottom-tested spelling is free -- a candidate prerequisite that pays
 *     nothing on its own, which is why 37 one-at-a-time variants in batch 321
 *     and six statement orders before that could not see it.
 *   * the same do-while WITH the volatile instrument emits `mov r7, sl` at the
 *     init, **no entry guard, and 118 instructions -- the ROM's exact count**
 *     (against 121 for `for` + the same instrument).  Its figure is 101 only
 *     because the volatile load swaps `j` and `v` between r7 and r4 and drops
 *     one reload from the trace.
 *
 * > **Residue (1)'s SHAPE is therefore reproduced** -- register copy out of sl,
 * > no guard, 118 instructions.  What remains is a DEVICE-FREE way to keep
 * > `ret`'s value from cse1 at the init of a bottom-tested loop.  State the open
 * > question that way; the boundary and the loop shape must arrive together.
 *
 * Device-free boundary attempts, all INERT at 4 (the label never survives to
 * cse1): `goto start; start: j = ret;` -- jump1 runs at pass 02 and deletes the
 * jump together with its now-unused label -- and braces on the `continue` arm.
 * `int ret = 0;` as a declaration initialiser with the do-while reads **22**.
 *
 * ===== A FAMILY NOBODY HAD CHECKED, NOW CLOSED, WITH ITS EVIDENCE =====
 *
 * `mov r4, sl` could have come from post-reload CSE instead of from source --
 * there are two passes that replace a non-register SET_SRC with a register that
 * already holds the value, and both run after reload.  Neither can fire here:
 *
 *   * `reload_cse_simplify_set` (**reload1.c:8046-8056**) substitutes when
 *     `this_cost < old_cost`, or on a tie because "If equal costs, prefer
 *     registers over anything else".
 *   * `reload_cse_simplify_operands` (**reload1.c:8219-8224**), optional
 *     reloading, gates a CONST_INT operand on
 *     `rtx_cost (operand, SET) > rtx_cost (reg, SET)`.
 *
 * and the Thumb costs are: `arm_rtx_costs` returns **0** for any `const_int`
 * below 256 with `outer == SET` (**arm.c:2078-2081**); `REGISTER_MOVE_COST` is
 * **4** when HI_REGS is involved, 2 otherwise (**arm.h:1280-1285**); and
 * `rtx_cost ((reg), SET)` is **1** = `! CHEAP_REG` (**cse.c:805**, CHEAP_REG at
 * **cse.c:505-507** -- `CHEAP_REGNO` is false for r10, which is neither
 * fp/sp/ap nor `FIXED_REGNO_P`).  `4 < 0` false, `4 == 0` false, `0 > 1` false.
 * **BOUND: on Thumb no post-reload pass can replace a `const_int` below 256
 * with a register.**  Also checked: `*thumb_movsi_insn`'s alternative 8 is
 * `"*lh"/"*lh"` (**arm.md:3839-3840**), so lo<-hi needs no reload in either
 * direction -- residues (1) and (2) are NOT linked through the reload count.
 *
 * ===== RESIDUE (2) RE-DIAGNOSED: find_reg ALREADY PICKS THE ROM'S REGISTER ====
 *
 * `.15.regmove:533` shows insn 200 is `(set (reg/v:SI 37) (ashift (reg 39) 12))`
 * and insn 202 `(set (reg 37) (ior (reg 37) (reg 36)))` -- both write `ret`,
 * which is in sl, so both need a lo-register reload, and insn 381
 * (`mov sl, r2`) is the reload copy.  `.18.greg` prints
 *
 *     Spilling for insn 200.
 *     Using reg 3 for reload 0
 *     Spilling for insn 202.
 *     Using reg 3 for reload 0
 *
 * -- **`find_reg` (reload1.c:1588, the only `Using reg` printf, :1664) already
 * chooses the ROM's r3.**  We emit r2.  So layer 1 agrees with the ROM and the
 * divergence is purely layer 3: `allocate_reload_reg`'s round-robin over
 * `spill_regs` starting at `last_spill_reg` (**reload1.c:4998-5013**; the cursor
 * is written only at **:4937**, inside `set_reload_reg`).
 *
 * Measured inputs.  `spill_regs` is built in ASCENDING hard-register order
 * (**reload1.c:3527-3532**) and here it is **{1,2,3}, n_spills 3**; the whole
 * cursor trace for the function is **3 2 3 2 1 3 2 3 3 2 3 2 3 3**.  At insn 200
 * the live pseudos are 32-39 in fp, r6, r9, r8, r7, sl, r4, r5, so the per-insn
 * grown set `chain->used_spill_regs` (**reload1.c:3621-3622**, read into
 * `reload_reg_unavailable` at **:5129**) is the full {1,2,3} and ALL THREE ARE
 * FREE -- batch 325's "`Using reg`, several free -> the cursor" triage, and the
 * cursor alone decides.
 *
 * **The hard-register pin flips the site to the ROM's `lsl r3, r5, #12`, and it
 * does it by putting r0 into the spill set** -- trace `3 2 3 2 0 3 1 3 3 3 3 3`,
 * n_spills 3 -> 4.  So residue (2)'s lever class is the **modulus or parity of
 * the cursor** (n_spills, or the count of non-inherited reload allocations
 * before insn 200), NOT the spelling of the statement.  That is why the park's
 * respellings were inert, and the park's old sentence "the register at index 82
 * is a function of the COUNT of reload-register allocations made EARLIER" was
 * right about the mechanism while its "`Using reg 3` selects which register to
 * SPILL, not which a reload gets" understated how close layer 1 already is.
 *
 * ===== MEASURED BATCH 326 (ref 119 / ours 119, 118 instructions unless noted) =
 *
 * EXACTLY INERT at 4 with a bit-identical reload trace -- candidate
 * prerequisites, not dead ends: the do-while above; `goto`+label before the
 * init; braces on the `continue` arm; `i >= 5` for `i > 4`; `if (flag)`;
 * `if (!flag)`; `a = i * 2 + 4` with `off`/`a` eliminated; `off = i << 1`;
 * `o = i << 1`; `v | (i << 12)`; `if (u[0x129])`; `u[0x129] > 0`;
 * `(int)ewram_2018000 + (i << 14)`; `i * 0x4000`;
 * `if (v != 0x1dc && v != 0x1e3) break;`; the tail test split into two `if`s;
 * `int a` before `int off`; `int i` before `int j`; `int j` before `int ret`;
 * `unsigned char *s`; `unsigned int flag`; `unsigned int v`; `void *` as
 * `_PreloadSpriteGFX`'s second parameter; `i = 0;` lifted out of the inner
 * `for`.
 *
 * WORSE: `j + v` for `v + j` **5**; `i != 6` as the inner bound **6**; `i > 5`
 * for `i == 6` **7**; `int ret = 0;` as a declaration initialiser **22**;
 * `ret = i << 12; ret |= v;` **46** at 120; the `j == 0` assignment moved below
 * the store block **47** at 120; `off` reused in place of `o` in the store
 * block **116** at 128.
 *
 * ===== THE INERT LIST, carried forward =====
 *
 * From earlier batches, all still 4 or far worse: `for (j = ret; ...)`, `j = ret`
 * before and after the call, `ret = j = 0`, `j = ret = 0`, `j = 0; ...; ret = j`;
 * commuted `v | (i << 12)`; `!j` for `j == 0`; a block-scoped temp for `i << 12`;
 * `ret = i << 12; ret |= v`; declaration order (`ret` last, `ret` first, `j`
 * before `ret`); `i` before `j`; `o` at function scope; the tail test as
 * `if (v != 0x1dc && v != 0x1e3) break;`; `(int)ewram_2018000 + (i << 14)`.
 * NOT inert and not useful: `a = off; a += 4;` -> 75; hoisting `u[0x128]` into a
 * local -> 59; moving `ret = 0` after the call -> 10; `+` for `|` -> 56.
 * Flags: `-fno-gcse`, `-fno-cse-follow-jumps`, `-fno-rerun-cse-after-loop`,
 * `-fno-strength-reduce` all leave it at 4, so NO flag group applies.
 * The cse-defeating barrier family is measured in full and all of it costs two
 * instructions, because `ret` is in a HI register: "+r" 96, "+h" 96, "+g" 98-103,
 * "+l" 100-104, at 121-123 encodings against 119.  "+h" not helping is the
 * important one -- gcc-2.96 copies through a low register either way.
 *
 * SHIMS -- NONE.  register class 0, .equ class 0, other __asm__ 0.
 *
 * NEXT, IF ANYONE TAKES IT UP, in priority order:
 *
 *   1. Residue (1): a DEVICE-FREE boundary that hides `ret`'s value from cse1,
 *      CROSSED WITH the bottom-tested loop (which is measured free).  The
 *      instrumented cross already gives the ROM's `mov <j>, sl` at the ROM's
 *      118 instructions, so the shape is right and only the boundary is missing.
 *      Do not test a boundary on the `for` body -- it will read two worse and
 *      look inert-or-harmful for the wrong reason.
 *   2. Residue (2): shift `allocate_reload_reg`'s cursor by one before insn 200,
 *      or change `n_spills`.  Nothing at the differing statement can do it; look
 *      for an edit that adds or removes ONE non-inherited reload allocation
 *      earlier in the function, or that brings r0 into `used_spill_regs`.
 *
 * Do NOT re-propose: 32+ respellings of the two differing statements, the
 * declaration/type/signature class (37 variants, 29 exactly flat), and the
 * post-reload-CSE family (closed above with citations).
 *

 * ===== BATCH 327 BRIEF H: RESIDUE (1) IS TWO FOLDS, NOT ONE, AND A PLAIN
 * ===== CODE_LABEL IS *NOT* A BOUNDARY
 *
 * Figure re-derived: **4 differing encodings of 119**, ref 119 / ours 119,
 * first at index 23, SIZE / INSTRUCTION COUNT / RELOCATIONS all silent.
 * BODY UNCHANGED.
 *
 * 1. THE FOLD IS UNCONDITIONAL FOR THE REASON THE PARK DERIVED, and the lines
 *    are: `CHEAPER(X,Y)` is literally `X->cost < Y->cost` (**cse.c:1479**);
 *    `COST` gives a pseudo 1 and a non-reg `rtx_cost (x, SET) * 2`
 *    (**cse.c:509-520**), and `arm_rtx_costs` returns 0 for a `const_int` < 256
 *    with `outer == SET` (**arm.c:2078-2081**), so `(const_int 0)` at cost 0
 *    takes the head of the class in `insert` (**cse.c:1546-1556**); cse_insn
 *    then walks from `elt->first_same_value` (**:5108**), prunes both `src` and
 *    `src_folded` as already-in-class (**:5109-5148**) and falls to
 *    `trial = copy_rtx (elt->exp)` (**:5247-5251**) -- the constant.
 *
 * 2. **THERE ARE TWO FOLDS, WITH DIFFERENT BOUNDARY SETS.**  `cse_end_of_basic_block`
 *    (**cse.c:6534-6743**) ends a block at ANY CODE_LABEL (**:6573**, no
 *    LABEL_NUSES test) and, when `! after_loop`, at NOTE_INSN_LOOP_END
 *    (**:6588-6591**).  cse1 is called with after_loop == 0 and **cse2 with
 *    after_loop == 1, so cse2 ignores LOOP_END.**  Measured:
 *      `do { v = Func_80c2384(u[0x128]); } while (0);` + the free do-while puts
 *      a NOTE_INSN_LOOP_END between `ret = 0` and `j = ret` AT ZERO INSTRUCTION
 *      COST, and `.02.jump` insn 69 `(set (reg 38) (reg 37))` **survives
 *      `.03.cse`, `.07.gcse` and `.08.loop` unchanged** -- cse1 does NOT fold --
 *      and `.09.cse2` turns it into `(const_int 0)` with a REG_EQUAL note.
 *      Figure 7 of 119 (119 encodings, first at index 20); the 3 extra are a
 *      scheduling perturbation of the `ldrb`/`mov sl,r1` pair and the init's
 *      placement.  Variants: the same wrapper on the `for` body 7; around
 *      `ret = 0` and the call together 7; around `ret = 0` alone 8.
 *
 * 3. **A CODE_LABEL IS NOT AUTOMATICALLY A BOUNDARY -- THIS IS WHY EVERY LABEL
 *    ATTEMPT IN THIS PARK'S HISTORY MEASURED INERT.**  Before reaching a label
 *    the block is EXTENDED ACROSS a forward conditional jump by two independent
 *    paths, both on at -O2: `follow_jumps` (**cse.c:6640-6684**) when
 *    `LABEL_NUSES == 1`, there are insns after the target and the target is
 *    preceded by a BARRIER; and `skip_blocks` (**:6686-6720**), "a branch around
 *    a block of code", when `q != CODE_LABEL` and no labels intervene.
 *    PROVED with an instrument: `if (u[0x129] != 0) return ret;` before the init
 *    (+5 instructions; semantically equivalent because the in-loop test already
 *    makes the body a no-op then and `ret` is 0) puts
 *    `barrier / code_label [1 uses] / insn 74 (set (reg 38) (reg 37))` in
 *    `.02.jump` -- and cse1 folds it anyway.  With `-fno-cse-follow-jumps`
 *    ALONE it STILL folds; only with **both** `-fno-cse-follow-jumps
 *    -fno-cse-skip-blocks` does `.03.cse` keep the copy.
 *    The park's "the label never survives to cse1" is therefore wrong in a way
 *    that matters: a single-use, barrier-preceded label DOES survive and is
 *    STEPPED OVER.
 *
 * 4. THE BOUND, STATED.  The boundary must be a label the block cannot be
 *    extended into -- reached by FALL-THROUGH plus a jump from outside the
 *    block (a true join), or `LABEL_NUSES >= 2`, or a loop top -- and it must
 *    still be a boundary at cse2, which ignores LOOP_END.  Every such label
 *    needs a compare and a branch, and the reference is exact at 118
 *    instructions with no conditional anywhere before `.Lb6d68`.
 *
 * 5. AND A CORRECTION TO "ONLY THE BOUNDARY IS MISSING".  The do-while +
 *    LOOP_END body with `-fno-rerun-cse-after-loop` (instrument) DOES keep the
 *    copy, and it comes out as `mov r4, r0 / mov r7, sl` -- **`j` in r7 and `v`
 *    in r4, the reverse of the ROM** -- 13 differing at 123 lines.  So blocking
 *    the fold lengthens `ret`'s live range and re-orders the allocnos: the
 *    boundary is NECESSARY AND NOT SUFFICIENT.  Treat "find a device-free
 *    boundary and it lands" as refuted; the allocation has to be re-won too.
 *
 * 6. THE POST-RELOAD BOUND NOW COVERS ALL THREE PASSES.  The park cited
 *    `reload_cse_simplify_set` and `reload_cse_simplify_operands`.  There is a
 *    THIRD: `reload_cse_move2add` (**reload1.c:8840**), called from
 *    `reload_cse_regs` (**:7991**) between two `reload_cse_regs_1` passes.  Its
 *    const path is gated on `GET_CODE (src) == CONST_INT && reg_base_reg[regno]
 *    < 0` and rewrites only against **the same hard register's own** recorded
 *    constant `reg_offset[regno]`, emitting `(set (reg) (reg))` -- a SELF-move
 *    -- when the delta is 0 (**:8899-8903**).  It can never take the value from
 *    a DIFFERENT register, so it cannot make `movs r4, #0` into `mov r4, sl`.
 *    **The bound stands, now checked across all three post-reload passes.**
 *
 * NEXT, REVISED:
 *   1. Residue (1) needs a free join-point label AND a way to keep `j` in r4
 *      once `ret` stays live across the init.  Do not test a boundary without
 *      checking the j/v allocation in `.19.flow2`.
 *   2. Residue (2) unchanged -- the `allocate_reload_reg` cursor's parity.
 * Do NOT re-propose: `goto`+label (stepped over by follow_jumps), do-while(0)
 * wrappers in any of the four positions measured above, or the post-reload CSE
 * family.
 
 * ===== BATCH 329 BRIEF D: A ZERO-BYTE cse1 BOUNDARY EXISTS, AND THE SECOND
 * ===== FOLD IS `.07.gcse`, NOT `.09.cse2`
 *
 * Figure re-derived: **4 differing encodings of 119**, ref 119 / ours 119, first
 * at index 23, SIZE / INSTRUCTION COUNT / RELOCATIONS all silent on BOTH --func
 * and --whole.  BODY UNCHANGED.  The bottom-tested do-while control was
 * re-measured and is still EXACTLY inert at 4, confirming it is free.
 *
 * 1. THE PARK'S BOUNDARY BOUND IS REFUTED.  Batch 327 point 4 ended on *"Every
 *    such label needs a compare and a branch, and the reference is exact at 118
 *    instructions with no conditional anywhere before .Lb6d68."*  Measured
 *    false.  A ONE-TRIP BOTTOM-TESTED LOOP AROUND THE INIT ALONE is a real
 *    CODE_LABEL boundary and costs ZERO BYTES:
 *        do { j = ret; } while (j > 1);    4 -> 5 of 119, dsize 0, reloc ok
 *        do { j = ret; } while (j != 0);   4 -> 5 of 119, dsize 0, reloc ok
 *    and in both `.03.cse` KEEPS `(insn 58 (set (reg/v:SI 38) (reg/v:SI 37)))`
 *    unfolded.  cse1 is defeated device-free for nothing.  The reason the
 *    extension code cannot step over it is structural: the loop-top label is
 *    reached by FALL-THROUGH, so the insn ending the block is not a conditional
 *    JUMP_INSN and `cse.c:6636-6719` -- which requires
 *    `GET_CODE (p) == JUMP_INSN && ... IF_THEN_ELSE` -- is never entered.  (The
 *    only other boundary at cse.c:6534-6600 is NOTE_INSN_SETJMP, unreachable.)
 *
 * 2. THE PASS THAT THEN FOLDS IT IS `.07.gcse`.  insn 58 traced through every
 *    dump of the boundary body: `.03.cse` reg copy; **`.07.gcse` const_int 0**;
 *    `.08.loop`, `.09.cse2`, `.13.combine`, `.15.regmove` const_int 0.  The
 *    park's chain blamed cse1 and then cse2 and never named gcse -- which is
 *    why four batches of boundary work aimed at the wrong pass.  **gcse's
 *    constant propagation is GLOBAL (reaching definitions over the CFG), so no
 *    basic-block boundary of any kind can stop it.**
 *
 * 3. THE gcse GATE, READ, AND UNREACHABLE FROM C.  `hash_scan_set`
 *    (**gcse.c:1877-1892**) records a set for cprop when the dest is a pseudo,
 *    the src is a reg / CONST_INT / SYMBOL_REF / CONST_DOUBLE, and
 *    `insn == BLOCK_END (BLOCK_NUM (insn)) || oprs_available_p (pat,
 *    next_nonnote_insn (insn))`.  The second disjunct fails only if the DEST is
 *    set again later in the same basic block -- so **the LAST definition of a
 *    register in a block always satisfies one disjunct or the other**, and no
 *    source edit can keep `ret = 0` out of the cprop set table.
 *    `can_copy_p[SImode]` is true and CONST_INT is explicitly accepted.
 *
 * 4. AND THE BOUNDARY'S ZERO COST IS gcse's OWN DOING -- the lever and the
 *    blocker are ONE PASS.  The same body measured with gcse disabled as an
 *    INSTRUMENT (the no-gcse flag, passed to objcmp through its extra-flags
 *    environment hook -- NOT written here as a key=value pair, because
 *    tools/parkcheck.py greps park headers for that spelling and would then
 *    measure this park under the flag and report TOOLING) reads
 *    **95 of 119 at dsize +4**: the one-trip loop's `cmp`/branch SURVIVES.  Its
 *    freedom in the normal build is bought by gcse propagating the constant so a
 *    later pass can delete the dead branch.
 *
 * 5. THE COST ESCAPE IS MEASURED SHUT.  `CHEAPER` is `cost <` (cse.c:1479) and
 *    a pseudo costs 1 (cse.c:509-514), so the register would take the class head
 *    if the constant cost more than 1.  `arm_rtx_costs` (**arm.c:2077-2082**)
 *    returns **0** for a `const_int` below 256 with `outer == SET`, so the ROM's
 *    own value 0 is precisely the one value that cannot lose the head.
 *    Instruments: `ret = 0x4321;` **11 of 119 at dsize +4**; `ret = 256;`
 *    **97 of 119**, both RELOCDIFF.
 *
 * 6. THE POST-RELOAD BOUND RE-READ LINE BY LINE AND CONFIRMED, with one
 *    precision.  `reload_cse_simplify_set` (**reload1.c:8025-8056**) sets
 *    `old_cost = rtx_cost (src, SET)` = 0 for a CONSTANT_P src and
 *    `this_cost = REGISTER_MOVE_COST` = 4 whenever HI_REGS is an endpoint
 *    (**arm.h:1280-1285**): `4 < 0` false, the tie branch needs `4 == 0`, false.
 *    `reload_cse_simplify_operands` (**reload1.c:8216-8224**) gates on
 *    `rtx_cost (operand, SET) > rtx_cost (reg, SET)` = `0 > 1`, false.
 *    `reload_cse_move2add` (**reload1.c:8891-8910**) rewrites only against the
 *    SAME hard register's own `reg_offset[regno]` and emits a SELF-move at delta
 *    0.  Precision worth keeping: `rtx_cost` on a REG is `! CHEAP_REG`
 *    (**cse.c:804-805**), and post-reload a pseudo rtx renumbered in place keeps
 *    `REG_USERVAR_P`, so CHEAP_REG can be TRUE and that cost 0 -- it does not
 *    matter here because the candidate rtx in that loop is not the user's and
 *    `0 > 0` is false too.
 *
 * 7. AND "NECESSARY BUT NOT SUFFICIENT" IS NOT ESTABLISHED.  `.26.mach` for the
 *    installed body is instruction-for-instruction the reference through the
 *    whole init -- `mov r1, #0 / mov r8, r0 / ldrb r0, [r5] / mov sl, r1 / bl`
 *    -- with the reference's allocation (`j` r4, `v` r7, `ret` sl, `i` r5, `s`
 *    r6, `flag` r8, `u` r9, `slot` fp).  **ONLY insn 54's SET_SRC differs.**
 *    Batch 327 point 5 inferred that blocking the fold must re-order the
 *    allocnos, but it measured that with `-fno-rerun-cse-after-loop` and with a
 *    volatile load, each of which perturbs the allocation by itself.  With the
 *    device-free boundary above the allocation is UNCHANGED; the only cost is
 *    that sched2 swaps the two init copies (+1).
 *
 * NEXT, REPLACING THE OLD LIST: the question is no longer a cse boundary -- that
 * is found, free, and written above.  It is **keep `ret = 0` out of gcse's cprop
 * set table, or out of `cprop_avin` at the init.**  On the reading above the
 * first is unreachable; the second needs a SECOND reaching definition of `ret`
 * at the init, which needs a branch.  Do NOT re-propose cse-era boundaries,
 * do-while(0) wrappers, labels, or the post-reload CSE family.
 */
extern unsigned char *_GetUnit(int id);
extern int Func_80c23c0(int a);
extern int Func_80c2384(int a);
extern int Func_80c23a0(int a);
extern int _PreloadSpriteGFX(int a, int b, int c, int d);
extern char *iwram_3001e74;
extern unsigned char ewram_2018000[];

int Func_80b6d30(int slot)
{
    char *s;
    unsigned char *u;
    int flag;
    int v;
    int ret;
    int j;
    int i;
    int off;
    int a;

    s = iwram_3001e74;
    u = _GetUnit(slot);
    flag = Func_80c23c0(u[0x128]);
    ret = 0;
    v = Func_80c2384(u[0x128]);
    for (j = ret; j <= 1; j++) {
        if (u[0x129] != 0)
            continue;
        for (i = 0; i <= 5; i++) {
            off = i * 2;
            a = off + 4;
            if (*(short *)(s + a) != 0)
                continue;
            if (flag != 0)
                break;
            if (i > 4)
                continue;
            a = off + 6;
            if (*(short *)(s + a) == 0)
                break;
        }
        if (i == 6)
            break;
        if (_PreloadSpriteGFX(i, (int)(ewram_2018000 + (i << 14)), v + j,
                              Func_80c23a0(u[0x128])) == 0)
            return 0;
        if (j == 0)
            ret = (i << 12) | v;
        {
        int o = i * 2;
        a = o + 4;
        *(short *)(s + a) = slot;
        if (flag == 0) {
            a = o + 6;
            *(short *)(s + a) = slot;
        }
        }
        if (v == 0x1dc || v == 0x1e3)
            continue;
        break;
    }
    return ret;
}
