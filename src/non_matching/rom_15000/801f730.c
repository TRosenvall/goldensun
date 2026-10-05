/* Func_801f730  @  0x0801f730  [rom_15000]   *** PARK -- 3 of 33 ***
 * NON-MATCHING, 3 of 33 encodings (measured batch 322).
 *
 * NOT A LANDING. Parked at 3, PIN-FREE, improved from the installed park's 7.
 *
 * FIGURE (measured, batch 322 brief B, on this body):
 *   objcmp --func : 3 of 33 encodings differ, COUNT MATCHED (ref 33, ours 33),
 *                   SIZE MATCHED (76 v 76), relocations identical.
 *   objcmp --whole: same 3 of 33, first at index 12.
 *   per-opcode MEM screen: clean (ref ldr x3 + ldrb x1; ours identical).
 *   PIN COUNT: 0.  No inline asm, no device, no fictitious symbol.
 *
 * THE INSTALLED PARK MEASURES 7 AND IS A WRONG PROGRAM BY THE MEM SCREEN.
 *   src/non_matching/rom_15000/801f730.c reads 7 of 33 with
 *   MEM {ldr:3, ldrsb:1} against the ROM's {ldr:3, ldrb:1} -- it reads the byte
 *   SIGN-EXTENDING where the ROM reads it unsigned. A figure of 7 on the wrong
 *   load opcode is not "four away" from 3.
 *   The installed park's own header already says "Residue is now 3, not 8" and
 *   points at scratch_elev/b235/f801f77c/f801f730_sibling.c -- WHICH WAS NEVER
 *   INSTALLED. This body is that one, reproduced so the figure above is
 *   reproducible from this file alone. Batch 235 found the fix; batch 322 is
 *   installing it.
 *
 * Verify with (INSTALLED PATH):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/801f730.c \
 *     asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.s --func Func_801f730
 *
 * SPLIT SHAPE: NONE NEEDED.
 *   grep -c thumb_func_start asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.s -> 1
 *   python3 tools/datacheck.py <that .s>            -> clean (no output)
 *   python3 tools/split_s.py   <that .s> --dry-run  -> nothing to split
 *   INSTALL PATH if it ever lands: src/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a_a.c
 *
 * ---------------------------------------------------------------------------
 * WHAT THE RESIDUE IS: THREE REAL INSTRUCTIONS. NO POOL WORDS.
 *
 * Per-index, indices 30/31/32 are the alignment halfword and the two pool
 * words and they are IDENTICAL, in the ROM's order. So all three differing
 * encodings are real instructions, and the usual "an objcmp encoding can be a
 * pool word" trap does not apply here:
 *
 *      11  4b0a  ldr  r3,[pc,#40]   | 4b0a  ldr  r3,[pc,#40]
 *   XX 12  490a  ldr  r1,[pc,#40]   | 480a  ldr  r0,[pc,#40]
 *      13  681b  ldr  r3,[r3,#0]    | 681b  ldr  r3,[r3,#0]
 *   XX 14  185a  adds r2,r3,r1      | 2102  movs r1,#2
 *   XX 15  2102  movs r1,#2         | 181a  adds r2,r3,r0
 *
 * 30 of 33 exact. One register, and the two insns around it.
 *
 * THE DECIDING RUNG IS RELOAD'S SPILL CHOICE -- not sched2, not the allocator.
 *
 *   .18.greg says:   ;; 4 regs to allocate: 45 36 33 32
 *                    ;; Register dispositions: 32 in 6  33 in 5  34 in 0
 *                                              36 in 1  ...  45 in 2
 *                    Spilling for insn 134.
 *                    Using reg 0 for reload 0
 *
 *   pseudo 36 = the counter `i` -> r1;  45 = the giv cursor -> r2;
 *   33 = `r` -> r5;  32 = `a` -> r6.
 *
 *   0x1071 IS NOT A PSEUDO AT ALL. .18.greg's BB2 has the loop-created insn
 *   deleted and a RELOAD insn in its place:
 *       (note  132 ... NOTE_INSN_DELETED 0)
 *       (insn  143 (set (reg:SI 0 r0) (const_int 4209)))
 *       (insn  134 (set (reg:SI 2 r2) (plus (reg 3) (reg 0))))
 *   so index 12 is RELOAD picking a spill register, and
 *   REG_ALLOC_ORDER (config/arm/arm.h:989) is  3, 2, 1, 0, 12, 14, 4, 5, ...
 *   -- r3 (base), r2 (cursor) and r1 (counter) are all live at insn 134, so r0
 *   is simply the first free one.
 *
 *   Indices 14/15 are sched2 choosing the counter init over the add. EVEN IF A
 *   SCHEDULING LEVER FLIPPED THEM, index 14 would still differ
 *   (`adds r2,r3,r0` v `adds r2,r3,r1`), so the best a sched2 lever alone can
 *   reach here is 2, not 0. The register is the irreducible part.
 *
 *   NOTE FOR THE PRIORITY-ARITHMETIC LEVER: `;; 4 regs to allocate` is NONZERO,
 *   so global-alloc is live on this function -- but the decision is not global
 *   alloc's, so deriving allocno_compare here is wasted work. Reported because
 *   the brief asked for the rung with numbers.
 *
 * ---------------------------------------------------------------------------
 * THE BOUND, WITH ITS MECHANISM -- READ FROM THE COMPILER, NOT FROM DUMPS
 *   (~/gs_project/camelot-gcc/gcc-2.96/gcc/)
 *
 * For reload to pick r1 the counter must not be live at the giv init. It always
 * is, and the reason is two file:line facts plus the call order between them:
 *
 *  1. THE GIV INIT IS EMITTED AT THE END OF THE PREHEADER.  loop.c:4778, in
 *     strength_reduce, under the comment "Add code at loop start to initialize
 *     giv's reduced reg":
 *         emit_iv_add_mult (bl->initial_value, v->mult_val, v->add_val,
 *                           v->new_reg, loop_start);
 *     and emit_iv_add_mult (loop.c:7610) ends in
 *         emit_insn_before (seq, insert_before);
 *     with insert_before == loop_start, i.e. immediately before
 *     NOTE_INSN_LOOP_BEG. Our dump shows exactly that: insn 134 sits between
 *     insn 39 (`i = 2`) and (note 40 ... NOTE_INSN_LOOP_BEG).
 *
 *  2. THERE ARE ONLY TWO PLACES THE COUNTER INIT CAN COME FROM, AND BOTH LAND
 *     BEFORE IT.
 *       (a) ordinary preheader code -- `i = 2` written in the source. Trivially
 *           earlier, and no declaration or statement order can move it past a
 *           pass-generated insn.
 *       (b) check_dbra_loop's reversed-biv init,
 *               emit_insn_before (gen_move_insn (reg, start_value), loop_start)
 *           at loop.c:~8154.  check_dbra_loop is CALLED FROM strength_reduce at
 *           loop.c:4408 -- 370 lines BEFORE the giv-init emission at 4778.
 *           Both insert immediately before loop_start, so the LATER insertion
 *           ends up CLOSER to loop_start: the giv init always lands AFTER the
 *           dbra counter init.
 *
 * So no source shape can order them the ROM's way, the counter's hard register
 * is always live at the giv init, and reload can never choose it.
 *
 * STATED WITH EVIDENCE ATTACHED SO IT CAN BE REFUTED, not as a closed class.
 * WHAT WOULD RETIRE IT: any gcc-2.96 Thumb function in this tree where a
 * reload-materialised giv addend takes the same hard register a loop counter
 * later uses. I did not find one; I also did not scan for one.
 *
 * ---------------------------------------------------------------------------
 * A NEW POSITIVE RESULT, WORTH MORE THAN THE REGISTER
 *
 * *** THE ROM'S COUNTDOWN LOOP IS REACHABLE FROM AN UP-COUNTING SOURCE. ***
 * `for (i = 0; i < 3; i++)` compiles to the ROM's
 *     movs r1,#2 / ... / subs r1,#1 / cmp r1,#0 / bge
 * BIT-IDENTICALLY to the explicit `for (i = 2; i >= 0; i--)` -- 3 of 33 at the
 * same three indices, same 76 bytes. That is check_dbra_loop reversing the
 * loop (loop.c:4408). Two spellings that look different in C are the same
 * program here.
 *
 * CONSEQUENCE FOR EVERY FUTURE BRIEF: do not sweep LOOP FORM on a function
 * whose counter biv is used only in the exit test. for/while/do-while and
 * up/down are one equivalence class under check_dbra_loop, and sweeping them
 * measures nothing. Fifteen of my rows below are that mistake, made cheaply.
 *
 * ---------------------------------------------------------------------------
 * CROSSED AND SWEPT -- 15 ROWS EXACTLY INERT, BIT-IDENTICAL OUTPUT
 *
 * All at 3 of 33, nenc 33, size 76, reloc ok, MEM clean, idx=[12,14,15]:
 *   base (this body)                      h1 h3 h4  declaration order x3
 *   h2  statement order k= before p=      h5  while instead of for
 *   h6  i=2 hoisted above p=/k=           h7  do { } while (i >= 0)
 *   i1  q = p + 0x1071, index from 0      i2  p = iwram_3001f1c + 0x1071
 *   i3  j = 0 before q =                  i4  k = k + 0x40 not k += 0x40
 *   a1  UP-COUNTING for (i=0;i<3;i++)     a2  up-counting while        [new]
 *   a4  k += 0x40 in the for-increment    a5  *(p+k) not p[k]          [new]
 *   a6  unsigned index                                                 [new]
 *
 * MEASURED WORSE:
 *   a3  `i != 3` exit test      6   (+3 at idx 22/23/24, the loop-end compare)
 *   installed park              7   walking pointer -> ldrsb, MEM FLAG
 *
 * Flag groups, from the batch-235 header, not re-run here:
 *   -fno-schedule-insns2 6; -fno-strength-reduce 20/13; -fno-gcse,
 *   -fno-rerun-cse-after-loop, -fno-rerun-loop-opt, -fno-strict-aliasing all 3.
 *
 * WHY THE FLAT SWEEP IS THE FINDING, AND HOW IT WAS SCREENED.
 * Per the batch-321 rule that allocation edits are screened by .17.lreg and not
 * by the figure, I checked the one variant that looks like it should move the
 * add out of the preheader. `i2`'s .18.greg BB2:
 *       (note 35 ... NOTE_INSN_DELETED 0)   <- `p = iwram_3001f1c + 0x1071` GONE
 *       (note 37 ... NOTE_INSN_DELETED 0)   <- `j = 0`                      GONE
 *       (insn 43  (set (reg/v:SI 1 r1) (const_int 2)))
 *       (insn 146 (set (reg:SI 0 r0) (const_int 4209)))     <- reload, r0 again
 *       (insn 137 (set (reg:SI 2 r2) (plus (reg 3) (reg 0))))
 *       Spilling for insn 137. / Using reg 0 for reload 0
 * strength_reduce FOLDS the source's explicit +0x1071 away and RE-CREATES it at
 * the preheader end. `a1` is the same: Spilling for insn 124 / Using reg 0.
 * So the fifteen flat rows are fifteen ERASED EDITS, not fifteen inert levers.
 *
 * ---------------------------------------------------------------------------
 * WHAT IS RIGHT AND MUST BE KEPT (30 of 33, both pool words, pool ORDER)
 *   - the WALKING INT INDEX `p[k]` with `k += 0x40`, which is what produces the
 *     ROM's `ldrb` + `lsl #24` + interleaved `add r2,#0x40` + `cmp #0`. A
 *     walking POINTER gives `mov #0` + `ldrsb` and is the installed park's bug.
 *     This refutes docs/elevation.md's two by-name negatives on this function
 *     ("`ldrb` + `lsl #24` before a `cmp #0` is not reachable" and its
 *     generalisation); batch 235 found that and it reproduces here.
 *   - `signed char *` for iwram_3001f1c -- unsigned gives `ldrb`+`cmp` with no
 *     `lsl`, one instruction short.
 *   - `r = -9` assigned AFTER the Func_80056cc call, which gives
 *     `bl / mov r5,#9 / neg r5,r5 / cmp r0,#0` in that order.
 *   - the pooled 0x1071 added to the DEREFERENCED global.
 *
 * ---------------------------------------------------------------------------
 * BATCH 325 BRIEF D -- REOPENED AGAINST THE RELOAD-CURSOR READING. PARK HOLDS
 * AT 3 of 33.  Re-derived, not inherited: objcmp --func 3 of 33 (ref 33, ours
 * 33), --whole 3 of 33 first at index 12, SIZE 76 v 76, relocations identical,
 * MEM ldr=3 ldrb=1.  Indices 12/14/15 as the park says.
 *
 * *** VERDICT: THIS IS A GENUINE ALLOCATION-ORDER CASE, NOT A RELOAD CURSOR. ***
 *
 * Batch 324 found reload's round-robin cursor in `allocate_reload_reg`
 * (reload1.c:4962, `last_spill_reg` at :5003) and batch 325's brief asked every
 * REG_ALLOC_ORDER park to be retested against it.  Read in the compiler
 * (~/gs_project/camelot-gcc/gcc-2.96/gcc/), the cursor is NOT what decides this:
 *
 *  1. THE TWO DUMP LINES EVERYONE READS ARE NOT `allocate_reload_reg`'S.
 *     `grep -n 'Using reg %d for reload' reload1.c` gives ONE hit, at
 *     reload1.c:1664, inside `find_reg` (reload1.c:1588).  "Spilling for insn
 *     %d." is reload1.c:1729 in `find_reload_regs`.  `allocate_reload_reg`
 *     prints nothing at all.
 *  2. `find_reg` READS REG_ALLOC_ORDER EXPLICITLY, reload1.c:1645-1662:
 *     minimum `spill_cost`, ascending hard-reg scan, with
 *     `inv_reg_alloc_order[regno] < inv_reg_alloc_order[best_reg]` as the
 *     tie-break among equal costs.
 *  3. AND THE CURSOR CANNOT REACH PAST `find_reg`.  `choose_reload_regs_init`
 *     (reload1.c:5129) sets `reload_reg_unavailable` to the COMPLEMENT of
 *     `chain->used_spill_regs`, which is what `find_reload_regs` wrote at
 *     reload1.c:1761 from `find_reg`'s picks; and `reload_reg_unavailable` is
 *     tested in BOTH gates of the cursor loop -- `reload_reg_free_p`
 *     (reload1.c:4280) and `reload_reg_free_for_value_p` (reload1.c:4684).
 *     So the cursor walks the global `spill_regs` array but can only stop on a
 *     register find_reg already reserved FOR THAT INSN.  This function has ONE
 *     reload, so `used_spill_regs` has ONE bit and the cursor has no freedom.
 *
 *     >> A RELOAD CURSOR CAN ONLY DECIDE ANYTHING ON AN INSN WITH TWO OR MORE
 *        RELOADS (which of them gets which reserved reg), OR UNDER RELOAD
 *        INHERITANCE.  On a single-reload insn "the cursor shifted" is not an
 *        available explanation.  The `.18.greg` SPILL SET is still the right
 *        observable -- it is just find_reg's output, not a cursor's.
 *
 * WHAT find_reg ACTUALLY WEIGHS HERE.  `order_regs_for_reload`
 * (reload1.c:1518) sets spill_cost[hard] = sum of REG_N_REFS over live pseudos
 * allocated to that hard reg, and `bad_spill_regs` excludes fixed regs plus any
 * HARD reg live in or across the insn.  At insn 134 the live pseudos are the
 * pointer->r3, counter 36->r1, giv 45->r2 (in dead_or_set), `r`->r5, `a`->r6,
 * and hard r7 is live so r7 is barred.  That leaves TWO zero-cost candidates,
 * r0 and r4, and inv_reg_alloc_order (r3=0 r2=1 r1=2 r0=3 ip=4 lr=5 r4=6) picks
 * r0.  So the park's "r0 is simply the first free one" understates it: there are
 * two free registers and REG_ALLOC_ORDER chooses between them.
 *
 *   >> THE EXACT REQUIREMENT FOR THE ROM'S `ldr r1,=0x1071` IS
 *      `spill_cost[r1] == 0` AT INSN 134.  r1 beats r0 and r4 on the tie-break
 *      (inv 2 < 3 < 6), so a TIE at zero suffices -- it need not be cheaper.
 *
 * THE PARK'S BOUND SURVIVES, RE-DERIVED INDEPENDENTLY FROM loop.c:
 *   - the giv init is emit_iv_add_mult(bl->initial_value, ..., loop_start) at
 *     loop.c:4777-4778, and emit_iv_add_mult ends in
 *     emit_insn_before (seq, insert_before) at loop.c:7636 -- immediately before
 *     NOTE_INSN_LOOP_BEG, which is where insn 134 sits in our dump.  CONFIRMED.
 *   - check_dbra_loop is called from strength_reduce at loop.c:4408, 370 lines
 *     BEFORE that, and also inserts before loop_start, so the later insertion
 *     lands CLOSER to loop_start: the giv init is always AFTER the counter init.
 *     CONFIRMED.
 *   - and in THIS function the counter init is not check_dbra_loop's at all: it
 *     is insn 39, a low (expand-era) UID, i.e. the source's own `i = 2`.  No
 *     pass-created insn can precede it.
 *   The only other route to spill_cost[r1] == 0 needs live pseudos in BOTH r0
 *   and r4 across the preheader -- two extra loop-spanning locals, a different
 *   program and a different prologue.
 *
 * ONE CORRECTION TO THE PARK'S DECOMPOSITION, IN ITS FAVOUR: indices 14/15 are
 * not an independent sched2 defect.  In the ROM `add r2,r3,r1` READS r1 and
 * `mov r1,#2` WRITES it -- a WAR dependence that forces the ROM's order.  Fix
 * index 12 and 14/15 follow for free.  ONE CAUSE, THREE ENCODINGS.
 *
 * MEASURED THIS ROUND, 13 more bodies, none better than 3 (ref 33 / ours 33,
 * size 76, relocations identical, idx=[12,14,15] unless noted):
 *   q06  k = 0x1071 before p = iwram_3001f1c                    3 (inert)
 *   u04  q = p + 0x1071 with an index from 0                     3 (inert)
 *   u05  i = 2 first, do/while, i-- at the bottom                3 (inert)
 *   u02  p[0x1071 + i * 0x40], no walking index                  5
 *   u03  p[0x1071 + (i << 6)]                                    5
 *   u06  p = base + 0x1071 then p[i * 0x40]                      5
 *   u01  single biv: for (k = 0x1071; k < 0x1131; k += 0x40)     6, 34 insns, +4 bytes
 *   q01  walking `signed char *q`, init BEFORE the counter       7  MEM ldrsb
 *   q02  same, spelled q[0]                                      7  MEM ldrsb
 *   q03  same, up-counting loop                                  7  MEM ldrsb
 *   q04  same, with a separate base local `p`                     7  MEM ldrsb
 *   r03  same, `if (*q)`                                         7  MEM ldrsb
 *   r05  same, via `signed char c = *q;`                         7  MEM ldrsb
 *   r07  same, via `int c = *q;`                                 7  MEM ldrsb
 *   q05  walking pointer, `while (i-- != 0)`                    25, 35 insns, +4 bytes
 *   r01  walking `unsigned char *q`                             20, 31 insns, -4 bytes
 *   r02  walking `char *q`                                      20, 31 insns, -4 bytes
 *   r04  walking unsigned ptr, `(signed char)*q != 0`           20, 31 insns, -4 bytes
 *   r06  walking unsigned ptr, `(*q << 24) != 0`                20, 31 insns, -4 bytes
 *   r08  walking unsigned ptr, `if (*q)`                        20, 31 insns, -4 bytes
 *
 * *** THE SHARPEST NEW FACT, AND IT IS A HALF-FIX IN THE OTHER LIST. ***
 * The walking-pointer form FIXES indices 12/14/15 -- q01's first differing
 * encoding is at index 16, so the reload register and the two insns around it
 * are EXACT.  Writing `q = iwram_3001f1c + 0x1071;` BEFORE the loop means the
 * 0x1071 is materialised before the counter is live, and find_reg takes r1.
 * What it breaks instead is the LOAD: Thumb-1 `ldrsb` has no immediate-offset
 * form, so `*q` with a `signed char *` costs `mov r3,#0` + `ldrsb r3,[r2,r3]`
 * where the ROM has `ldrb r3,[r2]` + `lsl r3,#24` -- a MEM-screen failure
 * (ldrsb=1 against the ROM's ldrb=1), not a distance.  An `unsigned char *`
 * gets `ldrb` back but loses the `lsl #24` entirely and runs TWO INSTRUCTIONS
 * SHORT (31 against 33), so its 20 is misalignment.
 *
 * So the two halves are: the walking POINTER buys index 12 and costs the load;
 * the walking INT INDEX buys the load and costs index 12.  SIX spellings of the
 * load were swept on top of the walking pointer (r01-r08) and none recovers
 * `ldrb` + `lsl #24` with the right length.
 *
 *   >> NAMED REMAINING CAUSE: the `ldrb` + `lsl #24` pair is the expand-time
 *      QImode sign-extend-by-shift-pair (its `asr #24` is dropped by combine's
 *      simplify_comparison against 0), and gcc only chooses it when the address
 *      is a `(plus base index)` at EXPAND time -- which is exactly the spelling
 *      that forces strength_reduce to create the giv init, and therefore the
 *      reload, after the counter init.  To close this park someone has to find a
 *      QImode sign-extending load whose address is a plain register at expand
 *      time and which still takes the shift-pair path.  That is one grep in
 *      arm.md's extendqisi/movqi alternatives, and it was not done here.
 *
 * (Also reconfirmed from the park: the explicit-countdown and up-counting loop
 * forms are bit-identical under check_dbra_loop, so loop form is one equivalence
 * class here and sweeping it measures nothing.)
 *
 * ===========================================================================
 * BATCH 327 BRIEF D -- *** THE BOUND ABOVE IS REFUTED.  THIS FUNCTION IS ONE
 * INSTRUCTION FROM MATCHING. ***  PARK HOLDS AT 3 of 33, device-free and
 * MEM-clean, but the remaining cause is a DIFFERENT one and it is now exact.
 * ===========================================================================
 * Re-derived: objcmp --func 3 of 33 (ref 33, ours 33), first at index 12
 * (ref 490a, ours 480a).  Figure unchanged.
 *
 * ---- THE REFUTATION ------------------------------------------------------
 * scratch_elev/b327/D/f730_w1.c -- walking `signed char *q` biv, a named
 * `signed char c = *q;`, and `q += 0x40;` written BETWEEN the read and the test
 * -- produces the ROM's instruction stream with EXACTLY ONE INSTRUCTION MISSING
 * (32 lines against 33).  tryc --full:
 *       rom ldr r3, =0x3001f1c     ours ldr r3, =0x3001f1c
 *       rom ldr r1, =0x1071        ours ldr r1, =0x1071     <<< THE ROM'S r1
 *       rom ldr r3, [r3, #0x0]     ours ldr r3, [r3, #0x0]
 *       rom add r2, r3, r1         ours add r2, r3, r1      <<< ROM'S ORDER
 *       rom mov r1, #0x2           ours mov r1, #0x2        <<< COUNTER AFTER
 *       rom ldrb r3, [r2, #0x0]    ours ldrb r3, [r2, #0x0]
 *    -> rom lsl  r3, #0x18         ours add r2, #0x40       <<< ONLY DEFECT
 *       ... identical thereafter, to and including `pop {r1} / bx r1`.
 *
 * WHY THE BOUND WAS WRONG: it assumed the 0x1071 must come from a RELOAD of a
 * strength-reduced giv init, and derived (twice, correctly) that loop.c orders
 * the giv init after the counter init.  With a walking-pointer BIV,
 * `q = base + 0x1071` is the source's own insn, the 0x1071 is an ORDINARY
 * PSEUDO that global-alloc places in r1 and that is dead before `mov r1,#2`.
 * No reload is involved, and find_reg / spill_cost[r1] never enter the question.
 * Everything derived from loop.c:4408 / :4778 is still true and simply is not
 * the governing mechanism.
 *
 * ---- AND THE PARK'S "ONE GREP IN arm.md" WAS THE WRONG QUESTION ----------
 * The header above says gcc "only chooses [ldrb + lsl #24] when the address is a
 * (plus base index) at EXPAND time".  Both halves are false:
 *   * `extendqisi2` (arm.md:3449) is a define_expand whose THUMB arm
 *     (:3471-3488) is unconditional and ends in DONE: it does
 *     `copy_to_mode_reg (QImode, operands[1])` then `ashift 24` then
 *     `ashiftrt 24`.  So on Thumb EVERY QImode sign-extending load expands to
 *     movqi + lsl + asr, whatever the address.  `*thumb_extendqisi2_insn`
 *     (arm.md:3543) is reachable ONLY by combine re-forming
 *     (sign_extend:SI (mem:QI ...)).  There is no expand-time choice to find.
 *   * In THIS body the address already IS a plain register at expand time --
 *     .12.life insn 50 is `(set (reg:QI 39) (mem:QI (reg:SI 45) 0))`.
 *
 * THE REAL QUESTION IS WHAT STOPS COMBINE, and this body's own dump answers it:
 *     (insn  50 (set (reg:QI 39) (mem:QI (reg:SI 45) 0)))      ldrb
 *     (insn 130 (set (reg:SI 45) (plus (reg:SI 45) 64)))       THE GIV BUMP
 *     (insn  51 (set (reg:SI 41) (ashift (subreg:SI (reg:QI 39)) 24)))
 *     (insn  52 (set (reg:SI 40) (ashiftrt (reg:SI 41) 24)))
 * can_combine_p (combine.c:933) refuses at the guard on :1083-1088:
 *     || (! all_adjacent
 *         && (((GET_CODE (src) != MEM
 *               || ! find_reg_note (insn, REG_EQUIV, src))
 *              && use_crosses_set_p (src, INSN_CUID (insn))) ...
 * -- insn 130 WRITES reg 45, the MEM's own address, so the 3-insn merge dies and
 * the `asr` is then killed by simplify_comparison against 0, leaving the ROM's
 * `ldrb` + `lsl #24` + `cmp #0`.  NOTE the predicate: the intervening insn must
 * SET A REGISTER USED IN THE ADDRESS.  A counter decrement makes `all_adjacent`
 * false but leaves use_crosses_set_p false, so the `&&` does not fire.
 *
 * ---- THE CORRECT BOUND: A MUTUAL EXCLUSION THROUGH loop.c ----------------
 *   * `ldrb` + `lsl #24` needs (a) an UNNAMED `signed char` rvalue, because
 *     PROMOTE_MODE makes a NAMED char local an SImode pseudo loaded by
 *     *thumb_zero_extendqisi2 and marks the subreg `/u` -- w1's .12.life:193
 *     `(zero_extend:SI (mem:QI (reg/v:SI 36) 0))` and :211
 *     `REG_EQUAL (sign_extend:SI (subreg/s/u:QI (reg:SI 41) 0))` -- so
 *     nonzero_bits <= 0xff and simplify_comparison takes
 *     (ne (ashift x 24) 0) all the way to (ne x 0), killing BOTH shifts;
 *     and (b) the intervening address-clobbering insn, which only loop.c's GIV
 *     update supplies ==> the base pointer must be loop-INVARIANT ==>
 *     strength_reduce folds the +0x1071 into the giv's add_val and
 *     re-materialises it as a RELOAD at the preheader end ==> r0.
 *   * `ldr r1,=0x1071` needs the +0x1071 to be an ordinary pre-loop pseudo ==>
 *     the pointer must be a BIV ==> the address is a plain register with no
 *     intervening set ==> either combine merges to `ldrsb`, or the named temp is
 *     promoted `/u` and the `lsl` dies.
 *
 *   >> WHAT WOULD RETIRE IT: any way to get an insn that WRITES the load's
 *      address register between the `movqi` and its `ashift` in a loop whose
 *      pointer is a source-level biv.  The obvious candidate -- bump the
 *      POINTER and keep a CONSTANT index, so the address is a giv off a pointer
 *      biv -- measures EXACTLY INERT at 3 (y1 below): strength_reduce folds it
 *      to the same giv.
 *
 * ---- MEASURED, batch 327 (base = this body, 3 of 33, ref 33 / ours 33) ----
 *   w2  `signed char *q` biv, `int c = *q;`, bump between      2  33/33  MEM ldrsb
 *   y3  `unsigned char *q`, `int c = (signed char)*q;`, ditto  2  33/33  MEM ldrsb
 *   y1  bump the POINTER, constant index 0x1071                3  33/33  INERT
 *   y4  `q += 0x40` first, read q[0], start 0x1031             4  33/33  MEM
 *   q01 park's repro: walk sc*, bump after the `if`            7  33/33  MEM
 *   w6  QI temp, bump in the for-increment clause              7  33/33  MEM
 *   y2  `p = base+0x1071` biv, `p[0]`, `p += 0x40`             7  33/33  MEM
 *   w1  `signed char c = *q;`, bump between               20  33/31  THE ROM -1 lsl
 *   w3/w4/w5/w8  w1 with decl order / `q = q + 0x40` /
 *                up-counting / a separate base local      20  33/31  (as w1)
 *   w7  this body's walking index + a QI temp above the test   22  33/31
 *
 * The 2s read better than this park's 3 but on the WRONG LOAD OPCODE, which this
 * park's own installed-figure decision already settled ("a figure of 7 on the
 * wrong load opcode is not four away from 3").  Both are recorded as figures
 * ABOUT THE BLOCKER.  PARK HOLDS AT 3 of 33.
 */
extern int Func_80056cc(void);
extern int Func_8005c68(void);
extern void Func_8005cf8(void);
extern signed char *iwram_3001f1c;

int Func_801f730(int a)
{
    int r;
    int t;
    int k;
    int i;
    signed char *p;

    t = Func_80056cc();
    r = -9;
    if (t == 0) {
        r = Func_8005c68();
        if (a != 0) {
            p = iwram_3001f1c;
            k = 0x1071;
            for (i = 2; i >= 0; i--) {
                if (p[k] != 0) {
                    r--;
                }
                k += 0x40;
            }
        }
    }
    Func_8005cf8();
    return r;
}
