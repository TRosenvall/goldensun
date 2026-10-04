/* Func_80b6a60 (0x080b6a60) -- NON-MATCHING.
 *
 * NON-MATCHING, 5 of 59 encodings  (MEASURED, batch 323).  WAS 14 of 59.
 * Pin-free, device-free, no per-file flags.  The whole register rotation the
 * park named is SOLVED; what is left is two sched2 decisions.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80b6a60.c \
 *     asm/rom_b5000/rom_b5a0c_c_c_a_a_a_c_a.s --func Func_80b6a60
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
 * NEXT: residue B, by finding the decrement one unit of priority.  Residue A is
 * blocked by the hoist-last rule and its remedy is measured to cost more than
 * it buys.
 */
extern unsigned char *iwram_3001e74;
extern unsigned char gState[];
extern int _GetPartySize(void);
extern unsigned char *_GetUnit(int id);

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
            u[0x95 << 1] = 2;
        } while (i != 0);
    }
    if (out != 0)
        *out = 0xff;
    return n;
}
