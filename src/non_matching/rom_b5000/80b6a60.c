/* Func_80b6a60 (0x080b6a60) -- NON-MATCHING.
 *
 * NON-MATCHING, **2 differing encodings of 59** (MEASURED batch 329 brief D on
 * THIS body: ref 59 / ours 59, 56 instructions each, first at index 25, SIZE /
 * INSTRUCTION COUNT / RELOCATIONS all silent on --func).  WAS 14, then 5; the
 * batch-329 section at the end of this header is the edit that took 5 -> 2 and
 * the body below carries it.
 * (`--whole` is uninformative here: the reference .s carries ~150 functions, so
 * compare per function.  Landing would need a split.)
 * Pin-free, device-free, shim-free, no per-file flags.  The whole register
 * rotation the park named is SOLVED, and so is residue B; what is left is ONE
 * sched2 decision, residue A, which is a closed-form LUID bound.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_b5000/80b6a60.c asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c_a.s --func Func_80b6a60
 *
 * PINS: 0.
 *
 * ---------------------------------------------------------------------------
 * WHAT CLOSED IT: TWO LEVERS FROM A LANDED SIBLING, CROSSED
 *
 * The park diagnosed "allocation ORDER -- the same rotation as
 * rom_15000/801a66c.c and rom_15000/8020150.c", which was RIGHT, and then said
 * "NEXT: nothing source-level", which was wrong.
 *
 * `Func_8020150` is not a park any more: it LANDED, at
 * src/rom_15000/rom_1fe2c_c_c_c.c, and its header carries the two levers that
 * fix this function.  Neither works alone:
 *
 *   A. THE WALKING INDEX, NOT THE WALKING POINTER.  `gState[k]` with `k++`
 *      instead of `*g` with `g++`.  gcc still strength-reduces the address into
 *      the same `add r2, #1` induction variable, so the emitted loop is
 *      unchanged -- only the allocno is.
 *   B. THE CONSTANT AS A LITERAL, NOT A NAMED LOCAL.  `u[0x95 << 1] = 2;`
 *      rather than `m = 2; ... = m;`.  Naming it gives it an allocno that
 *      competes for a callee-saved register; as a literal, loop-invariant
 *      motion hoists it on its own.
 *
 * MEASURED, and this is the point:
 *
 *     walker     constant    figure
 *     pointer    named       14  of 59   <- the installed park
 *     pointer    literal     15  of 59   <- WORSE
 *     index      named       10  of 59   <- better, still half the residue
 *     index      literal      5  of 59   <- crossed
 *
 * B alone is a REGRESSION and A alone leaves 10.  This is the fourth measured
 * shape in docs/elevation.md's "cross the lists" law -- an edit sitting in a
 * park's rejected-because-worse list that is HALF of a two-part fix -- and a
 * one-at-a-time sweep would have put `mlit` in the negatives and stopped.
 *
 * What the crossing buys is the whole rotation.  The park's table was:
 *
 *                   the party count   the gState walker
 *       rom         r7                r2
 *       the park    r2                r7
 *
 * With both levers all five long-lived values land on the ROM's registers:
 * `out` r5, the loop counter r6, the count r7, the hoisted 2 in r8, and the
 * walker in r2 -- caller-saved and therefore saved around the `_GetUnit` call
 * with the ROM's own `str r2,[sp] / bl / ldr r2,[sp]`.  Indices 14, 15, 17, 18,
 * 20, 23, 24, 27, 28 and 48 all close.
 *
 * ---------------------------------------------------------------------------
 * THE 5 THAT REMAIN -- two sched2 decisions, both with their rung named
 *
 * RESIDUE A, indices 25 and 26 (2 places).  The hoist and the counter init are
 * transposed:
 *
 *       rom    movs r3, #2 / mov r8, r3 / adds r6, r7, #0
 *       ours   movs r3, #2 / adds r6, r7, #0 / mov r8, r3
 *
 * `.23.sched2` for the preheader block, at the deciding cycle:
 *
 *     ;;  Ready list (t = 6):    174  53
 *     ;;      --> scheduling insn <<<53>>> on unit core
 *
 * insn 174 is `r8 = r3`, the second half of the invariant hoist; insn 53 is
 * `r6 = r7`, `i = n`.  Both carry prio 1, both are class 3 against the
 * last-scheduled `r3 = 0x2` (its cost is 1, and cost == 1 means class 3), and
 * both have zero in-block dependents.  rank_for_schedule therefore falls all
 * the way to its last rung, INSN_LUID, AND THE HOIST HAS THE HIGHER LUID
 * BECAUSE loop.c EMITS EVERY LOOP-INVARIANT HOIST LAST IN THE PREHEADER.  That
 * is exactly the batch-322 bound, and its documented remedy -- make the
 * invariant a statement whose position you choose -- is lever B INVERTED, so
 * it cannot be taken: naming the constant costs the rotation and reads 10.
 *
 * Measured, including the instrument:
 *   constant literal (hoisted)                             5
 *   constant named                                        10
 *   constant named AND pinned `register int m __asm__("r8")`  10
 *
 * so there is NO PINNED LANDING HIDING HERE EITHER -- the pin does not recover
 * the rotation, and pin-free is simply the best body.  Residue A is a genuine
 * conflict between two levers, not an untried spelling.
 *
 * RESIDUE B, indices 38-40 (3 places).  The counter decrement is late:
 *
 *       rom    subs r6, #1 / mov r1, r8 / strb r1, [r3]
 *       ours   mov r1, r8 / strb r1, [r3] / subs r6, #1
 *
 * This one is NOT a tie.  In the loop's call block the decrement (insn 88)
 * carries prio 2 and `r1 = r8` (insn 200) carries prio 3, so insn 200 wins on
 * the PRIORITY rung -- the first test in rank_for_schedule.  The decrement sits
 * in the ready list from cycle 0 to cycle 40, passed over every cycle:
 *
 *     ;;  Ready list (t = 37):    88  201 ... --> scheduling insn <<<200>>>
 *
 * prio(88) = prio(102) + 1 = 2, where 102 is the combined `cmp r6,#0 / bne`,
 * and the decrement's only dependent IS that branch.  prio(200) = 3 because it
 * feeds the `strb`.  Note the margin is exactly one: AT prio 3 the decrement
 * would WIN, because `r1 = r8` is anti-dependent on the just-scheduled
 * `adds r3, r0, r1` (class 2) while the decrement is independent (class 3).
 * So the lever to look for is anything that gives the decrement one more unit
 * of priority -- a second in-block dependent, or a cost-2 edge to the branch --
 * not another spelling of the decrement.
 *
 * ---------------------------------------------------------------------------
 * MEASURED INERT AND WORSE (all at the tree's flags, ref 59 encodings)
 *
 * 120-variant grid: {two-statement pointer, `*g++`, walking index} x {named
 * constant, literal} x {6 preheader initialisation orders} x {decrement
 * mid-body, decrement at the tail, `--i` in the exit test}:
 *
 *   index + literal, ALL 18 combinations of preheader order,
 *     decrement position and `--i`-in-test                 5 (all exactly inert)
 *   index + named constant, all 18                        10
 *   pointer + named, all 18                               14   <- the park
 *   pointer + literal, all 18                             15
 *   `*g++` + literal, all 18                              15
 *   ascending `for (i = 0; i < n; i++)`, either constant   46/47 + RELOCDIFF
 *   ascending `for (i = 0; i != n; i++)`, named constant   11
 *   ascending `for (i = 0; i != n; i++)`, literal          12
 *
 * The `<` versus `!=` gap at 47 against 11 is docs/elevation.md's operator rule
 * paying out again: combine's simplify_comparison rewrites `LT C>0` into
 * `LE C-1` so every `<` form collapses together, and `!=` is untouched.
 *
 * Round two, all on the 5-of-59 body, ALL EXACTLY INERT at 5:
 *   `*out++ = id;` for `*out = id; out++;`
 *   `*(u + (0x95 << 1)) = 2;` and `u[0x12a] = 2;` for `u[0x95 << 1] = 2;`
 *   `while (i)`, `while (--i != 0)`, `while (--i)`, `while (i-- != 1)`
 *   `--i;` for `i--;`, and the decrement at either position
 *   reusing `lim` -- dead after the clamp -- as the counter, which removes the
 *     `i = n` statement entirely and STILL does not move residue A
 * Worse: `while (i > 0)` reads 6 (it emits `bgt` where the ROM has `bne`).
 *
 * WHAT IS RIGHT: the `lim = 4; if (...) lim = 3;` clamp; the optional output
 * pointer tested twice; the pooled 0xff terminator halfword, which gcc pools
 * from a plain literal; the three-operand `add r2, r3, r1` for the table base,
 * which falls out of the two-statement index init once the allocation is right;
 * and the whole prologue/epilogue including the r8 save.
 *
 * ===== BATCH 326 BRIEF F: BOTH RESIDUES RE-READ OFF `.23.sched2` VERBATIM ====
 *
 * THE PARK'S EXPLANATION OF ITS OWN 10 IS REFUTED.  Its residue-A paragraph
 * says the remedy "is lever B INVERTED, so it cannot be taken: naming the
 * constant costs the rotation and reads 10".  The named-constant variant was
 * rebuilt and the whole stream read: **the register allocation is IDENTICAL to
 * this body** -- `out` r5, the counter r6, the count r7, the constant r8, the
 * walker r2, exactly the ROM's.  The 10 is (i) **seven preheader encodings that
 * are a pure ORDER PERMUTATION** -- the same seven instructions in the same
 * registers, with `mov r1,#2 / mov r8,r1` migrating to the FRONT of the
 * preheader and `mov r6,r7` landing before `add r2,r3,r1` -- plus (ii) the same
 * 3 of residue B, untouched.  So naming the constant does NOT cost the
 * rotation: it makes residue A worse (7 instead of 2) and leaves B alone.  What
 * cannot be taken is a 7-encoding preheader permutation, not a lost rotation.
 *
 * RESIDUE A, exact rungs (`.23.sched2`, the preheader block).  Dependence table
 * verbatim: `53` (`r6 = r7`) prio 1 cost 1, no dependents; `174` (`r8 = r3`)
 * prio 1 cost 1, no dependents; `194` (`r3 = 0x2`) prio 2, its only dependent
 * 174.  At `Ready list (t = 6): 174 53` **all four rungs of rank_for_schedule
 * tie** -- priority 1 = 1; class 3 = 3, because `insn_cost (194, link, X) == 1`
 * takes the `tmp_class = 3` escape; dependent count 0 = 0 -- and the function's
 * last line is `INSN_LUID (tmp) - INSN_LUID (tmp2)`, so the LOWER LUID wins and
 * LUID is simply position in the pre-sched stream.
 *
 * `53` is the ONLY original insn in that preheader: 165/203/204/172 (the gState
 * base) and 174 (the hoisted 2) are all created by loop.c and therefore land
 * after it.  **So residue A needs the counter initialisation to sit AFTER the
 * invariant hoist in the pre-sched stream, which means it must be created or
 * moved by something that runs after loop.c's `move_movables`** -- not a
 * different statement order, which is exactly why the park's six preheader
 * orders and the `lim`-as-counter form (which deletes the `i = n` statement
 * outright) were all inert.  loop.c emits movables immediately before
 * `loop_start`, and giv initialisations after them, so the one shape worth
 * trying is a counter that loop.c CREATES as a derived induction variable; the
 * only such spelling reachable from C here, `gState[(0xfc<<1) + n - i]`, reads
 * 14 and is already in the negatives.
 *
 * RESIDUE B, exact rungs -- AND A RUNG THE PARK NEVER MENTIONS.  Loop-block
 * (`b 8`) table verbatim: `88` (`sub r6,#1`) prio 2 cost 1, one dependent
 * (102); `200` (`mov r1,r8`) prio 3; `98` (`strb`) prio 2, dependents 102 and
 * 188; `188` (`ldr r2,[sp]`) prio 1; `102` (the combined `cmp r6,#0 / bne`)
 * prio 1.  The decrement gets **two** chances and loses both:
 *
 *   * **t = 37**, ready `88 200`: loses on PRIORITY, 2 against 3.  (The park has
 *     this one and its "the margin is exactly one" is right.)
 *   * **t = 38**, ready `88 98`: priority TIES at 2 and class TIES at 3, and it
 *     loses on the **DEPENDENT-COUNT rung** -- `98` has two in-block dependents
 *     (102, 188) against the decrement's one.  The park does not mention this
 *     rung at all, so "give the decrement one more unit of priority" would have
 *     won t = 37 and then lost t = 38 on a rung nobody had looked at.
 *
 * AND BOTH OF THE PARK'S TWO NAMED ROUTES ARE CLOSED, with their evidence:
 *
 *   * "a cost-2 edge to the branch".  `insn_cost`'s only target hook is
 *     `ADJUST_COST` -> `arm_adjust_cost` (**arm.c:2416-2453**), and every branch
 *     of it either returns 0 (REG_DEP_ANTI / REG_DEP_OUTPUT, :2425-2427),
 *     returns 1 (a call, or a load from the stack or the constant pool) or
 *     passes `cost` through unchanged -- **it never raises a cost.**  The base
 *     latency comes from the `core` function unit (**arm.md:253-264**) where
 *     only `load` (and `store1` when `ldsched` is not yes) has ready-delay 2;
 *     a register-to-register `subs` is `single`, delay 1.  So the 88 -> 102 edge
 *     is 1 and cannot be 2 unless the decrement becomes a memory load, i.e.
 *     unless `i` is spilled.
 *   * "a second in-block dependent" of the decrement requires a SECOND USE of
 *     `i` inside the loop.  The ROM's body has none, and the only spelling that
 *     manufactures one -- deriving the gState index from `i` -- is the measured
 *     14.  `priority()` is `max` over in-block dependents of
 *     `insn_cost + priority(dep)`, so a second dependent only helps if it
 *     itself has priority 2.
 *   * the remaining alternative, dropping prio(200) from 3 to 2 so the
 *     priorities tie and the CLASS rung hands it to the decrement (200 is
 *     anti-dependent on the just-scheduled `adds r3, r0, r1`, class 2, against
 *     the decrement's class 3), needs prio(98) = 1, i.e. the `strb` with NO
 *     in-block dependents.  It has two: the 188 edge is closed by the
 *     documented alias bound (char-precision store, `lang_get_alias_set`
 *     returns 0, `DIFFERENT_ALIAS_SETS_P` can never fire against a reload spill
 *     slot) and the 102 edge is the block-ending branch.
 *
 * MEASURED BATCH 326, ref 59 / ours 59, 56 instructions on every row, nothing
 * below 5: base **5**; `lim` reused as the counter **5**; `while (i != 0)`
 * **5**; `k` initialised after `i = n` **5**; `i = n` hoisted above the
 * `if (n > 0)` guard **12**; descending index `gState[(0xfc<<1) + n - i]`
 * **14**; `for (i = 0; i != n; i++)` **17**; ascending do-while
 * `while (i != n)` **19**; ascending with the index folded into the subscript
 * **19**; `n` itself as the counter with a saved return value **52 with
 * COUNT + RELOCDIFF + MEM** (it loses the `str`/`ldr` spill pair entirely);
 * the named constant **10** (composition above).
 *
 * NEXT: residue B has no route left that this project can state -- all three of
 * its rungs are closed above with citations, so treat it as bounded rather than
 * untried, and report any disagreement with the cited lines rather than with
 * another spelling.  Residue A's one live shape is a counter that loop.c
 * CREATES after its own movables (a giv initialisation), because loop.c emits
 * movables immediately before `loop_start` and giv inits after them.
 *

 * ===== BATCH 327 BRIEF H: RESIDUE A IS A CLOSED-FORM LUID BOUND =====
 *
 * Figure re-derived: **5 differing encodings of 59**, ref 59 / ours 59, first at
 * index 25, SIZE / INSTRUCTION COUNT / RELOCATIONS silent.  BODY UNCHANGED.
 *
 * The reference preheader is
 *   ldr r3,=gState / mov r1,#0xfc / lsl r1,#1 / add r2,r3,r1   (the giv base)
 *   mov r3,#2 / mov r8,r3                                      (the hoist)
 *   mov r6,r7                                                  (i = n)
 * and we transpose only the last two.  Brief F showed all four rungs of
 * `rank_for_schedule` tie at `Ready list (t = 6): 174 53` so the last line,
 * `INSN_LUID (tmp) - INSN_LUID (tmp2)`, decides and the LOWER LUID wins.  What
 * was missing was why no source order can move it:
 *
 * > **EVERY `move_movables` EMISSION SITE IN loop.c USES
 * > `emit_insn_before (..., loop_start)`** -- loop.c:1825, :1890, :1982, :1991,
 * > :2024, :2028, :2047, :2055 -- so a hoisted insn is ALWAYS placed immediately
 * > before NOTE_INSN_LOOP_BEG, i.e. **after every original statement of the
 * > preheader.**  `i = n` is an original statement, therefore
 * > LUID(i = n) < LUID(hoist) unconditionally.
 *
 * So residue A is not an untried spelling, it is an ordering loop.c fixes, and
 * there are exactly two escapes -- both already measured:
 *   (a) the hoist stops being loop.c-created.  Naming the constant makes its
 *       init an ORIGINAL insn with the LOWEST LUID of all, so it migrates to the
 *       FRONT of the preheader, ahead of the giv base too: the recorded **10**,
 *       of which 7 are exactly that permutation.
 *   (b) the counter init stops being original, i.e. loop.c must create it after
 *       its own movables (a giv initialisation).  The only C spelling,
 *       `gState[(0xfc<<1) + n - i]`, is the recorded **14**.
 * Residue B is unchanged and remains closed on brief F's three citations.
 * **Report disagreement with loop.c's emission sites, not with another spelling.**
 
 * ===== BATCH 329 BRIEF D: RESIDUE B IS CLOSED.  5 -> 2 OF 59.
 * ===== THE PARK'S ALIAS BOUND WAS WRONG IN BOTH OF ITS HALVES
 *
 * Figure re-derived first: **5 differing encodings of 59**, ref 59 / ours 59,
 * first at index 25.  THEN the body changed: residue B, the 3 encodings at
 * indices 38-40, is GONE.  New figure **2 differing encodings of 59**, ref 59 /
 * ours 59, first at index 25, SIZE / INSTRUCTION COUNT / RELOCATIONS silent.
 * Still PIN-FREE, SHIM-FREE, FLAG-FREE, DEVICE-FREE.
 *
 * THE EDIT IS ONE LINE: the byte store is a STRUCT COMPONENT instead of a
 * pointer subscript.
 *     was   u[0x95 << 1] = 2;
 *     now   ((struct Bc *)(u + (0x95 << 1)))->b = 2;      struct Bc { unsigned char b; };
 * and the loop now emits the reference's `add r3, r0, r1 / sub r6, #1 /
 * mov r1, r8 / strb r1, [r3] / ldr r2, [sp]`.
 *
 * WHY, AND IT IS NOT THE ALIAS SET.  The park closed residue B's third route on
 * *"the 188 edge is closed by the documented alias bound (char-precision store,
 * lang_get_alias_set returns 0, DIFFERENT_ALIAS_SETS_P can never fire against a
 * reload spill slot)"*.  Both halves are wrong:
 *
 *   1. A RELOAD SPILL SLOT DOES CARRY A FRESH NONZERO ALIAS SET --
 *      `MEM_ALIAS_SET (x) = new_alias_set ();` at **reload1.c:1918** and again
 *      at **:1954** (with :1952 inheriting an existing slot's set).  So the
 *      spill-slot side of the comparison was never the obstacle.
 *   2. `DIFFERENT_ALIAS_SETS_P` IS NOT THE ONLY DISJOINTNESS TEST.
 *      `true_dependence` (**alias.c:1559**) and `write_dependence_p`
 *      (**:1658**) both consult **`fixed_scalar_and_varying_struct_p`
 *      (alias.c:1521-1543)**, which returns non-NULL -- the two MEMs can never
 *      alias -- when one is `MEM_SCALAR_P` at a NON-varying address and the
 *      other is `MEM_IN_STRUCT_P` at a VARYING one.  The spill slot is a scalar
 *      at a fixed sp-relative address; the unit byte is reached through the
 *      `_GetUnit` result, which varies.  **The test is on MEM_IN_STRUCT_P, which
 *      the TYPE CONSTRUCTOR of the lvalue sets, and it is reached with the
 *      field still `unsigned char` and its alias set still 0.**
 *
 * The store -> `ldr r2, [sp]` edge therefore disappears, the `strb`'s in-block
 * dependents drop from two to one, and at the park's `t = 38` the decrement no
 * longer loses the dependent-count rung (haifa-sched.c:4097-4108).  Everything
 * the park said about the RUNGS was right; what it had wrong was that one of
 * the two edges feeding them is reachable from C.
 *
 * MEASURED, AND THE TYPE CONSTRUCTOR IS THE WHOLE VARIABLE (ref 59, dsize 0):
 *   u[0x95 << 1] = 2;                                  5   <- the old park
 *   *(u + (0x95 << 1)) = 2;                            5
 *   u[0x12a] = 2;                                      5
 *   *(unsigned char *)(u + (0x95 << 1)) = 2;           5
 *   a named `unsigned char *w` then `*w = 2;`          5
 *   ((struct Bc *)(u + (0x95 << 1)))->b = 2;           2   <- shipped
 *   the same with 0x12a, or via a named struct pointer  2
 *   struct { unsigned short b:8; } / { unsigned int b:8; }  2
 *   ((struct Bc *)u)[0x95 << 1].b = 2;                 36 with RELOCDIFF
 * so it is MEM_IN_STRUCT_P and not the alias set, not the address spelling and
 * not the field width.
 *
 * AND THE PARK'S ELEVEN "EXACTLY INERT AT 5" ROWS WERE RE-CROSSED AGAINST IT --
 * all ELEVEN are still exactly inert, now at 2, so none of them was a missing
 * prerequisite and residue A is independent of every one: `k` initialised after
 * `i = n`; `while (i)`; `--i != 0`; `--i`; `i-- != 1`; `--i;` as the step; the
 * decrement at the tail; `*out++ = id;`; `lim` reused as the counter.  Worse,
 * now re-measured on this body: `while (i > 0)` **3**; the named constant
 * **7** (was 10); the pointer walker **41 with RELOCDIFF**; the store through
 * `u` with no offset **36 with RELOCDIFF**.
 *
 * RESIDUE A IS UNCHANGED AND ITS THIRD ROUTE IS NOW ALSO REFUTED.  The
 * remaining 2 are still indices 25/26, the hoist and `i = n` transposed, and
 * still the LUID bound: loop.c emits every movable with
 * `emit_insn_before (..., loop_start)` so LUID(i = n) < LUID(hoist)
 * unconditionally.  Escape (b) -- "the counter init must be created by loop.c
 * after its own movables" -- has a mechanism the park never named and it does
 * not fire: **`check_dbra_loop` (loop.c:7748), called from `strength_reduce` at
 * loop.c:4408 and therefore AFTER `move_movables`, emits its reversed counter
 * init with `emit_insn_before (gen_move_insn (reg, start_value), loop_start)`
 * at loop.c:8154**, with the CONST_INT-initial-value branch at :8157-8178
 * emitting `add reg, comparison_value, offset` -- exactly the shape and exactly
 * the LUID position residue A needs.  MEASURED: gcc does NOT reverse this loop.
 * Seven ascending bodies (`i != n` / `i < n` / `++i` / `++i != n` in the test /
 * `i++` before the call / a `while` head), all **17-20, all first = 9**: the
 * counter stays ascending (`mov r2, #0` in the preheader) AND the ascending
 * shape loses the earlier `lim`/`n` rotation, emitting `mov r7, #4` / `mov r6,
 * r0` where the reference has `mov r6, #4` / `mov r7, r0`.
 *
 * NEXT: residue A only, and only escape (b) -- some construct that makes the
 * counter initialisation a `strength_reduce` creation.  Loop reversal is now
 * measured dead; a derived induction variable is the recorded 14.  Do NOT
 * re-propose residue B: it is closed, in the body.
 */
extern unsigned char *iwram_3001e74;
extern unsigned char gState[];
extern int _GetPartySize(void);
extern unsigned char *_GetUnit(int id);

struct Bc { unsigned char b; };

int Func_80b6a60(unsigned short *out)
{
    unsigned char *b;
    unsigned char *u;
    int k;
    int lim;
    int n;
    int i;
    int id;

    b = iwram_3001e74;
    lim = 4;
    if (*(b + 0x44) != 0)
        lim = 3;
    n = _GetPartySize();
    if (n > lim)
        n = lim;
    if (n > 0) {
        k = 0xfc << 1;
        i = n;
        do {
            id = gState[k];
            k++;
            if (out != 0) {
                *out = id;
                out++;
            }
            u = _GetUnit(id);
            i--;
            ((struct Bc *)(u + (0x95 << 1)))->b = 2;
        } while (i != 0);
    }
    if (out != 0)
        *out = 0xff;
    return n;
}
