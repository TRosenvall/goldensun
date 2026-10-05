/* Func_807a0f4 -- 0x0807a0f4, asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_a.s
 * (1 function, no data section, so landing needs NO split).
 *
 * STILL NON-MATCHING.  PARKED AT 7 differing encodings of 88 -- RE-MEASURED
 * AGAIN in batch 325 and unchanged: 7 differing encodings of 88, ref 88 against
 * ours 88, 92 lines against 92, SIZE EQUAL, RELOCATIONS IDENTICAL (objcmp prints
 * no RELOCATIONS line).  Body below is the park's body, unchanged.  PINS: 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/807a0f4.c asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_a.s --func Func_807a0f4
 *
 * ---------------------------------------------------------------------------
 * WHAT I REPRODUCED, AND WHAT I REFUTED.
 *
 * REPRODUCED, all of it, by reading `.18.greg` and `.17.lreg` directly:
 *   - the whole residue is the r5/r6 contest between pseudo 79 (the
 *     strength-reduced gState byte cursor) and pseudo 40 (the outer
 *     check_dbra_loop down-counter).  Every other register matches.
 *   - it IS global-alloc's decision: `;; 12 regs to allocate: 38 37 42 41 39 79
 *     40 32 36 33 35 34`, so `allocno_compare` (global.c:607) applies, and it
 *     DOES carry floor_log2.  (For the record, so does local-alloc's
 *     QTY_CMP_PRI at local-alloc.c:1496; the real difference between the two
 *     formulae is the DENOMINATOR -- `death - birth` against `live_length`.
 *     Nothing here rests on that, because this is a global-alloc decision, but
 *     a brief circulating the "local-alloc has no floor_log2" claim is wrong
 *     and should not be inherited.)
 *   - the reference counts.  Counted by hand in `.17.lreg`:
 *       pseudo 79: def (insn 286) + two `zero_extend(mem:QI (reg 79))` uses
 *                  + `(set 79 (plus 79 1))` counting twice   = 5
 *       pseudo 40: def (insn 275) + `(set 40 (plus 40 ...))` twice
 *                  + `(ne (reg 40) ...)`                      = 4
 *   - 79 is allocated first and `find_reg` hands it r5 (r0-r3 conflict across
 *     the call, r4 is -fcall-used-r4), so 40 gets r6.  The ROM is the other way.
 *
 * REFUTED: the park's stated route out.  It wrote
 *
 *   "At 4 against 4 the tie breaks by allocno number, 40 < 79 takes r5, and
 *    that IS the ROM.  So batch 272's declaration-order lever would apply if
 *    the counts were equal."
 *
 * EQUAL COUNTS DO NOT TIE, because `allocno_compare` divides by live_length and
 * the two live ranges are NOT equal.  `.19.flow2` has the counter's init
 * (insn 275) BEFORE the cursor's init (insn 286) in the insn chain, with insn
 * 315 between them, so len40 = len79 + 2.  Then:
 *     refs 4 and 4:  pri40 = 32/(L+2)  <  pri79 = 32/L      -> 79 still first
 *     refs 5 and 5:  pri40 = 40/(L+2)  <  pri79 = 40/L      -> 79 still first
 * There is no tie to break, at any equal reference count, while the counter's
 * init is the earlier of the two.  The declaration-order lever cannot reach it.
 *
 * AND THE REFS-ONLY ROUTE IS CLOSED FROM THE OTHER SIDE TOO.  Giving the
 * counter a fifth reference is not available: `check_dbra_loop` (loop.c, the
 * `else` arm at the `initial_value == const0_rtx && comparison_value CONST_INT`
 * test) requires `no_use_except_counting` whenever the comparison value is NOT
 * a constant -- and `size` is `GetPartySize()`.  ANY extra mention of `i`
 * therefore kills the reversal, which is measured: `for (i = 0; i != size; i++)`
 * reads 70 at 92 instructions and `i = 0; if (size > i) { do ... }` reads 75.
 *
 * SO THE FLIP NEEDS TWO THINGS AT ONCE, and that is the sharpened map:
 *   (A) the cursor's init EARLIER than the counter's in the insn chain, so
 *       len79 > len40 -- which is also visible in the output, since the ROM
 *       emits `adds r6,r3,r2` (cursor) at index 32 and `adds r5,r0,#0`
 *       (counter) at index 33, while we emit them the other way round; AND
 *   (B) equal reference counts, which given (A) then favours 40.
 * With refs 5 against 4 and (A) in place the arithmetic is still 32/L against
 * 40/(L+2), which needs L < 8 to flip and L is about 30.  So (A) alone is not
 * enough, and neither is (B) alone.  NOT A BOUND -- the evidence is the two
 * formulae, the two reference counts and the chain order above.
 *
 * MEASURED AND SCREENED ON `.18.greg`'s ALLOCATION ORDER, not on the figure
 * (docs/elevation.md's correction 4).  All leave the order `... 39 79 40 ...`
 * with `79 in 5, 40 in 6`, i.e. exactly 7:
 *   `while` form with the increment at the bottom; `++i` for `i++`; `base` as
 *   `int` rather than `unsigned int`; the inner loop as a bottom-tested
 *   do/while with `k--` in the body; `k = gState[...]` as a named index.
 * CHANGES THE ORDER but breaks the program: `i` unsigned 70 (order becomes
 *   `41 40 79 39`, `40 in 1`); `i != size` 70 (`40 in 4`); the outer loop as a
 *   `goto` loop -- the park's own suggested next move -- 83 at 92 instructions,
 *   because suppressing loop.c suppresses the strength reduction the ROM needs.
 * FLAG INSTRUMENTS: -fno-regmove and -fno-rerun-cse-after-loop exactly inert
 *   at 7; -fno-rerun-loop-opt changes the SIZE (every relocation offset moves),
 *   so the two-pass loop structure is load-bearing but removing it is not the
 *   answer.
 *
 * DECLARATION DIVIDEND, no figure change: GetUnit's real definition is
 * `void *GetUnit(unsigned int id)` (src/rom_77000/rom_77320_a_a_c_c_a_b.c) and
 * GiveDjinni's is `int GiveDjinni(int id, int elem, int bit)`
 * (src/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_b.c); Func_807a458 really does
 * take three arguments (asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_c_a_a.s,
 * r0/r1/r2 all copied on entry).  The park's int-returning declarations for
 * GiveDjinni and Func_807a458 are load-bearing for the argument fill order and
 * are kept.
 *
 * ===========================================================================
 * BATCH 325.  THE PARK SURVIVES.  THREE CORRECTIONS TO ITS ARITHMETIC, ONE OF
 * WHICH MAKES ITS BOUND HARDER, AND ONE STRUCTURAL SHARPENING.
 *
 * Everything the park REPRODUCED, I reproduced again: 7 of 88 at equal size and
 * identical relocations; the whole residue is the r5/r6 contest between the
 * strength-reduced gState byte cursor (pseudo 79) and the check_dbra_loop
 * down-counter (pseudo 40); `;; 12 regs to allocate: 38 37 42 41 39 79 40 32 36
 * 33 35 34` and `;; Register dispositions: ... 40 in 6 ... 79 in 5`, so it IS
 * global-alloc's ordering; the ROM wants 40 in r5 and 79 in r6.
 *
 * CORRECTION 1 -- THE REFERENCE COUNTS THE FORMULA DIVIDES ARE NOT RAW COUNTS.
 * `allocno_compare` divides `allocno[v].n_refs`, and global.c:449 accumulates
 * that from `REG_N_REFS`, which flow.c:4948 increments by
 * `(optimize_size ? 1 : pbi->bb->loop_depth + 1)` PER REFERENCE -- so at -O2
 * every reference in the outer loop body counts TWO.  The park's "pseudo 79: 5,
 * pseudo 40: 4" are the raw counts.  The weighted ones are
 *     79: def (depth 0) 1 + two reads (depth 1) 2+2 + increment set&use 2+2 = 9
 *     40: def (depth 0) 1 + `sub` set&use 2+2 + the `ne` compare 2         = 7
 * and `floor_log2(9)*9 = 27` against `floor_log2(7)*7 = 14`, not 10 against 8.
 * SO THE PARK'S BOUND IS HARDER THAN IT WROTE, not softer: with (A) in place
 * (len79 = len40 + 2) the inequality for 40-first becomes 14/len40 > 27/len79,
 * i.e. **len79 > 1.93 * len40**, against the park's 1.25.  Both pseudos live
 * across the same loop, so that ratio is not reachable by moving one init.
 *
 * CORRECTION 2 -- THE DIVISION IS DOUBLE, NOT INTEGER (global.c:605-612): the
 * `(double)` cast is on the numerator and the truncation to int happens only
 * after `* 10000 * size`.  Nothing in the park turns on this, but a reader
 * doing the arithmetic in C ints gets 0 == 0 for both allocnos and would
 * conclude, wrongly, that they always tie.
 *
 * CORRECTION 3 -- THE TIE-BREAK IS ON THE ALLOCNO INDEX, NOT THE PSEUDO NUMBER.
 * `allocno_compare` ends `return v1 - v2` (global.c:617) where v1/v2 are
 * allocno indices; global_alloc numbers allocnos in ascending pseudo order, so
 * allocno(40) < allocno(79) and a tie DOES go to 40.  The park's target (B) --
 * "make the priorities equal and 40 takes r5" -- is therefore the right target.
 * What (B) needs is EQUAL WEIGHTED counts, and by correction 1 that means
 * moving 9 to 7 or 7 to 9, i.e. adding or removing a whole outer-loop
 * reference, which the park already closed from both sides: the cursor is read
 * twice in the ROM (`ldrb r0,[r6]` and `ldrb r3,[r6]`) so neither read can go,
 * and any extra mention of the counter kills check_dbra_loop's reversal.
 *
 * STRUCTURAL SHARPENING -- (A) AND THE REGISTER SWAP ARE ONE PHENOMENON, SO (A)
 * IS NOT SEPARATELY PURCHASABLE.  The park lists the init order and the
 * register assignment as two things that must both be got.  `.23.sched2`'s
 * visualization for the preheader block says they are the same thing:
 *
 *     ;; block 3   0  253  r3=[`*.LC0']
 *     ;;           2  315  r2=0xfc
 *     ;;           3  316  r2=r2<<0x1
 *     ;;           4  275  r6=r0        <- the counter init
 *     ;;           5  286  r5=r3+r2     <- the cursor init
 *
 * Both 275 and 286 are ready at clock 4 and both have priority 1, so
 * rank_for_schedule decides on a rung below priority and 275 wins with the
 * smaller INSN_LUID.  The LUID order is the chain order, which is also what
 * decides the live lengths, which is what `allocno_compare` divides by.  In
 * BOTH streams the init that comes FIRST in the chain takes r6 and the second
 * takes r5 -- ours has the counter first and the ROM has the cursor first.  So
 * a single edit that flips the chain order flips the emitted order AND the two
 * registers together; there is no separate (A) to buy.  Stated as evidence, not
 * as a claim: two streams is a small sample, and correction 1's arithmetic says
 * the length change alone does not reach the priority flip.
 *
 * MEASURED THIS BATCH, all screened on `.18.greg` and not on the figure:
 *   EXACTLY INERT AT 7, allocation order and dispositions BIT-IDENTICAL (so the
 *   edit never reached the allocator, which is not the same as the lever being
 *   inert):
 *     `base = 0xfc * 2;` moved BEFORE the GetPartySize call
 *     `base` as `int` rather than `unsigned int`
 *     `size` declared last in its declaration list (batch 272's lever)
 *   WORSE, and both still `79 in 5 / 40 in 6`:
 *     `size > i` instead of `i < size`                9 of 88, first diff 27
 *     the gState base named in a local `g`           10 of 88, first diff 27
 *   WORSE AND A DIFFERENT PROGRAM -- these SUPPRESS the strength reduction the
 *   ROM needs, and come out SHORTER than the reference, so their figures are
 *   measuring misalignment:
 *     an explicit `unsigned char *p` walked with `p++` in the for-step
 *                                                  63 of 88, 8 bytes SHORT
 *     the same with the init before GetPartySize    64 of 88, 8 bytes SHORT
 *     the same with `p[i]` indexing                 62 of 88, 4 bytes SHORT
 *     the same with `p++` at the bottom of the body 64 of 88, 8 bytes SHORT
 *     the `0xfc * 2` pasted in as a literal, no `base`
 *                                                   62 of 88, 8 bytes SHORT
 *     `unsigned int i` with `i < (unsigned)size`    70 of 88, 8 bytes LONG
 *
 * NEXT MOVE, with its price: the only remaining purchase is (B), equal WEIGHTED
 * reference counts, and by correction 1 that is a whole outer-loop reference,
 * not a spelling.  The one thing not yet swept is the loop-depth weighting
 * itself: a reference moved from the outer loop body to a DEEPER block counts
 * three instead of two, so a shape in which one of the cursor's two reads sits
 * inside the inner k-loop would make 79's weighted count 10 (floor_log2 3, 30)
 * and one in which a counter reference sits there would make 40's 9
 * (floor_log2 3, 27).  Neither is obviously honest C for this function; it is
 * recorded as the only unswept direction, not as a recommendation.
 *
 * CLASSIFIED AGAINST THE BATCH-325 RELOAD-CURSOR TRIAGE RULE, because this park
 * IS a coherent two-register rotation and that is exactly the shape the rule is
 * for.  `.18.greg` for this function prints
 *
 *     Spilling for insn 24.            <- no `Using reg` at all
 *     Spilling for insn 27.   Using reg 3 for reload 0
 *     Spilling for insn 30.   Using reg 3 for reload 0
 *     Spilling for insn 286.  Using reg 2 for reload 0
 *     Spilling for insn 81.   Using reg 2 for reload 0
 *     Spilling for insn 95.   Using reg 3 for reload 0
 *     Spilling for insn 147.  Using reg 3 for reload 0
 *     Spilling for insn 168.  Using reg 3 for reload 0
 *
 * -- ONE `Using reg` per `Spilling for insn` block, and every one of them is
 * r2 or r3, the scratch for a pool load.  `"Using reg %d for reload %d"` occurs
 * once in reload1.c, at :1664 inside `find_reg` (:1588), so those lines are
 * find_reg's decisions and NOT `allocate_reload_reg`'s round-robin cursor; with
 * one reload per insn the cursor has no freedom anyway, because
 * `choose_reload_regs_init` (reload1.c:5129) sets `reload_reg_unavailable` to
 * the complement of `chain->used_spill_regs`.
 *
 * NONE OF THEM IS r5 OR r6.  The r5/r6 contest is on the
 * `;; Register dispositions:` line -- `79 in 5`, `40 in 6` -- so this park is
 * about an ALLOCNO, which is the third case of the triage rule and is what the
 * park already says.  Neither the reload cursor nor `REG_ALLOC_ORDER` is the
 * place to look here; `allocno_compare`'s ordering is, and the corrections
 * above are to that.
 */
typedef struct { unsigned char _b[704]; } GlobalState;
extern GlobalState gState;
extern int GetFlag(int flagID);
extern void SetFlag(int flagID);
extern int GetPartySize(void);
extern unsigned char *GetUnit(int id);
extern int GiveDjinni(int who, int id, int effect);
extern int Func_807a458(int who, int id, int effect);

int Func_807a0f4(int id, int effect)
{
    int flag, bestIdx, best;
    unsigned char *u, *q;
    int size, i, sum, k, o, t;
    unsigned int base;

    flag = id * 20 + effect + 0x30;
    bestIdx = 0;
    best = 0x3e7;
    if (GetFlag(flag) != 0)
        return -1;
    size = GetPartySize();
    base = 0xfc * 2;
    for (i = 0; i < size; i++) {
        u = GetUnit(((unsigned char *)&gState)[base + i]);
        o = 0x8c * 2;
        if (u[id + o] <= 9) {
            q = u + o;
            sum = 0;
            for (k = 3; k >= 0; k--) {
                t = *q;
                q++;
                sum += t;
            }
            if (best > sum) {
                best = sum;
                bestIdx = ((unsigned char *)&gState)[base + i];
            }
        }
    }
    if (best == 0x3e7)
        return -2;
    GiveDjinni(bestIdx, id, effect);
    Func_807a458(bestIdx, id, effect);
    SetFlag(flag);
    return bestIdx;
}
