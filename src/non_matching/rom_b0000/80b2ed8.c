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
