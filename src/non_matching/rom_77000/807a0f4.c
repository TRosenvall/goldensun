/* Func_807a0f4 -- PARK AT 7 of 88 stream encodings.  UNCHANGED FIGURE.
 * PIN-FREE, DEVICE-FREE.  Body below is the park's body, unedited.
 *
 * MEASURED BATCH 350, BRIEF D (not inherited): 7 differing encodings of 88
 * (ref 88 / ours 88), 79 INSTRUCTIONS against 79 -- IN PHASE, so the figure IS
 * a distance.  dsize 0, no RELOCATIONS line, first differing index 32
 * (ref 189e / ours 1c06).  `--whole` agrees: `XX Func_807a0f4 7 of 88 differ
 * (ours 88), first at index 32`.  PINS 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" \
 *   -w /work goldensun-build python3 tools/objcmp.py \
 *   src/non_matching/rom_77000/807a0f4.c \
 *   asm/rom_77000/rom_79460_c_c_c_c_a_c_c_c_a_c_a_a.s --func Func_807a0f4
 *
 * SPLIT SHAPE: NONE.  One function, no data section; a landing is a whole-file
 * conversion and needs no split_s.py step.  (Unchanged.)
 *
 * ALLOCATOR INPUTS RE-READ out of `.17.lreg`, not inferred from the chain:
 *     Register 40 used 7 times across 24 insns   <- the check_dbra_loop counter
 *     Register 79 used 9 times across 46 insns   <- the gState byte cursor
 *     ;; 12 regs to allocate: 38 37 42 41 39 79 40 32 36 33 35 34
 * Batch 337's numbers are EXACT.  Counter-first needs 14*L79 > 27*L40, i.e.
 * 644 against 648 -- short by four.  All of that stands.
 *
 * ===== BATCH 350: BATCH 337'S CLOSING QUESTION IS REFUTED =====
 *
 * Batch 337 ended "SO THE WHOLE PARK IS NOW ONE QUESTION: A SOURCE SHAPE WITH A
 * **SECOND BIV** IN THE OUTER LOOP ON PASS 1.  Two bivs plus giv_count != 0
 * fails check_dbra_loop on pass 1; pass 2 then carries it on the
 * no_use_except_counting disjunct."  *** THE STATE IT ASKS FOR IS THE STATE WE
 * ARE ALREADY IN. ***  Read straight out of the `.08.loop` dump of this body:
 *     PASS 1   Loop from 54 to 163: 31 real insns
 *              Insn 157: possible biv, reg 40, const =1     <- ONE biv
 *              ... many givs with `src reg 40' ...
 *              (NO "Can reverse loop" LINE AT ALL)          <- pass 1 already fails
 *     PASS 2   Loop from 54 to 163: 24 real insns
 *              Insn 270: possible biv, reg 76, const =1     <- TWO bivs
 *              Insn 157: possible biv, reg 40, const =1
 *              Can reverse loop / Reversed loop             <- pass 2 already carries it
 * Both halves of the requirement already hold and the chain order is still wrong.
 *
 * AND THE GATE IT REASONED FROM IS MISREAD.  check_dbra_loop's disjunction is
 * loop.c:7987-7994:
 *     if ((num_nonfixed_reads <= 1
 *          && ! loop_info->has_call           <-- THIS LOOP HAS A CALL (GetUnit)
 *          && ! loop_info->has_volatile
 *          && reversible_mem_store
 *          && (bl->giv_count + bl->biv_count + num_mem_sets
 *             + num_movables + compare_and_branch == insn_count)
 *          && (bl == loop_iv_list && bl->next == 0))   <-- "only one biv" is HERE
 *         || no_use_except_counting)
 * `! loop_info->has_call' kills the ENTIRE FIRST DISJUNCT unconditionally for
 * this loop, so the "there is only one biv" condition quoted from the comment at
 * :7886-7890 is a term inside a dead disjunct and is unreachable here.  The
 * reversal can only ever fire through `no_use_except_counting', which is
 * computed only when `bl->giv_count == 0 && ! loop->exit_count' (loop.c:7884).
 * BIV COUNT IS NOT THE DIAL.  Pass 1 fails for a different reason.
 *
 * ===== THE ACTUAL CHAIN-ORDER MECHANISM, END TO END IN THE LOOP DUMP =====
 *     PASS 1  Insn 64: giv reg 53 src reg 40 benefit 2 lifetime 34 ... add 504
 *             Sorted combine statistics:
 *               {64, 23} {147, 12} {70, 12} {145, 0} {141, 0} {68, 0}
 *             giv at 147 / 141 / 70 combined with giv at 64
 *             => creates reg 76,  `Biv 76 initialized at insn 273: initial
 *                value 504'   -- i.e. `i + 504', WITHOUT the gState add
 *     PASS 2  bivs 76 and 40; givs of 76 at insns 70 and 147, `add (reg:SI 54)'
 *             Can reverse loop / Reversed loop   <- counter init emitted, insn 275
 *             biv 76 can be eliminated
 *             giv at 147 reduced to (reg:SI 79)  <- cursor init emitted, insn 286
 *             giv at 70  reduced to (reg:SI 79)
 *             biv 76 was eliminated / Reg 76: biv eliminated
 *
 * *** PASS 1 REDUCES TO `i + 504' AND PASS 2 THROWS THAT AWAY, CREATING THE REAL
 * *** CURSOR (reg 79 = gState + 504 + i) FRESH.  So BOTH inits are PASS-2
 * *** creations, and within one pass check_dbra_loop (loop.c:4408) always runs
 * *** before the giv emission (loop.c:4778) -- so the counter init is
 * *** unconditionally first.  Insn numbers confirm it: counter 275 < cursor 286.
 *
 * SO THE REQUIREMENT IS NOT A SECOND BIV.  IT IS: THE CURSOR'S FINAL REGISTER
 * MUST BE CREATED ON PASS 1 AND SURVIVE PASS 2.  Then its init predates the
 * pass-2 counter init, which gives L79 = 47 AND L40 = 23 at the same time --
 * 14*47 = 658 > 27*23 = 621, a flip with margin rather than by four.
 *
 * WHY PASS 1 REDUCED TO THE BARE 504: because `base + i' EXISTS AS A STANDALONE
 * VALUE.  Insn 64's giv (`reg 53 ... lifetime 34', benefit 23) is that value,
 * used by both array reads, and it outranked the two `dest address' givs that
 * carry `add (plus (reg 54) (const_int 504))'.  Remove the standalone value and
 * pass 1 has only the full-pointer givs to reduce.
 *
 * MEASURED THIS BATCH -- THAT ROUTE, AND WHY IT FAILS.  Hoisting the whole
 * pointer removes the standalone value but ALSO takes the `ldr =gState' pool
 * load and the `0xfc*2' constant build out of the loop preheader, which is where
 * the ROM has them.  All worse, all RELOCDIFF (figure | dsize):
 *     base                                                        7 |  0
 *     `gp = (unsigned char *)&gState + 0xfc*2', both reads gp[i]  62 | -4
 *     the same with the `base' local kept           62 | -4  bit-identical
 *     `gp = (unsigned char *)&gState + base'                      60 |  0
 *     `gp = &((unsigned char *)&gState)[base]'      60 |  0  bit-identical
 *     gp[i] in the FIRST read only (halves varied independently)  62 | +4
 *     gp[i] in the SECOND read only                               61 | +4
 *     gp hoisted above GetPartySize()                             63 | -4
 * EIGHT SPELLINGS, FIVE DISTINCT PROGRAMS.
 *
 * ==> OPEN, AS A REQUIREMENT AND NOT A SWEEP: a source shape in which pass 1's
 *     giv combination reduces to the FULL pointer `gState + base + i' -- i.e.
 *     one with NO standalone `base + i' value -- WHILE LEAVING THE `ldr =gState'
 *     POOL LOAD AND THE `0xfc*2' CONSTANT BUILD INSIDE THE LOOP PREHEADER.  The
 *     eight rows above remove both at once; they need separating.  Worth the
 *     whole 7, not two of it, because it moves BOTH live lengths together.
 *
 * DO NOT RE-SWEEP: the anchor axis (20 placements, 9 distinct programs, b347),
 * the flag axis (all eight groups, b347), the declaration lever (b325), or any
 * spelling of the comparison or the inner loop (b337's fifteen rows).  The
 * residue is a loop-pass-ordering effect, not a spelling.
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
