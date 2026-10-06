/* ============ BATCH 328, BRIEF F -- THE LAYER IS NOW PINNED DOWN ============
 *
 * FIGURE RE-DERIVED, NOT INHERITED: 6 differing encodings of 167 (ref 167, ours
 * 167).  No objcmp SIZE line, no INSTRUCTION COUNT line, relocations identical.
 * THE FIGURE IS A DISTANCE.  The park's 6 survives untouched.
 *
 * (1) LAYER 1 ALREADY AGREES WITH THE ROM.  MEASURED, .18.greg of the body as
 *     installed:
 *         Spilling for insn 322.
 *         Using reg 3 for reload 0
 *         ;; Register dispositions: ... 108 in 0  109 in 3
 *     `find_reg` reserves r3 -- THE ROM'S REGISTER -- and r1 is what gets
 *     emitted.  So no edit aimed at layer 1 can help: not `spill_cost`, not
 *     `inv_reg_alloc_order`, not allocno priority, not `REG_N_REFS`, not
 *     declaration order.  The whole residue is layer 3.
 *
 * (2) ONE STANDING HYPOTHESIS IS NOW DEAD: "pass 0 reused r1".
 *     `allocate_reload_reg` is NOT a plain round-robin.  reload1.c:4996-5006
 *     runs TWO PASSES, and pass 0 accepts a register only when
 *         TEST_HARD_REG_BIT (reload_reg_used_at_all, regnum)
 *         && ! TEST_HARD_REG_BIT (reload_reg_used_for_inherit, regnum)
 *     (reload1.c:5029-5035) -- a REUSE-FIRST pass over registers already taken
 *     by another reload OF THE SAME INSN.  And `reload_reg_used_at_all` is
 *     CLEARED PER INSN, in `choose_reload_regs_init` at reload1.c:5102 --
 *     four lines above the `COMPL_HARD_REG_SET (reload_reg_unavailable,
 *     chain->used_spill_regs)` at :5126 that this park already cites.
 *     INSN 322 HAS EXACTLY ONE RELOAD, so the set is empty when
 *     allocate_reload_reg runs and PASS 0 CAN MATCH NOTHING.  r1 is therefore
 *     chosen on pass 1, the plain round-robin from `last_spill_reg`.
 *     => the three-layer model is CONFIRMED rather than complicated, and the
 *        ONLY remaining free quantity is `chain->used_spill_regs` at insn 322,
 *        i.e. WHICH PSEUDO HOLDS r3 ACROSS IT.  `.18.greg` says pseudo 109
 *        (the `const_int 48`, `109 in 3`) is the only candidate anywhere near,
 *        which is the park's own prime suspect -- still unconfirmed, because the
 *        per-chain `live_throughout` set is not dumped.
 *
 * (3) A CITATION CORRECTION THAT MATTERS BEYOND THIS FILE.  Anywhere in this
 *     bank's parks that blames `insert_insn_end_bb`'s successor test for a PRE
 *     placement: gcse.c:4389-4446 (`pre_edge_insert`) reaches
 *     `insert_insn_end_bb` ONLY when `(eg->flags & EDGE_ABNORMAL) ==
 *     EDGE_ABNORMAL`.  Every normal edge goes to `insert_insn_on_edge` and is
 *     committed by `commit_one_edge_insertion` (flow.c:1656-1718), which tests
 *     IN ORDER: dest has ONE pred -> insert at TOP of dest; else src has ONE
 *     succ -> insert at END of src before its jump; else `split_edge`.  TWO
 *     non-splitting routes, not one.  Written up in full in
 *     src/non_matching/rom_a1000/80a9f10.c, this module's other park.
 *
 * NEXT STEP, REPLACING THE PARK'S OWN (which chased reload COUNT): confirm or
 * refute pseudo 109 in insn 322's `live_throughout` by instrumenting
 * `finish_spills` (reload1.c:3609-3627) or by printing `chain->used_spill_regs`,
 * then find a BYTE-NEUTRAL source change that kills 109's liveness there.  Do
 * not spend another round on spellings of the differing site; layer 1 is right.
 * ==========================================================================  *
 * Func_80a8f40 -- DrawEquipPage -- 0x080a8f40, asm/rom_a1000/rom_a8604_a_a_c_c_c.s
 * NON-MATCHING, 6 of 167 encodings (measured batch 322).
 *
 * PARK, 6 of 167 encodings  (MEASURED batch 322, brief H).  PINS: 0.
 * Instruction count matches, 167 against 167; RELOCATIONS ARE IDENTICAL.
 * The figure IS a distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a8f40.c \
 *     asm/rom_a1000/rom_a8604_a_a_c_c_c.s --func Func_80a8f40
 *
 * SPLIT SHAPE: a pure text split, three ways.  `tools/datacheck.py
 * asm/rom_a1000/rom_a8604_a_a_c_c_c.s` is silent (exit 0 -- no data in the .s).
 * `tools/split_s.py asm/rom_a1000/rom_a8604_a_a_c_c_c.s Func_80a8f40 --dry-run`:
 *     would write ..._a.s  (1 function, 254 lines)
 *     would write ..._b.s  (1 function, 171 lines)   <- this function
 *     would write ..._c.s  (1 function, 325 lines)
 *     would REMOVE ..._a_a_c_c_c.s, would rewrite stage1.ld
 * Install path on a landing: src/rom_a1000/rom_a8604_a_a_c_c_c_b.c.  Exports: none.
 *
 * VERIFICATION SHIM, scratch only: the `__asm__(".equ _MSG_333, 0x333")` line
 * below.  _MSG_333 is ALREADY ADMITTED in message.sym (batch 285); a landed file
 * must NOT carry the shim.  It is an instrument, not a result, and it is the only
 * device in this file.
 *
 * ============ THE DIAGNOSIS, REPRODUCED IN FULL IN BATCH 322 ============
 *
 * The park's blocker claim is one of the few that survives intact, and I
 * reproduced it rather than inheriting it.  Compiled with
 * `-fno-schedule-insns2`, THE TAIL BECOMES THE ROM'S INSTRUCTION ORDER EXACTLY
 * and the only remaining difference is one register field:
 *
 *   ROM        ldr r3,[sp,#4] / ldrb r0,[r3,#0xf] / mov r3,#0x30 / str r3,[sp]
 *              / mov r1,#2 / mov r2,r8 / mov r3,#0x18 / bl _Func_801ea08
 *   -fno-s2    ldr r1,[sp,#4] / ldrb r0,[r1,#15]  / mov r3,#48   / str r3,[sp]
 *              / mov r1,#2 / mov r2,r8 / mov r3,#24 / bl _Func_801ea08
 *   with s2    ldr r1,[sp,#4] / mov r3,#48 / ldrb r0,[r1,#15] / mov r2,r8
 *              / str r3,[sp] / mov r1,#2 / mov r3,#24
 *
 * So the WHOLE residue is `allocate_reload_reg` (reload1.c:5003) taking r1 for
 * the third reload of the spilled `unit` pointer where the ROM took r3, and the
 * schedule difference is a CONSEQUENCE: with the pointer in r3 the ROM's order
 * is forced, because `mov r3,#0x30` cannot then be hoisted above the `ldrb`.
 * 6 = 2 register fields x their sched2 fan-out.  This is the `last_spill_reg`
 * rotation, and every insn before that block is byte-identical, so the rotation
 * state entering it is identical too -- there is nothing earlier to perturb
 * without changing emitted code.
 *
 * ============ CROSSED SWEEP, BATCH 322 -- FLAT, AND THAT IS THE FINDING ======
 *
 * `tools/crossfire.py --depth 2` over six declaration edits, 7 edits total,
 * every pair.  Reference memory profile ldr=18 ldrb=6 ldrh=2 str=6; BASE carried
 * NO flags (no COUNT, no MEM, no RELOC).  Sixteen rows read EXACTLY 6:
 *
 *   `_GetUnit` -> `void *`                          6  (exactly inert)
 *   `_Func_801ea08` -> `int` return                 6  (exactly inert)
 *   `_UIDrawText` first arg -> `unsigned char *`    6  (exactly inert)
 *   `_Func_801e8b0` first arg -> `unsigned char *`  6  (exactly inert)
 *   `iwram_3001f2c` -> `unsigned char *const`       6  (exactly inert)
 *   ... and every pair of the above                 6  (exactly inert)
 *
 * TRIED AND WORSE in the same sweep, with figures:
 *   `t` as `unsigned char`                          87 of 167 at 169 insns -- COUNT,
 *                                                   so that figure is misalignment
 *   `af22c` as `unsigned char *` not `[]`           23, RELOC + MEM -- a WRONG
 *                                                   PROGRAM (one extra indirection)
 *
 * THE DECLARATION LEVER IS A DIVIDEND HERE, NOT A FIX.  Five independent type
 * corrections are provably free.  I have NOT folded them into the body, because
 * `_Func_801ea08` returning `int` is a guess with no evidence behind it, and the
 * `void` declarations below agree with three other files in this bank
 * (80a8604.c, 80a4924.c, 80a112c.c).  Changing them would trade a measured
 * nothing for an unevidenced claim.
 *
 * RETURN TYPES CHECKED AGAINST THE TREE, not against park extern lines, because
 * a wrong return type has been found twice in this bank: none of _Func_801e7c0,
 * _Func_801e8b0, _Func_801e9d4, _Func_801ea08, _UIDrawText has a DEFINITION in
 * src/ -- they are all still asm -- and every park that declares them declares
 * them `void` (_GetUnit / _GetMoveInfo return pointers, as here).  So there is
 * no definition-level evidence to correct, and the `int`-return lever is inert
 * here by measurement rather than by argument.
 *
 * ============ WHAT IS RIGHT, kept from the park ============
 * The four levers below were transferred from Func_80a6b64 in
 * rom_a5534_c_c_c_a_c_c.s, which went EXACT in batch 290 and is the same drawing
 * routine one screen over.  The first candidate written with all four already in
 * place scored 6, and nothing since has moved it.
 *
 *  1. THE LOOP IS `i = 0; if (n > i) { ofs = ...; do { ... } while (n > i); }`,
 *     NOT a `for`.  The ROM emits the entry guard BEFORE the walking offset's
 *     init, so the init sits in the LOOP PREHEADER, after the copied exit test.
 *     Measured on the twin: 19 differing as a `for`, 6 as guard + do/while.
 *  2. THE LOOP CONDITION IS SPELLED COUNT-FIRST, `n > i`.  `i < n` is 83
 *     differing on the twin: the ROM's `cmp r9, r10 / bhi` puts the count first
 *     and gcc does not commute it.
 *  3. THE ADDRESS IS `*(unsigned short *)(ofs + (int)state)` -- OFFSET FIRST.
 *     `state + ofs` gives `ldrh rD,[state,ofs]`; the ROM has `ldrh rD,[ofs,state]`.
 *  4. `i` AND `n` ARE `unsigned char`.  The lsl #24 / lsr #24 pairs are the QImode
 *     zero-extension, and they are also what makes the two loop compares unsigned
 *     (`bhi` / `bls`).  `int` counters cost 4 instructions and 89 differing.
 *
 * TRIED AND INERT (park, all still 6): a named local for `unit[0xf]`; a second
 * pointer local `u2 = unit`; `*(unsigned char *)(unit + 0x129)`; `&af22c[0]`; a
 * `struct Unit *` with real f0f/f129 fields; an `int`-typed `unit` with casts;
 * declaring _Func_801ea08 / _Func_801e8b0 / _Func_801e7c0 / _Func_801e9d4 `int`.
 * TRIED AND WORSE (park): a shared `y = 0x30` local across the last two calls
 * (90); a local for the 0x741 message id (21); `_UIDrawText` declared `int` (9);
 * hoisting `unit[0xf]` above the _UIDrawText call (21).
 *
 * THE `.Laf22c` REFERENCE NEEDS NO label.sym ENTRY.  `extern unsigned char
 * af22c[] __asm__(".Laf22c");` makes gcc emit the relocation against the label
 * verbatim and objcmp reports the relocation table IDENTICAL.  The label is
 * already `.global` at asm/rom_a1000/rom_a8604_c_c_c_c_c.s:4.
 *
 * ============ BATCH 326, BRIEF C: RE-MEASURED 6, AND THE PARK'S =============
 * ============ ATTRIBUTION TO THE ROTATION IS WRONG              =============
 *
 * Re-measured 6 of 167: ref and ours both 167 encodings, SIZE equal, NO
 * `INSTRUCTION COUNT` line, relocations identical.  The figure IS a distance.
 *
 * THE LAYER-1 / LAYER-3 DIVERGENCE, MEASURED HERE.  `.18.greg` says
 *
 *     Spilling for insn 322.
 *     Using reg 3 for reload 0
 *
 * and `.19.flow2` emits
 *
 *     (insn 455 (set (reg:SI 1 r1) (mem:SI (plus (reg:SI 13 sp) (const_int 4)))))
 *     (insn 322 (set (reg:SI 0 r0)
 *               (zero_extend:SI (mem:QI (plus (reg:SI 1 r1) (const_int 15))))))
 *
 * `find_reg` printed r3; the emitted register is r1.  That is batch 325 brief E's
 * observable reproduced in a second bank, on a SINGLE-RELOAD insn -- so a
 * `Using reg` line is not the register you get even when there is only one.
 *
 * THE CURSOR IS NOT THE VARIABLE.  `unit` is reloaded three times in the tail,
 * and the first two agree with the ROM exactly:
 *
 *     reload 1  `_Func_801e8b0(unit, ...)`   ROM ldr r0,[sp,#4]   ours ldr r0,[sp,#4]
 *     reload 2  `unit[0x129]`                ROM ldr r1,[sp,#4]   ours ldr r1,[sp,#4]
 *     reload 3  `unit[0xf]`                  ROM ldr r3,[sp,#4]   ours ldr r1,[sp,#4]
 *
 * So `last_spill_reg` stands at r1's index entering reload 3 in BOTH builds.
 * `spill_regs` is built by ASCENDING HARD REG NUMBER -- reload1.c:3527-3532 is a
 * plain `for (i = 0; i < FIRST_PSEUDO_REGISTER; i++)` over `used_spill_regs`,
 * NOT `REG_ALLOC_ORDER` (which only reaches `find_reg`'s tie-break, layer 1).
 * From r1 the scan is therefore r2, r3, r0, r1: the ROM STOPPED AT r3, and we fell
 * through r2, r3 and r0 and wrapped back to r1.
 *
 * > CORRECTED QUESTION, with its evidence.  The variable is WHETHER r3 IS
 * > AVAILABLE AT INSN 322, not where the rotation stands.  Availability is
 * > `choose_reload_regs_init`'s
 * > `COMPL_HARD_REG_SET (reload_reg_unavailable, chain->used_spill_regs)`
 * > (reload1.c:5126), and `chain->used_spill_regs` is `finish_spills`'
 * > COMPL(hard regs used by pseudos in `live_throughout | dead_or_set`) INTERSECT
 * > the global spill set (reload1.c:3600-3628).  SO THIS IS A PSEUDO-LIVENESS
 * > QUESTION AT ONE INSN -- which is the action batch 325 settled on -- and NOT a
 * > count-the-reloads question.  The park's own "NEXT STEP FOR PASS 3" below
 * > therefore chases the wrong variable: one more or one fewer reload-register
 * > allocation EARLIER cannot be the answer when reloads 1 and 2 already agree.
 * > The one reading this evidence cannot exclude is that the ROM's build made an
 * > extra allocation BETWEEN reloads 2 and 3 that emitted no surviving insn; that
 * > is still a question about this one insn, not about the function's total.
 *
 * PRIME SUSPECT, STATED AS A HYPOTHESIS AND NOT CONFIRMED.  Pre-reload
 * (`.17.lreg`) the block is
 *
 *     insn 322: (set (reg:SI 108) (zero_extend (mem:QI (plus (reg/v:SI 36) (const_int 15)))))
 *     insn 324: (set (reg:SI 109) (const_int 48))
 *
 * and `.18.greg`'s dispositions read `108 in 0` and `109 in 3`.  Pseudo 108 being
 * `dead_or_set` at insn 322 explains why r0 is rejected.  Pseudo 109 -- the 0x30
 * written to the outgoing stack slot, carrying `REG_EQUIV (const_int 48)` -- is
 * the ONLY pseudo allocated to r3 anywhere near that insn.  I could NOT confirm
 * that 109 is in insn 322's `live_throughout`: the per-chain set is not dumped.
 * Test it; do not assume it.
 *
 * ================ REFUTED THIS BATCH, one instrument each ================
 *
 * 1. "THE TWO 0x30s ARE CSE'd INTO ONE PSEUDO LIVE ACROSS INSN 322."  My own
 *    hypothesis, and false: changing the last call's stack argument to 0x31, so no
 *    sharing with `_UIDrawText`'s 0x30 is possible, STILL emits `ldr r1,[sp,#4]`.
 *
 * 2. THE BATCH-325 `Func_808fe38` PRECEDENT DOES NOT TRANSFER.  That landing
 *    (11 -> 0) named an ADDRESS as its own statement so a pseudo left an insn's
 *    live set.  This park had tried naming the VALUE (`unit[0xf]`) and a second
 *    POINTER (`u2 = unit`) but never the ADDRESS -- a real gap in its list.  Five
 *    spellings of it, ALL EXACTLY INERT AT 6 and all still r1:
 *      `p = unit + 0xf; *p`     `p = &unit[0xf]; *p`     `*(unit + 0xf)`
 *      `q = (int)unit + 0xf; *(unsigned char *)q`        `p = unit; p[0xf]`
 *
 * 3. `const` IS NOT THE LEVER HERE, IN EITHER POSITION.  All inert at 6:
 *    `unsigned char *const unit` with an initialiser; the same without `const`;
 *    `state` alone as an initialiser; `const unsigned char *` on the POINTEE with
 *    `_GetUnit` re-declared to match.  This tests batch 325's "inert through a
 *    pointer, decisive on the object" ruling in a new place and finds it INERT ON
 *    A LOCAL POINTER OBJECT TOO.
 *
 * ================ r3 IS REACHABLE, AND WHAT THAT COSTS ================
 *
 * `unsigned char *const unit = _GetUnit(iwram_3001f2c[0x21a]);`, dropping the
 * `state` indirection, emits `ldr r3,[sp,#4] / ldrb r0,[r3,#0xf]` -- THE ROM'S
 * REGISTERS -- at 147 of 167 with relocations shifted, because reading the global
 * twice restructures the prologue.  So the park's bound is right as stated
 * ("cannot be arranged without changing emitted code") and WRONG IN SPIRIT in its
 * "or a register pin": the register is sensitive to upstream pseudo structure, not
 * only to a pin.  What is missing is a BYTE-NEUTRAL change to liveness at insn
 * 322, and none of the 13 bodies measured this batch reaches it.
 *
 * Two devices probed and reported for completeness, screened on the emitted
 * register rather than the figure: `return unit[0x10];` adds a 4th reload which
 * takes r2 while insn 322 STILL takes r1; `return first;` gives the new reload r0
 * and insn 322 r1.  Neither frees r3.
 *
 * NEXT STEP FOR PASS 3, and it is not a C question: shift the `last_spill_reg`
 * rotation.  That needs either one more or one fewer reload-register ALLOCATION
 * earlier in the function (an inherited reload does not advance the rotation),
 * which cannot be arranged without changing emitted code -- or a register pin,
 * which this file deliberately does not carry.
 *
 * ========= BATCH 329, BRIEF C: THE RESIDUE IS SOLVED, AND THE =========
 * ========= REMAINING QUESTION IS AN OWNER DECISION ONLY       =========
 *
 * FIGURE RE-DERIVED, not inherited: 6 differing encodings of 167 (ref 167, ours
 * 167), no SIZE line, no INSTRUCTION COUNT line, relocations identical, first
 * differing index 139 (ref 9b01 = ldr r3,[sp,#4]; ours 9901 = ldr r1,[sp,#4]).
 *
 * >>> ONE EDIT TAKES THIS FUNCTION TO ZERO.  Spelling the 0x741 message base as a
 * >>> SYMBOL REFERENCE -- exactly as the loop above already spells 0x333 --
 * >>>     _Func_801e7c0(unit[0x129] + (int)&_MSG_741, win, 0, 0x20);
 * >>> with `extern int _MSG_741;` measures
 * >>>     OK Func_80a8f40 -- 380 bytes, 167 encodings and 16 relocations identical
 * >>> i.e. **0 of 167, byte-identical, relocation table identical.**
 * >>> It is NOT INSTALLED, because _MSG_741 is not in message.sym and a new
 * >>> symbol-table entry is an owner decision under owner standard 1.  Measured
 * >>> here with the same .equ instrument the file already carries for _MSG_333.
 *
 * THE MECHANISM, READ END TO END IN THE DUMPS.  The park's "corrected question"
 * (pseudo liveness at insn 322) IS REFUTED, and so is its NEXT STEP's premise.
 * The variable is the `last_spill_reg` ROTATION after all, and the park closed
 * that door on a wrong count: it reasoned that because unit's reloads 1 and 2
 * already take the ROM's registers the rotation must agree, but **the rotation is
 * advanced by reload-register allocations for OTHER values too.**
 *
 * `.19.flow2` of the BASE body shows four allocations in the window, in order:
 *     insn 446  r1 = [sp,#4]        reload of `unit` for unit[0x129]   (agrees)
 *     insn 449  r2 = 0x129          reload of the CONSTANT 297
 *     insn 452  r3 = 0x741          reload of the CONSTANT 1857
 *     insn 455  r1 = [sp,#4]        reload of `unit` for unit[0xf]     <-- the 6
 * `set_reload_reg` (reload1.c:4937) sets `last_spill_reg = i` only on acceptance,
 * so it stands at r3's index entering insn 322.  `allocate_reload_reg` (:5003)
 * starts at `last_spill_reg` and PRE-INCREMENTS, so the scan is r0, r1, r2, r3;
 * r0 is the insn's own output (pseudo 108 in r0), so the first acceptable is r1.
 *     last = 0 -> r1     last = 1 -> r2     last = 2 -> r3     last = 3 -> r1
 * **Only `last_spill_reg` = r2's index reaches the ROM's r3.**
 *
 * `.19.flow2` of the SYMBOL body shows THREE allocations in the window:
 *     insn 446  r1 = [sp,#4]        insn 449  r2 = 0x129
 *     insn 452  r3 = [sp,#4]        <-- the ROM's register
 * and `.18.greg` no longer prints `Spilling for insn 298` at all.  A `symbol_ref`
 * cannot be an `add` immediate, so expand loads it into a PSEUDO, which global
 * allocation homes in r3 -- emitting the identical `ldr r3,=...`.  A `const_int`
 * 1857 is folded into the `add` and RELOAD must materialise it, which costs the
 * extra allocation.  Same instruction, same encoding, one more rotation step.
 *
 * So the 6 is: index 139's register field, plus its sched2 fan-out, which is why
 * `-fno-schedule-insns2` reduced the tail to a single register difference.
 *
 * THE DEVICE-FREE ROUTE DOES NOT EXIST, MEASURED (crossfire, 7 edits, 167
 * encodings, reference memory profile ldr=18 ldrb=6 ldrh=2 str=6 on every row).
 * ALL SEVEN EXACTLY INERT AT 6:
 *     `int base = 0x741;` named before the call ................. 6
 *     `const int base = 0x741;` ................................. 6
 *     `int msgid = unit[0x129] + 0x741;` then pass msgid ........ 6
 *     `0x741 + unit[0x129]` (operand order) ..................... 6
 *     file-scope `static const int kMsgBase = 0x741;` ........... 6
 *     `*(unit + 0x129) + 0x741` ................................. 6
 *     `unit[0x129] + 0x740 + 1` ................................. 6
 * AND THE DUMP SAYS WHY: the named-local body still emits all four allocations
 * (451 r1, 454 r2, 457 r3, 460 r1) and still carries exactly one `const_int 1857`
 * in `.17.lreg` -- cprop folds every spelling back to the immediate.  The cap is
 * structural: nothing that is a compile-time integer constant can avoid reload.
 *
 * THE OTHER THREE ROUTES, EACH CLOSED WITH ITS REASON:
 *   * make the 0x741 reload take r2 -- that breaks index 137, which matches now.
 *   * add allocations to reach last = r2 from last = r3 -- needs THREE more, each
 *     emitting an instruction.
 *   * make r1 AND r2 unavailable at insn 322 -- `reload_reg_unavailable` is
 *     COMPL(chain->used_spill_regs) (:5126) and `finish_spills` (:3609-3627)
 *     builds that from pseudos in `live_throughout | dead_or_set`.  At insn 322
 *     the only live pseudos are 32 (`win`, in r8) and the SPILLED 36, which
 *     `AND_COMPL_REG_SET` has already removed.  Two more live values in the
 *     function tail is not byte-neutral.
 *
 * WHAT THE OWNER IS BEING ASKED, stated against the _MSG_b24 ruling it resembles.
 * 0x741 is neither 8-bit-movable nor shiftable, so gcc pools it as a literal
 * either way and the ROM's pool word carries NO relocation -- the POOL-WORD
 * evidence is exactly as inconclusive as _MSG_b24's was.  The argument here is a
 * DIFFERENT observable:
 *   (a) NECESSITY, on the register field and not on the pool word.  The ROM's
 *       `ldr r3,[sp,#4]` at index 139 is UNREACHABLE from a `const_int`, because
 *       the const_int forces one extra reload-register allocation and the
 *       round-robin then cannot land on r3.  Measured both ways and read in the
 *       dumps: 0 of 167 as a symbol, 6 of 167 as a constant, seven spellings.
 *   (b) IDIOM, inside this one function.  The loop above already does
 *       `id = (item & 0x3fff) + (int)&_MSG_333` -- the same "entry id + message
 *       base" construct, feeding the SAME callee `_Func_801e7c0`, and _MSG_333
 *       was admitted in batch 285.  The two message bases in one function would
 *       otherwise be spelled two different ways.
 *   (c) COMPLETION.  It takes the function to 0, so owner standard 2 is met --
 *       unlike _FILE_e4 / _FILE_e5, which were withheld on exactly that test.
 * If the entry is declined, THE PARK FIGURE IS 6 and every route from a constant
 * is now closed, so it should be read as a terminal park rather than an open one.
 *
 * Blocker class: `allocate_reload_reg` round-robin position (reload1.c:5003) at
 * insn 322, set by the COUNT of reload-register allocations in the window, not by
 * pseudo liveness and not by the printed `Using reg` line.
 * PARK HELD AT 6 device-free; 0 of 167 at one message.sym entry.
 */
__asm__(".equ _MSG_333, 0x333");

struct MoveInfo { unsigned char pad_00[8]; unsigned char f8; unsigned char f9; };

extern unsigned char *iwram_3001f2c;
extern unsigned char *_GetUnit(int id);
extern struct MoveInfo *_GetMoveInfo(int id);
extern void Func_80a2324(int count, int first, unsigned int win, int x, int y);
extern void Func_80a21b0(unsigned int win, int total, int perPage, int page, int col);
extern void Func_80a8cc0(unsigned int win, int a, int b, int c, int d);
extern void _Func_8016498(unsigned int win);
extern void _Func_801e7c0(int id, unsigned int win, int x, int y);
extern void _Func_801e8b0(void *unit, unsigned int win, int x, int y);
extern void _Func_801e9d4(int v, int n, unsigned int win, int x, int y);
extern void _Func_801ea08(int v, int n, unsigned int win, int x, int y);
extern void _UIDrawText(void *s, unsigned int win, int x, int y);
extern unsigned char af22c[] __asm__(".Laf22c");
extern int _MSG_333;

int Func_80a8f40(unsigned int win, int a1, int *d)
{
    unsigned char *state;
    unsigned char *unit;
    unsigned char n;
    unsigned char i;
    int first;
    int ofs;
    int y;
    int id;
    int t;
    struct MoveInfo *info;

    state = iwram_3001f2c;
    unit = _GetUnit(state[0x21a]);
    _Func_8016498(win);
    first = d[2] * 5;
    n = d[5] - first;
    if (n > 5)
        n = 5;
    Func_80a2324(5, first, win, 0x50, 0x3a);
    Func_80a21b0(win, d[5], 5, d[2], 0x1c);
    _Func_801e7c0(0xaed, win, 0xb0, 0);
    i = 0;
    if (n > i) {
        ofs = first * 2 + 0xe4 * 2;
        do {
            info = _GetMoveInfo(*(unsigned short *)(ofs + (int)state) & 0x3fff);
            id = (*(unsigned short *)(ofs + (int)state) & 0x3fff) + (int)&_MSG_333;
            y = i * 16 + 0x10;
            _Func_801e7c0(id, win, 0x58, y);
            _Func_801e9d4(info->f9, 2, win, 0xb0, y);
            t = info->f8;
            if (t == 0xff)
                t = 0xb;
            else
                t = t - 1;
            Func_80a8cc0(win, 0x19, i * 2 + 2, t, 0);
            i++;
            ofs += 2;
        } while (n > i);
    }
    if (state[0x218] == 0)
        _Func_801e7c0(0xaef, win, 0x60, 0x11);
    _Func_801e8b0(unit, win, 0x28, 0);
    _Func_801e7c0(unit[0x129] + 0x741, win, 0, 0x20);
    _UIDrawText(af22c, win, 0, 0x30);
    _Func_801ea08(unit[0xf], 2, win, 0x18, 0x30);
    return 1;
}
