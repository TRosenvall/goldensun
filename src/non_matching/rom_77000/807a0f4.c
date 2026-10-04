/* Func_807a0f4 -- 0x0807a0f4, asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_a.s
 * (1 function, no data section, so landing needs NO split).
 *
 * STILL NON-MATCHING.  PARK AT 7 of 88 encodings -- the park's figure, RE-MEASURED
 * and confirmed, including `--whole`: 7 of 88, 92 lines against 92, RELOCATIONS
 * IDENTICAL.  Body below is the park's body, unchanged.  PINS: 0.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b322/F/p3_candidate.c \
 *     asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_a.s --func Func_807a0f4
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
