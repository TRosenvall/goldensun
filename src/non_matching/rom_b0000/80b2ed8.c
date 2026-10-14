/* ===== BATCH 350 BRIEF B -- 3 of 48 RE-DERIVED AND HELD.  THE PARK'S ONE
 * ===== NAMED OPEN QUESTION (jump.c) IS ANSWERED, AND IT CLOSES ROUTE (a).
 *
 * NON-MATCHING, 3 differing encodings of 48  (MEASURED THIS BATCH, not
 * inherited).  ref 48 / ours 48, 16-bit instructions 37 / 37, dsize 0,
 * relocations IDENTICAL, first differing index 29 (ref 2105 `mov r1,#5`,
 * ours 1c05 `mov r5,r0`).  All three length dimensions in phase, so the 3 IS
 * A TRUE DISTANCE.  PINS 0.  DEVICE-FREE.  NO FLAG GROUP.  BODY UNCHANGED.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b0000/80b2ed8.c \
 *     asm/rom_b0000/rom_b0070_c_c_a_c_c_a_d.s --func Func_80b2ed8
 *
 * SPLIT SHAPE (tools/piece.py, verified this batch): the reference .s holds ONE
 * function, Func_80b2ed8, and it is the only parked member -- so NO SPLIT is
 * needed and none is in the manifest.  tools/datacheck.py prints nothing for
 * this .s.  Install path when it lands: src/rom_b0000/rom_b0070_c_c_a_c_c_a_d.c.
 *
 * ===========================================================================
 * 1.  THE NEGATIVE-LIST RE-TEST: 12 SPELLINGS, 6 DISTINCT PROGRAMS
 * ===========================================================================
 * tools/sweep_variants.py with object fingerprinting
 * (scratch_elev/b350/B/v1, the generator is in that directory):
 *
 *   installed body (control)                              3  BIT-IDENTICAL base
 *   the condition named in its own statement              3  BIT-IDENTICAL base
 *   `n = 5;` duplicated into BOTH arms                    3  BIT-IDENTICAL base
 *   `s = cond ? 0xd2c : 0xd2d;`                          27  dsize -4 RELOCDIFF
 *   an explicit `goto` diamond                           27  bit-identical to the ternary
 *   `s = 0xd2c; s = s;` in each arm                       27  bit-identical to the ternary
 *   `id = 0xd2c; s = id;` in each arm                     27  bit-identical to the ternary
 *   the condition's result IS `s`                         25  RELOCDIFF
 *   `switch`, the call inside each case                   30  dsize +4 RELOCDIFF
 *   `switch`, the call at the join                       32  RELOCDIFF
 *   `_Func_8019908(h,5)` in both arms                    39  dsize +16 (park's row, reproduced)
 *   the whole tail in both arms                          39  dsize +28
 *
 * >> THE CONSTRUCT-KIND AXIS IS CLOSED.  Batch 349's `Field_Halt` lesson is
 * >> "ask what KIND of construct every recorded row has in common and try a
 * >> different kind".  Applied here: the ternary, an explicit `goto` diamond,
 * >> a second `s = s;` statement per arm and a two-variable `id`/`s` arm are
 * >> ONE PROGRAM -- four rows, one fingerprint.  `jump.c` normalises the CFG
 * >> and the extra statements fold before `.14.ce` runs.  A `switch` IS a
 * >> distinct program (unlike the other three) and is worse. <<
 *
 * >> AND THE ifcvt `last_active_insn_p` GATE IS UNREACHABLE FROM C.
 * >> `ifcvt.c:609-615` bails when the THEN block holds more than one active
 * >> insn -- that is why the park's call-in-the-arms works.  But a second
 * >> NON-call statement never reaches the gate: both `s = s;` and the
 * >> two-variable form are folded to a single set first, so
 * >> `first_active_insn` sees one insn either way.  Re-read and confirmed:
 * >> `noce_try_store_flag_constants` (`ifcvt.c:591-601`) needs
 * >> `! no_new_pseudos && GET_CODE (a) == CONST_INT && GET_CODE (b) == CONST_INT`,
 * >> and with `diff == STORE_FLAG_VALUE` it sets `normalize = 0` and fires
 * >> regardless of BRANCH_COST.  The park's reading of this is exact. <<
 *
 * ===========================================================================
 * 2.  WHY `find_cross_jump` DECLINES THE SIX-INSN TAIL -- SOLVED, WITH LINES
 * ===========================================================================
 * The park left this as its one open question: "By the letter of
 * `find_cross_jump` it should merge ... That gap is the one open question on
 * this function, and it is a question about jump.c, not about C."
 * IT IS NOT A GAP IN THE LETTER.  The park's walk omitted one insn.
 *
 * `.23.sched2` on the duplicated-tail variant (v1/g01_dup8019908.c):
 *
 *   ;; basic block 2 (THEN arm)  56 r5=0xd2c / 59 r0=r5 / 60 call Func_80b2884
 *                                / 67 r1=0x5 / 62 r5=r0 / 65 r0=r8
 *                                / 68 call _Func_8019908 / 70 pc=L91
 *   (barrier 71)
 *   ;; basic block 3 (ELSE arm)  ... / 89 call _Func_8019908
 *                                / 112 (use (const_int 0))
 *   ;; basic block 4 (JOIN)      (code_label 91) ...
 *
 * A VOID CALL CARRIES A TRAILING `(use (const_int 0))` INSN.  THE ELSE ARM
 * KEEPS ITS COPY AND THE THEN ARM DOES NOT, because the then arm's `b` sits
 * where that USE would be.  Verified across the whole dump: the two
 * `Func_80b2884` calls (insns 60 and 81) RETURN A VALUE and have no trailing
 * USE; every void call has one -- insn 112 after `_Func_8019908` in the else
 * arm, insn 114 after `_DrawSmallText` in the join.
 *
 * Now walk `find_cross_jump (e1 = jump_insn 70, e2 = code_label 91,
 * minimum = 1)`; the simplejump call site is `jump.c:660`:
 *
 *   jump.c:1445  i1 = prev_nonnote_insn (i1);              -> call_insn 68
 *   jump.c:1447  i2 = PREV_INSN (i2);
 *   jump.c:1448  while (i2 && (GET_CODE (i2) == NOTE
 *                              || GET_CODE (i2) == CODE_LABEL))
 *                  i2 = PREV_INSN (i2);                    -> insn 112, an INSN
 *   jump.c:1468  if (i2 == 0 || GET_CODE (i1) != GET_CODE (i2))
 *                  break;        <-- CALL_INSN vs INSN: BREAKS ON ITERATION ONE
 *   jump.c:1607  if (minimum <= 0 && last1 != 0 && last1 != e1)
 *                  *f1 = last1, *f2 = last2;   <-- last1 == 0, so *f1 stays 0
 *
 * NEITHER WALK SKIPS A `USE` INSN: `prev_nonnote_insn` skips only NOTEs, and
 * `:1448` skips NOTEs and CODE_LABELs.  So the asymmetric trailing USE is
 * compared against the then arm's CALL_INSN and the walk dies before recording
 * a single matching pair.  `last1` is only ever assigned at `jump.c:1599`,
 * inside the `win:` arm and gated `GET_CODE (p1) != USE && GET_CODE (p1) !=
 * CLOBBER`, so even a SYMMETRIC USE could never have been absorbed.  Hence no
 * merge at all -- which is exactly the park's own observation that the call
 * count is 12 in `.23.sched2` and still 12 in `.25.jump2`.
 *
 * Contrast the BASE body, which does merge three insns at the same site: there
 * each arm's last statement is `s = (int)Func_80b2884(s)` and that call
 * RETURNS A VALUE, so neither arm carries a trailing USE and both walks start
 * on `mov r5, r0`, an ordinary INSN.  That is why cross-jumping is not
 * disabled and does not refuse calls as such -- the park was right about that
 * and the explanation is the USE, not the call.
 *
 *   >> ROUTE (a) OF THE PARK'S "NEXT" IS CLOSED, WITH A MECHANISM.  The
 *   >> six-insn tail CANNOT cross-jump while the duplicated tail ends in a
 *   >> VOID call: the else arm keeps that call's `(use (const_int 0))`, the
 *   >> then arm does not, and `jump.c:1468` breaks on the GET_CODE mismatch at
 *   >> iteration one.  BOTH statements this park could duplicate
 *   >> (`_Func_8019908`, `_DrawSmallText`) are void, so NO duplication depth
 *   >> escapes it -- `g11_dup_all` duplicating both reads 39 as well.
 *   >> WHAT WOULD RETIRE IT: a tail whose LAST statement is a non-void call or
 *   >> an ordinary assignment, which this function does not have and which
 *   >> cannot be added without emitting an instruction. <<
 *
 * ===========================================================================
 * 3.  THE GOOD NEWS INSIDE IT, AND WHERE THE NEXT READER SHOULD GO
 * ===========================================================================
 * Block 2 above is the REFERENCE'S ORDER VERBATIM -- `mov r0,r5 /
 * bl Func_80b2884 / mov r1,#5 / mov r5,r0 / mov r0,r8 / bl _Func_8019908` --
 * so the park is right that the ordering is not the blocker and sched2 will
 * produce it for free once the three movs share a block.
 *
 *   ref   mov r0,r5 / bl Func_80b2884 / mov r1,#5 / mov r5,r0 / mov r0,r8 / bl
 *   ours  mov r0,r5 / bl Func_80b2884 / mov r5,r0 / mov r0,r8 / mov r1,#5 / bl
 *
 * THE REFERENCE PUTS `bl Func_80b2884` IN THE JOIN BLOCK: its arms hold only
 * `ldr r5,=0xd2c` and `ldr r5,=0xd2d` (read off the reference .s this batch).
 * So in the original the three movs shared a block for nothing.  Getting there
 * needs `.14.ce` not to convert a ONE-INSN-PER-ARM diamond of two CONST_INTs
 * differing by 1, and `ifcvt.c:591-601` fires on exactly that.
 *
 *   >> THE PARK'S TWO ROUTES HAVE BECOME ONE.  The open question is the ifcvt
 *   >> bail, NOT the cross-jump.  Everything that defeats ifcvt by adding an
 *   >> arm statement either folds away (measured above) or emits an
 *   >> instruction.  The remaining untested gates in `noce_process_if_block`
 *   >> are `ifcvt.c:646-648` ("X may not be mentioned in the range
 *   >> (cond_earliest, jump]") and `:651-654` ("A and B may not be modified in
 *   >> [cond_earliest, jump)"); A and B are CONST_INTs so `:651` is dead, and
 *   >> `:646` needs `s`'s pseudo mentioned in the condition's setup, which the
 *   >> reference forbids -- it has `cmp r0,#0` against the call's result while
 *   >> the arms write r5, i.e. two pseudos.  Measured: making the condition's
 *   >> result BE `s` reads 25 with dirty relocations. <<
 *
 * `precompute_register_parameters` re-read this batch (`calls.c:805-854`): the
 * park's reading is exact.  The gate is `rtx_cost (args[i].value, SET) > 2` at
 * `:850`, with the value not already a REG at `:846-849`; `arm_rtx_costs`
 * returns 0 for a CONST_INT under 256 with `outer == SET`, so a literal `5`
 * can never be precomputed.  DO NOT RE-PROPOSE IT.
 *
 * -- scratch_elev/b350/B
 * ===========================================================================
 */
/* Func_80b2ed8 -- NOT MATCHING
 *
 * NON-MATCHING, 3 of 48 encodings  (MEASURED, batch 333).
 *   SIZE, ENCODING COUNT AND INSTRUCTION COUNT ARE ALL EXACT -- 48 encodings
 *   both sides, 37 instructions both sides, relocations identical.  THE FIGURE
 *   IS A TRUE DISTANCE, not phase, for the first time in this park's life; it
 *   stood at twenty-six of forty-eight on a stream two encodings short.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b0000/80b2ed8.c \
 *     asm/rom_b0000/rom_b0070_c_c_a_c_c_a_d.s --func Func_80b2ed8
 *
 * === BATCH 333: THE MISSING INSTRUCTION IS FOUND. ONE VARIABLE, NOT TWO. ===
 *
 * The park's standing verdict -- "Nothing puts the constant in a callee-saved
 * register first ... that is the same allocator disagreement as
 * src/non_matching/ovl_7ed0a0/2009458.c" -- IS REFUTED.  It is not an allocator
 * disagreement.  The ROM holds the MESSAGE ID and the RETURNED HANDLE in the
 * same register, r5, because THEY ARE THE SAME VARIABLE IN THE SOURCE:
 *
 *     if (Func_80b27b0(b, lang)) { s = 0xd2c; s = (int)Func_80b2884(s); }
 *     else                       { s = 0xd2d; s = (int)Func_80b2884(s); }
 *
 * One `int s` for both.  That single pseudo crosses _Func_8019908, so find_reg's
 * `used1 |= call_used_reg_set` when calls_crossed != 0 (global.c:967, :992-995)
 * bars r0-r4 and it reaches r5 -- and because the pseudo IS r5 rather than r0,
 * the argument needs the ROM's `mov r0, r5` and the return needs the ROM's
 * `mov r5, r0`.  Keeping the call inside each arm still blocks the
 * if-conversion (this park's own earlier finding, which stands) and
 * cross-jumping (jump.c:675, :1602, :1607) merges the two identical tails back
 * into one `bl`, leaving only the two `ldr r5, =K` differing.  The ROM's shape.
 *
 * THE RULE TO CARRY, more general than this function: WHERE THE ROM STAGES A
 * CALL ARGUMENT THROUGH A CALLEE-SAVED REGISTER THAT IT THEN REUSES FOR THAT
 * CALL'S RESULT, THE SOURCE HAS ONE VARIABLE, NOT TWO.  This park had tried
 * "a named `id` assigned in each arm first" -- a SECOND variable -- and the
 * dimension never varied was id and s being the same name.
 *
 * WHAT IS LEFT IS THREE ENCODINGS OF PURE ORDER.  The ROM has
 *
 *     bl Func_80b2884 / mov r1, #5 / mov r5, r0 / mov r0, r8 / bl _Func_8019908
 *
 * and this candidate has `mov r5, r0 / mov r0, r8 / mov r1, #5` -- the second
 * call's constant argument placed last instead of first.
 *
 * IT IS NOT THE SCHEDULER.  -fno-schedule-insns2 and -fno-schedule-insns both
 * leave the order BYTE-IDENTICAL, so nothing is being moved: this is EXPAND
 * order surviving untouched.  `.00.rtl` shows it -- insn 77
 * `(set (reg/v:SI 36) (reg:SI 0 r0))`, then the join `code_label 79`, then insn
 * 82 `(set (reg:SI 0 r0) (reg/v:SI 35))`, then insn 84
 * `(set (reg:SI 1 r1) (const_int 5))`.  load_register_parameters walks arguments
 * 0..n-1, so r0 is loaded before r1, and the preceding statement's result copy
 * is emitted before either.
 *
 * AND THE SCHEDULER COULD NOT HAVE DONE IT EVEN IF IT HAD RUN.
 * rank_for_schedule (haifa-sched.c) ties all three candidates: priority is
 * equal because insn_cost returns 0 for an anti-dependence, so
 * `mov r5,r0` -> `mov r0,r8` contributes nothing; INSN_REG_WEIGHT is gated on
 * `!reload_completed` and sched2 runs after reload; the last-scheduled-insn
 * classes are equal; and the dependent-count rung is one apiece.  The
 * comparison therefore falls through to `INSN_LUID (tmp) - INSN_LUID (tmp2)`,
 * i.e. EXPAND ORDER.  So no spelling of these two statements reorders them
 * while they remain two statements.  precompute_register_parameters
 * (calls.c:805, final clause) is the one place an argument CAN be materialised
 * ahead of the others, but its gate is `rtx_cost (value, SET) > 2` and
 * arm_rtx_costs (arm.c:2078-2081) returns 0 for a CONST_INT below 256 under
 * `outer == SET`, so a `5` can never take that path.  A constant at or above
 * 256 that is not thumb_shiftable_const costs COSTS_N_INSNS (3) and WOULD --
 * worth remembering for the next function of this shape.
 *
 * SCREENED, INERT at the same three encodings: naming the constant
 * (`n = 5; _Func_8019908(h, n);`) immediately before the call, and before the
 * whole if/else -- cse folds it back to the use both times.
 * WORSE: swapping the arms so the `== 0` test leads (five of 48); duplicating
 * _Func_8019908 into both arms (54 encodings, twenty-four differing);
 * duplicating the whole tail including _DrawSmallText (59 encodings, seventeen
 * differing); a separate `void *s` with a separate `int id` set in each arm (46
 * encodings, nine differing); `id` hoisted above the Func_80b27b0 call (50
 * encodings, eighteen differing).
 *
 * --- THE PRE-BATCH-333 RECORD FOLLOWS; ITS NUMBERS ARE SUPERSEDED ---
 *
 * Source asm: goldensun/asm/rom_b0000/rom_b0070_c_c_a_c.s
 * Best screen: forty-five instructions against the ROM's forty-six, and the missing one is a
 * single `mov r0, r5`. Everything before it and everything after it is right;
 * the nineteen differing positions are that one-instruction offset cascading.
 *
 * BLOCKER CLASS: gcc propagates a constant straight into the argument register
 * where the ROM stages it through a callee-saved one.
 *
 *     rom    ldr r5, =0xd2c / b .L2 / .L1: ldr r5, =0xd2d / .L2: mov r0, r5 / bl
 *     ours   ldr r0, =0xd2c / b .L2 / .L1: ldr r0, =0xd2d / .L2: bl
 *
 * A TWO-WAY CHOICE OF ADJACENT CONSTANTS -- AND HOW TO GET THE BRANCH BACK.
 * This is worth keeping even though the function did not close, because it is
 * the first time the branchless-nearby-constant blocker has been defeated.
 *
 * 0xd2c and 0xd2d differ by one, so the plain
 *
 *     if (cond) id = 0xd2c; else id = 0xd2d;
 *     s = Func_80b2884(id);
 *
 * gets if-converted: gcc emits the `neg / orr / lsr #31` boolean-normalise
 * idiom and subtracts, with no branch at all (twenty-six differing of
 * forty-six). Putting the CALL INSIDE EACH ARM --
 *
 *     if (cond) s = Func_80b2884(0xd2c); else s = Func_80b2884(0xd2d);
 *
 * -- blocks the if-conversion, because gcc will not speculate a call. It then
 * cross-jumps the two identical tails back into one `bl`, which is exactly the
 * ROM's shape: two pool loads, a `b`, and one shared call. That took it
 * from twenty-six differing to nineteen and made the whole control flow match.
 *
 * So the rule to carry: when the ROM branches over a choice of two constants
 * that gcc insists on making branchless, look for a CALL that can be moved
 * inside the arms. Cross-jumping puts it back.
 *
 * WHAT WAS TRIED AGAINST THE REMAINING INSTRUCTION, all identical at nineteen:
 *   - the call in each arm, with the constant inline
 *   - the call in each arm, with a named `id` assigned in each arm first
 *     (an attempt to make the cross-jumped tail `mov r0, id / bl`)
 *   - both of those with Func_80b2884's prototype withheld
 *   - withholding Func_80b2778's prototype (this also fixes an r0/r1 rotation
 *     earlier in the function, but the rotation is already right without it)
 *
 * Nothing puts the constant in a callee-saved register first. gcc coalesces the
 * constant's pseudo with r0 because nothing else wants it; the ROM's build did
 * not. That is the same allocator disagreement as
 * src/non_matching/ovl_7ed0a0/2009458.c, seen on a constant rather than on a
 * loaded byte.
 *
 * ===== BATCH 336 BRIEF D: THE PARK'S rank_for_schedule TIE ANALYSIS IS VOID --
 * ===== THE THREE INSNS ARE IN TWO DIFFERENT BASIC BLOCKS.  AND THE ROM'S
 * ===== ORDER IS WHAT sched2 PRODUCES ONCE THEY SHARE ONE.
 *
 * Figure re-derived first: **3 differing encodings of 48**, ref 48 / ours 48,
 * 37 instructions each side, first at index 29, relocations identical.  BODY
 * UNCHANGED.
 *
 * THE REFUTATION.  The section above says rank_for_schedule "ties all three
 * candidates ... The comparison therefore falls through to
 * `INSN_LUID (tmp) - INSN_LUID (tmp2)`, i.e. EXPAND ORDER.  So no spelling of
 * these two statements reorders them while they remain two statements."  Every
 * rung it names is reproduced, and the conclusion does not follow, because
 * **rank_for_schedule never compares these insns at all.**  `.23.sched2` on
 * this exact body:
 *
 *     ;; -- basic block 2 from 105 to 64 --   56 r5=0xd2c / 59 r0=r5 /
 *                                            60 call Func_80b2884 / 62 r5=r0 /
 *                                            64 pc=L79
 *     ;; -- basic block 3 from 66 to 77 --   71 r5=0xd2d / 74 r0=r5 /
 *                                            75 call Func_80b2884 / 77 r5=r0
 *     ;; -- basic block 4 from 79 to 107 --  Ready list (t = 0): 84 82
 *                                            0  82 r0=r8
 *                                            1  84 r1=0x5
 *
 * `mov r5, r0` is the LAST insn of EACH ARM BLOCK (insns 62 and 77).
 * `mov r0, r8` and `mov r1, #5` are the FIRST TWO insns of the JOIN BLOCK (82,
 * 84).  Scheduling is per basic block, so the only comparison sched2 ever makes
 * here is 82 against 84 -- and the ROM's order requires `mov r1, #5` to come
 * BEFORE `mov r5, r0`, i.e. to cross a block boundary, which no scheduler can
 * do.  **The park checked the arithmetic of a comparison that does not
 * happen.**  This is the batch-335 precondition failure in its sharpest form:
 * not readiness, not pass applicability, but *same-block membership*.
 *
 * The CFG is this way because cross-jumping runs at `.25.jump2`, AFTER
 * `.23.sched2`: `jump_optimize` is only called with cross_jump set in the final
 * jump pass, and `find_cross_jump` / `do_cross_jump` (jump.c:118-119, call
 * sites :605, :651-680) merge the arms' duplicated tails afterwards.  So the
 * single `bl Func_80b2884` in the output is a jump2 artefact of a CFG that was
 * still duplicated when the order was decided.
 *
 * AND THE ROM'S ORDER IS REACHABLE -- IT IS WHAT sched2 EMITS WHEN THE THREE
 * INSNS SHARE A BLOCK.  Duplicating the second call into both arms puts
 * `r5=r0`, `r0=r8` and `r1=5` in one block, and sched2 then produces the
 * reference's order VERBATIM in each arm:
 *
 *     mov r0, r5 / bl Func_80b2884 / mov r1, #0x5 / mov r5, r0 / mov r0, r8
 *       / bl _Func_8019908
 *
 * because `r1 = 5` hangs off the call by a REG_DEP_OUTPUT edge, which
 * `arm_adjust_cost` ZEROES (arm.c:2425-2427), while `r5 = r0` is a true
 * dependence on the call's result at cost 1.  So `r1 = 5` is ready a cycle
 * earlier and wins without any rung being tied.  **The ordering is not the
 * blocker and never was.**
 *
 * WHAT IS LEFT IS A CROSS-JUMP MERGE THAT DOES NOT HAPPEN.  That variant reads
 * 39 of 48 at dsize +16 because jump2 leaves BOTH copies of the six-insn tail:
 * the call count is 12 in `.23.sched2` and still 12 in `.25.jump2`.  By the
 * letter of `find_cross_jump` it should merge -- walking back from arm 1's
 * `b L2` and from L2, six insns match before `ldr r5,=0xd2c` meets
 * `ldr r5,=0xd2d`, `minimum` starts at 1 (jump.c:660) and the test is
 * `if (minimum <= 0 && last1 != 0 && last1 != e1)` -- and it does not.  **That
 * gap is the one open question on this function, and it is a question about
 * jump.c, not about C.**  Note the base body DOES get a three-insn merge at the
 * same site, so cross-jumping is not disabled and does not refuse calls as
 * such.
 *
 * MEASURED THIS ROUND, ref 48 unless noted:
 *   this body                                                       3
 *   `n = 5;` as the LAST statement of each arm, `_Func_8019908(h,n)` 3 (inert)
 *   `n = 5;` as the FIRST statement of each arm                      3 (inert)
 *     -- both inert because gcse's cprop is GLOBAL: both arms define n as the
 *        same const, so it is available at the join, folded back into the use,
 *        and the arm insns die.  The park's two positions (before the call,
 *        before the whole if/else) fail the same way; these are the two
 *        positions it had NOT tried, and they fail for the same reason.
 *   `_Func_8019908(h, 5)` duplicated into both arms                 39, +16
 *   the call with the constant inline, no `s =` in the arms         26, -4
 *   `id = 0xd2c; if (!cond) id = 0xd2d;` then the call at the join  49, +4
 *   `id = 0xd2d; if (cond) id = 0xd2c;` then the call at the join   49, +4
 *   the same two reusing `s` as the id                              33 / 34, -4
 *
 * The four IF-THEN rows are worth keeping as a negative with its mechanism.
 * They were tried because `noce_try_store_flag_constants` (ifcvt.c:584-624)
 * is gated on `GET_CODE (if_info->a) == CONST_INT && GET_CODE (if_info->b) ==
 * CONST_INT`, and in an IF-THEN the not-taken value is the REG itself, so the
 * gate fails and the branch survives WITH the call left in the join -- exactly
 * the CFG this residue needs.  It does not pay: the `ldr r5,=0xd2c` then hoists
 * above the `Func_80b27b0` call and everything downstream shifts.
 * (For the record on the IF-THEN-ELSE form the park measured at 26: `diff` is
 * +/-1 = +/-STORE_FLAG_VALUE, so `normalize = 0` at ifcvt.c:605 and the
 * conversion fires regardless of BRANCH_COST.  The only escape is one operand
 * not being a CONST_INT, which is the `_MSG_b24` argument and is SUFFICIENT,
 * NOT NECESSARY -- the branch is equally explained by the call being in the
 * arms -- so it fails the standard in docs/owner-decisions.md:17-44.)
 *
 * NEXT: not argument order and not the scheduler.  Either (a) find why
 * `find_cross_jump` declines the six-insn two-call tail when it accepts the
 * three-insn one-call tail at the same site, which would land this function
 * outright, or (b) treat the 3 as bounded on the two-basic-blocks fact above.
 * Do NOT re-propose a reordering of the three movs: they are not in one block.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 3 of 48, all three length dimensions EQUAL, so this
 *   figure IS a real distance.  FLAG AXIS: eight groups, none improves the
 *   aligned residue.  ANCHOR AXIS: 10 placements swept, but 8 WERE BIT-IDENTICAL to
 *   the base, so only 2 DISTINCT PROGRAMS were actually tested.
 *   Those 2 are inert; the other 8 measured nothing.  Nothing capped, nothing failed to compile.
 *   DO NOT RE-SCREEN.  The residue is a register choice, a tie or a pass-level
 *   effect, not a placement or flag question.  Read as "inert against THIS body".
  */
extern unsigned char *iwram_3001f2c;
extern void *Func_80b2778(int a, int b);
extern int Func_80b27b0(int a, int b);
extern void *Func_80b2884(int id);
extern void _Func_8016478(int a);
extern void _Func_8019908(void *h, int n);
extern void _DrawSmallText(void *s, int a, int b, int c);

void Func_80b2ed8(int a, int b)
{
    int lang;
    void *h;
    int s;
    lang = *(signed char *)(iwram_3001f2c + 0x3aa);
    h = Func_80b2778(b, lang);
    if (a != 0) {
        _Func_8016478(a);
        if (Func_80b27b0(b, lang)) {
            s = 0xd2c;
            s = (int)Func_80b2884(s);
        } else {
            s = 0xd2d;
            s = (int)Func_80b2884(s);
        }
        _Func_8019908(h, 5);
        _DrawSmallText((void *)s, a, 0, 0);
    }
}
