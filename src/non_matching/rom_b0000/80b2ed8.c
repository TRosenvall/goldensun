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
