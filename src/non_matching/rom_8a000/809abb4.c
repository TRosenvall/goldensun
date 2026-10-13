/* ===== BATCH 336 (brief E) -- 1 OF 191 STREAM ENCODINGS.  BODY UNCHANGED. ====
 * RE-MEASURED: `XX ENCODINGS differ in 1 place(s) (ref 191, ours 191) ...
 * instructions are 159/159`, index 98, ref dbc9 (blt) against ours d1c9 (bne).
 * Still the closest park in the tree.  PIN-FREE, DEVICE-FREE.
 *
 * ONE NEW DIMENSION TRIED AND REFUTED, and it was the right one to try.
 * Every bound row below is a BODY ASSIGNMENT (`n = 11;` as a statement, in five
 * positions).  Batch 310c's landed finding -- recorded in
 * src/non_matching/rom_c9000/Anim_CriticalHit.c, and the lever that moved two
 * functions -- is that ONLY A DECLARATION INITIALISER makes a pseudo lose its
 * hard register, because allocno_compare's priority is
 * log2(n_refs)*freq/LIVE_LENGTH and a range starting at function entry is the
 * longest and lowest-priority; a body assignment keeps the range SHORT and
 * RAISES priority, "the exact opposite of what is wanted".  That is this park's
 * rung 4 exactly, and `int n = 11;` had never been tried here.  MEASURED:
 *     int n = 11; in place, `i < n`                164 insns / 159    178 RELOCDIFF
 *     int n = 11; FIRST in the declaration list    164 / 159          178 RELOCDIFF
 *     int n = 11; `n > i`                          164 / 159          178 RELOCDIFF
 *     int n = 0xb; `i < n`                         164 / 159          178 RELOCDIFF
 *     int n = 11; AND int i = 0; both initialised  164 / 159          178 RELOCDIFF
 *     int n = 11; `i <= n - 1`                     159 / 159            2 idx 97
 * THE LEVER DOES NOT TRANSFER.  Every `<` form costs exactly FIVE instructions
 * and changes the relocation sequence, so none is a distance -- read the
 * instruction column.  The bound is live across seven calls either way, which
 * is what the declaration initialiser cannot fix.
 * AND THE LAST ROW IS THE USEFUL ONE: 159/159, 2 differing, index 97
 * `ref 2f0b (cmp r7,#0xb)` against `ours 2f0a (cmp r7,#0xa)` -- the named bound
 * was propagated and `11 - 1` folded to 10, collapsing the row onto the literal
 * `<=` row.  So this is a DIRECT MEASUREMENT OF RUNG 5's PRICE: 164 insns when
 * the bound survives to the jump, 159 when it does not.  Five instructions is
 * what -ffixed-r11 avoids, and nothing cheaper exists.
 *
 * TWO COMPILER-LEVEL CORRECTIONS TO THE LATTICE.  Both change the REASON, not
 * the verdict, and the first closes rung 3 harder than the stated reason did.
 * (1) *** NEW ***  gcse.c:3682-3703 -- `cprop_jump` substitutes the constant,
 *     calls `simplify_ternary_operation` on the jump, and then
 *         if (temp == 0)
 *           return 0;
 *     so the replacement is ABANDONED unless the jump folds to a known branch.
 *     **gcse's global cprop can therefore NEVER put a constant into a LIVE
 *     conditional branch**, whatever the block structure.  The lattice says
 *     rung 3 holds because combine re-canonicalises; the stronger statement is
 *     that gcse cannot deliver the constant to the jump at all.  This also
 *     retires the CFG note below as the operative reason -- it is not cse1's
 *     block boundary that stops the fold, it is cprop_jump's gate.
 * (2) "else combine.c:10082 re-canonicalises it" needed a mechanism, because
 *     `simplify_comparison` is NOT a sweep over all comparisons -- it runs only
 *     inside a SUCCESSFUL `try_combine`.  The `i <= n - 1` row above supplies
 *     the mechanism: combine folds the CONSTANT-LOAD INSN into the compare,
 *     which is what produced `cmp r7,#0xa` there.  The claim is right; the
 *     route is combination, not canonicalisation.
 * Both switch arms re-read and confirmed verbatim: fold-const.c:6272-6294 (gate
 * `TREE_CODE (arg1) == INTEGER_CST && TREE_CODE (arg0) != INTEGER_CST &&
 * tree_int_cst_sgn (arg1) > 0`, cases GE_EXPR and LT_EXPR) and
 * combine.c:10138-10175 (LT -> LE whenever `const_op > 0`, unconditionally; the
 * reverse LE -> LT fires only for `const_op < 0`).  **So for a POSITIVE constant
 * the canonical RTL forms are LE and GT, and `(lt reg (const_int 11))` is
 * non-canonical -- it can only be created AFTER combine.**  That is why
 * -ffixed-r11 is byte-identical and why nothing else is.
 *
 * VERDICT UNCHANGED: park at 1 pin-free.  The owner's revisit condition ("the 1
 * turns out to be reachable another way") is NOT met.  Do not re-sweep literal
 * spellings, either side of the comparison, RTL flags at this index, or -- now
 * -- declaration-initialised bounds.
 * -- scratch_elev/b336/E
 * =============================================================================
 */
/* Field_Halt -- 1 differing encoding of 191.  PARKED, PIN-FREE, DEVICE-FREE.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/809abb4.c asm/rom_8a000/rom_9a44c_c_c_c.s --func Field_Halt
 *
 * ===== OWNER DECISION, 2026-10-04: per-file `-ffixed-r11` DECLINED. =====
 * This function is BYTE-IDENTICAL under a per-file FIXEDR11_CFLAGS rule and that
 * was refused; the counter-evidence is the landed twin Field_Whirlwind
 * (src/rom_8a000/rom_9a44c_c_c_a_a.c, byte-exact) using fp freely in this same
 * bank.  Full reasoning in docs/owner-decisions.md entry 2.  DO NOT RE-PROPOSE.
 * Its revisit condition was "or the 1 turns out to be reachable another way".
 * BATCH 327 ANSWERS THAT: NO.  The lattice is closed and every rung below is
 * either read in the compiler or measured in a dump.
 *
 * RE-DERIVED batch 327, brief F: 1 differing encoding of 191 (ref 191, ours 191),
 * 444 bytes against 444, 30 relocations identical.  `--whole` prints
 * `SIZE ref 456 bytes, ours 444`; that 12-byte gap is NOT text -- the reference
 * ends with `.section .rodata / .global .La012c / .La012c: .incrom 0xa012c,
 * 0xa0138`, exactly 12 bytes, and that blob is consumed by an ALREADY-LANDED
 * sibling (src/rom_8a000/rom_9a44c_a_a_a_c.c:111,
 * `extern struct Script *La012c[] __asm__(".La012c");`).  Whatever ships this
 * function must carry that rodata; it is not a defect in the body.
 *
 * ===================== THE RESIDUE, DECODED =====================
 * index 98 (the park said 97; off by one): ref `dbc9` against ours `d1c9`.
 * Thumb `1101 cccc iiiiiiii`: 0xdb is cond 0b1011 = LT -> `blt`; 0xd1 is cond
 * 0b0001 = NE -> `bne`.  The imm8 is 0xc9 = -55 in BOTH, so only the CONDITION
 * differs.  The `cmp r7, #0xb` at index 97 MATCHES (that is what the installed
 * `i != 11` bought).  The ROM is `cmp r7,#0xb` + `blt`; we emit `cmp r7,#0xb` +
 * `bne`.  ONE RUN, ONE CAUSE.
 *
 * ========== CORRECTION: THE REWRITE IS IN fold, NOT IN combine ==========
 * Both this park and an independent read of combine.c attributed `LT 11 -> LE 10`
 * to combine's `simplify_comparison`.  MEASURED, IT IS ALREADY DONE IN `.00.rtl`:
 * with the literal `i < 11`, jump_insn 101 of x.c.00.rtl reads
 *     (if_then_else (le (reg/v:SI 36) (const_int 10 [0xa])) (label_ref 104) (pc))
 * before jump, cse, gcse, loop, life or combine has run.  The site is
 *     fold-const.c:6269-6291, comment "Change X >= CST to X > (CST - 1) if CST
 *     is positive", whose switch also carries
 *       case LT_EXPR: code = LE_EXPR;
 *                     arg1 = const_binop (MINUS_EXPR, arg1, integer_one_node, 0);
 * gated by exactly
 *     TREE_CODE (arg1) == INTEGER_CST && TREE_CODE (arg0) != INTEGER_CST
 *     && tree_int_cst_sgn (arg1) > 0
 * It is a FRONT-END TREE FOLD.  **That is why this park's 12-toggle RTL flag sweep
 * found nothing at this index -- the sweep was looking in the wrong half of the
 * compiler.**  combine's `simplify_comparison` (combine.c:10140, inside
 * `while (GET_CODE (op1) == CONST_INT)` at :10082) performs the SAME rewrite and
 * is a second, redundant gate: it re-canonicalises the constant if any RTL pass
 * puts it back before combine.
 *
 * ================= THE CLOSED LATTICE, rung by rung =================
 * Target: `cmp r7,#0xb` + `blt` is ONE insn, `cbranchsi4` (arm.md:5152), whose
 * alternative 0 is operand1 "l" + operand2 "rI" -- so a `const_int 11` is legal
 * there and reload CAN put one back.
 *
 * 1. NO LITERAL SPELLING CAN WORK, and the operator space is now EXHAUSTED by
 *    the fold table rather than by sampling:
 *      X <  11  -> X <= 10   (fold-const.c:6286)        -> cmp #0xa + ble
 *      X <= 10  -> stable                               -> cmp #0xa + ble
 *      X >= 11  -> X > 10                               -> cmp #0xa
 *      X >= 12  -> X > 11  (GT survives; GT is not in that switch)
 *                                                       -> cmp #0xb + `ble` 0xdd
 *      11 >  X  -> fold swaps the constant right first  -> as X < 11
 *      X != 11  -> not in the switch at all             -> cmp #0xb + `bne` 0xd1
 *    `(lt X 11)` is UNREACHABLE as a tree.  The park's eleven literal spellings
 *    were therefore exhaustive over the wrong dimension; the installed `!=` is
 *    the best literal there is.
 * 2. SO op1 MUST BE A REGISTER THROUGH combine -- i.e. a named non-const bound.
 * 3. AND THE CONSTANT MUST RETURN AFTER combine, else :10082 re-canonicalises it.
 *    The only post-combine pass that can put a `const_int` into an operand is
 *    reload's `reg_equiv_constant` substitution, which needs
 *    `reg_renumber[bound] < 0`.
 *    - `update_equiv_regs` (local-alloc.c:667, called from :326) cannot do it.
 *      Its replace path needs `REG_N_REFS == 2` (set once, used once) and the
 *      loop weighting makes it THREE: all four REG_N_REFS sites in flow.c add
 *      `pbi->bb->loop_depth + 1`, so a bound SET at depth 0 (counts 1) and USED
 *      in the loop test at depth 1 (counts 2) is 3.  MEASURED, .17.lreg:206 of
 *      the named-bound variant: `Register 39 used 3 times across 134 insns`.
 *      (The 134 is 2 x 67 -- `update_equiv_regs`'s own `REG_LIVE_LENGTH *= 2`
 *      for a constant-equivalent pseudo, which is why the bound sorts LAST.)
 * 4. SO greg MUST DECLINE THE BOUND.  Measured in the named-bound variant:
 *      ;; 11 regs to allocate: 35 34 137 33 36 166 37 61 60 32 39   <- 39 last
 *      ;; 39 conflicts: ... 0 1 2 3 5 13 14
 *      ;; Register dispositions: ... 39 in 11 ...
 *      ;; Hard regs used:  0 1 2 3 5 6 7 8 9 10 11 14
 *      .17.lreg: `crosses 7 calls; pref LO_REGS`, costs
 *                `LO_REGS:0 HI_REGS:2 GENERAL_REGS:4 ALL_REGS:20 MEM:72`
 *    39 conflicts with the six allocnos holding r5..r10 (32->r9 33->r5 34->r6
 *    36->r7 61->r8 60->r10) and crosses calls, so `find_reg`'s `used1`
 *    (global.c:967) already holds call_used_reg_set + {r5..r10}: **r11 is the one
 *    remaining bit.**  find_reg DOES decline on the PREFERRED class, because
 *    `IOR_COMPL_HARD_REG_SET (used1, reg_class_contents[class])` with LO_REGS
 *    leaves nothing -- but global_alloc's alternate pass is UNCONDITIONAL
 *    (global.c:565: `if (reg_alternate_class (...) != NO_REGS) find_reg (..., 1,
 *    0, 0);`) and the bound's alternate class is ALL_REGS.  (.17.lreg prints
 *    `Register 39 pref LO_REGS` with no `, else` suffix precisely in
 *    regclass.c:1240-1245's `alt == ALL_REGS` branch.)  For that alternate class
 *    to be NO_REGS, regclass.c:1217-1232 needs EVERY non-LO class to cost >=
 *    mem_cost; HI_REGS costs 2 against MEM 72, and those costs come from
 *    `cbranchsi4`'s alternative 1 ("r","r"), which no C spelling removes.
 * 5. SO r11 MUST BE UNAVAILABLE.  Three routes, all costed:
 *    - `-ffixed-r11`: byte-identical.  OWNER DECLINED.
 *    - `frame_pointer_needed` puts HARD_FRAME_POINTER_REGNUM into
 *      `no_global_alloc_regs` -- but on thumb that register is **r7**, not r11
 *      (arm.h:898-899, THUMB_HARD_FRAME_POINTER_REGNUM 7).  Route does not exist.
 *    - a SEVENTH call-crossing allocno taking r11: it grows the push set.
 *      MEASURED: the named-bound body is **195 insns / 452 bytes against 191 /
 *      444**, and `Hard regs used` goes from `0 1 2 3 5 6 7 8 9 10 14` (this
 *      body -- exactly the ROM's six call-saved registers, r4 and r11 both
 *      absent) to `... 10 11 14`.  There is no zero-cost seventh.
 *
 * BOUND PLACEMENTS, re-confirmed (the park's figures are exact):
 *      bound literal, `!=` (this file)                1  (191 enc, 444 bytes)
 *      bound literal, any `<`/`<=` form               2  (191 enc)
 *      const int n = 11 (folded away)                 2  (191 enc)
 *      n = 11 where this file puts it               179  (195 enc, 452 bytes)
 *      n = 11 as the first statement                178  (195 enc)
 *      n = 11 after the `if (p == 0) return`        180  (195 enc)
 *      n = 11 immediately before the loop           180  (195 enc)
 *      unsigned n, `i < (int)n`                     179  (195 enc)
 * CFG fact worth keeping: the whole body after `if (p == 0) return;` is at a
 * LABEL (the ROM emits `cmp r6,#0 / bne .L9abea / b .L9ad52`), so the loop is in
 * a jumped-to block and cse1 cannot fold a bound assigned before that branch.
 * That is why naming the bound produces `blt` at all; the cost is purely greg's.
 *
 * DO NOT: re-sweep literal spellings (closed by the fold table above);
 * re-sweep RTL flags at this index (the decider is a front-end tree fold);
 * re-propose -ffixed-r11.
 *
 * ========= BATCH 330, BRIEF B: ONE NEW DIMENSION, MEASURED AND CLOSED =========
 * RE-DERIVED FIRST, and the park is exact: one differing encoding of 191, index
 * 98, ref dbc9 against ours d1c9, 191 encodings both sides.
 *
 * The eleven spellings above, and the fold table that replaced them, all share
 * one unexamined assumption: THE CONSTANT SITS ON THE RIGHT.  That is precisely
 * the gate of the rewrite -- fold-const.c:6269-6291 requires
 *     TREE_CODE (arg1) == INTEGER_CST && TREE_CODE (arg0) != INTEGER_CST
 *     && tree_int_cst_sgn (arg1) > 0
 * so putting the eleven into the LEFT-HAND EXPRESSION and comparing against zero
 * makes `tree_int_cst_sgn (arg1) > 0` FALSE and the rewrite never fires.  The
 * tree then really does keep a strict LT, which is the thing the park declared
 * unreachable.  NOBODY HAD VARIED THAT DIMENSION; I did, and it does not pay,
 * because gcc materialises the subtraction instead of folding it back into the
 * compare operand:
 *
 *     loop condition            instructions          figure
 *     i - 11 < 0                161 against 159         87
 *     i + (-11) < 0             161 against 159         87
 *     0 > i - 11                161 against 159         87
 *     !(i - 11 >= 0)            161 against 159         87
 *     i - 10 < 1                161 against 159         87
 *     i - 11 <= -1              163 against 159         88
 *     i - 11 != 0               191 against 191          1   folds back to != 11
 *     (i ^ 11) != 0             191 against 191          1   folds back to != 11
 *
 * Every row with the constant on the left is LONGER than the reference, so none
 * of them is a distance at all -- read the instruction column, not the figure
 * column.  The last two rows show fold collapsing the algebraic disguise back to
 * the installed inequality, which is the same `fold kills algebraic duplication`
 * result recorded elsewhere in the tree.
 *
 * SO THE CLOSED LATTICE IS NOW CLOSED ON ITS LAST OPEN SIDE TOO: the strict LT
 * is reachable as a tree after all, and reaching it costs two instructions.  The
 * park's conclusion survives; only its reason for rung one needed widening.
 * DO NOT re-sweep either side of the comparison again.
 * -- scratch_elev/b330/B
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 1 of 191, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: 51 placements swept, but 22 WERE BIT-IDENTICAL to
 *   the base, so only 29 DISTINCT PROGRAMS were actually tested.
 *   Those 29 are inert; the other 22 measured nothing.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
union blob { unsigned char *pp; short hh; int ii; };

extern int *iwram_3001f30;
extern unsigned char *CreateParticleActor(int a, int b, int c, int d);
extern void Func_8097384(void);
extern void _Actor_SetAnim(unsigned char *p, int n);
extern void WaitFrames(int n);
extern void _PlaySound(int id);
extern unsigned int Random(void);
extern void vec3_translate(int a, int b, int *v);
extern void Func_80974d8(int *v);
extern void Func_809ba90(unsigned char *e, int a, int b, int c);
extern void Func_809ba7c(unsigned char *e, void (*f)(unsigned char *));
extern void _Sprite_SetColorswap(void *s, int n);
extern void _DeleteActor(unsigned char *p);
extern void Func_809748c(void);
extern void Func_809aa98(unsigned char *e);

void Field_Halt(void)
{
    int v[3];
    int from[3];
    int to[3];
    int *o;
    int *t;
    unsigned char *p;
    unsigned char *ep;
    int i;
    int k;
    int w;
    int n;
    int c;
    int *vp;

    o = iwram_3001f30;
    t = (int *)o[4];
    o[2] = t[3];
    p = CreateParticleActor(0xfa, 0, 0, 0);
    i = 0;
    n = 11;
    _Actor_SetAnim(p, 0);
    if (p == 0)
        return;
    Func_8097384();
    from[0] = t[2];
    from[1] = t[3] + (0x80 << 13);
    from[2] = t[4];
    to[0] = o[1];
    to[1] = o[2] + (0x80 << 12);
    to[2] = o[3];
    for (; i != 11; i++) {
        *(int *)(p + 8) = from[0] + (to[0] - from[0]) * i / 10;
        *(int *)(p + 0xc) = from[1] + (to[1] - from[1]) * i / 10;
        *(int *)(p + 0x10) = from[2] + (to[2] - from[2]) * i / 10;
        c = 0xc0 << 8;
        w = c * i / 10 + (0x80 << 7);
        *(int *)(p + 0x18) = w;
        *(int *)(p + 0x1c) = w;
        WaitFrames(1);
    }
    WaitFrames(5);
    _Actor_SetAnim(p, 1);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6c);
    WaitFrames(10);
    _PlaySound(0x6d);
    ep = (unsigned char *)o + 0x58;
    for (k = 0; k < 16; k++) {
        v[0] = *(int *)(p + 8);
        v[1] = *(int *)(p + 0xc) + (0x80 << 12);
        v[2] = *(int *)(p + 0x10);
        Func_80974d8(v);
        vec3_translate(0x80 << 11, Random(), v);
        Func_809ba90(ep, 0x11d, v[0], v[2]);
        Func_809ba7c(ep, Func_809aa98);
        _Sprite_SetColorswap(*(void **)ep, 7);
        ep += 0x48;
    }
    v[0] = *(int *)(p + 8);
    v[1] = *(int *)(p + 0xc) + (0x80 << 12);
    v[2] = *(int *)(p + 0x10);
    WaitFrames(8);
    _DeleteActor(p);
    WaitFrames(4);
    WaitFrames(0x1e);
    Func_809748c();
}
