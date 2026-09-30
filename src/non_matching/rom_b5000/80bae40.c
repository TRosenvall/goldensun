/* Func_80bae40  --  0x080bae40  --  ResolveActionOutcome
 *
 * NON-MATCHING, 789 of 830 encodings differ  (objcmp at production flags).
 *
 * SIZE IS EXACT: ref 1864 bytes, ours 1864.
 * COUNT is NOT: ref 830 instructions, ours 826  (4 short).
 * So this is NOT yet a true distance -- that needs size AND count -- and the
 * 789 figure is SATURATED (first difference at index 9, in the prologue).
 * The ranking figure is aligncmp:
 *
 *     aligncmp  294 aligned-equal  (35.4% of ref)  831 differing in 107 hunks
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80bae40.c \
 *     asm/rom_b5000/rom_b9b30_c_c_c.s --func Func_80bae40
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_b5000/80bae40.c \
 *     asm/rom_b5000/rom_b9b30_c_c_c.s Func_80bae40
 *
 * SPLIT SHAPE: NONE NEEDED. asm/rom_b5000/rom_b9b30_c_c_c.s holds this one
 * function, and its 75 data words are gcc's OWN two casesi tables (below), not
 * a data section -- so split_s.py is not involved and nothing is exported.
 *
 * SHIMS: none. tools/shimcount.py reports nothing; no pins, no flag rows, no
 * fakematch.txt entry due.
 *
 * ========================================================================
 * THE FRAME IS EXACT, AND THE "HOLE" WAS A THIRD ARRAY
 * ========================================================================
 * `sub sp, #0x5c` = 92 bytes, matched from the first candidate. There is NO
 * outgoing-argument area at all -- every callee here takes at most one
 * argument -- so sp+0x00..0x20 is eight pure spill slots:
 *
 *     sp+0x00,0x04,0x08   inner-sort spill slots (late temps)
 *     sp+0x0c             reload's slot for the `&out[0]` address pseudo,
 *                         re-materialised on six different paths
 *     sp+0x10             reload's slot for the `&cand[0]` address pseudo
 *     sp+0x14             n     -- the compacted count
 *     sp+0x18             i     -- see the variable partition below
 *     sp+0x1c             arg0  (the parameter, highest spill slot)
 *     sp+0x20..0x38       int  out[6]    24 bytes
 *     sp+0x38..0x50       int  cand[6]   24 bytes
 *     sp+0x50..0x5c       u16  flag[6]   12 bytes
 *
 * 32 + 24 + 24 + 12 = 92 EXACTLY, no hole and no padding.
 *
 * THE 24-BYTE GAP AT sp+0x20 WAS THE FINDING. `add r2,sp,#0x5c` appears twice
 * and looks like it addresses incoming stack arguments; it does not. It is a
 * scratch base that gcc SUBTRACTS from to reach two of the arrays --
 * `sub r0,#0xc` gives sp+0x50 and `sub r4,#0x24` gives sp+0x38 -- so those two
 * are found early and sp+0x20..0x38 reads as an unexplained 24-byte hole.
 * The third array only shows up much later, at .Lbb38a, spelled differently:
 *     mov r0, sp / add r0, #0x20 / str r3, [r0, r1]
 * That is `out[]`, written during the compaction pass. So the rule to carry
 * forward: a frame hole between two known aggregates is a THIRD aggregate
 * addressed by a different idiom, and `mov rN,sp / add rN,#K` must be grepped
 * for alongside `add rN,sp,#K` or a real array will be missed.
 *
 * ========================================================================
 * BOTH SWITCHES ARE gcc's OWN TABLES -- derived, not assumed
 * ========================================================================
 * SWITCH 1 at .Lbaf7a on `p->f3`, guarded `cmp r0,#0x40 / bls`, table at
 * .Lbaf84: EXACTLY 65 entries (cases 0..64) resolving to 20 distinct arms,
 * with .Lbb142 (`score = 1`) as `default`. count = 65, range = 65, and
 * 65 >= 5 with 65 <= 10*65, so expand_end_case takes the TABLE path -- which
 * is what the brief settled and the entry count confirms.
 * Because it IS a table, group_case_nodes merges contiguous same-label nodes
 * for free, so adjacent cases are PAIRED in the source and must not be
 * duplicated: case 0-2, 6-7, 8-9, 10-11, 12-13, 14-15, 16-17, 61-62.
 * The non-adjacent sharers (5 with 56 and 57) are stacked case labels on one
 * body -- a table is indexed by value, so label order is irrelevant and one
 * code_label serves all three.
 * Every value the table sends to .Lbb142 is simply ABSENT from the source
 * (20-22, 25, 27, 29-32, 34-55, 58-60, 63), because the out-of-range `bls`
 * guard targets that same label -- .Lbb142 is `default`, not a case.
 *
 * SWITCH 2 at .Lbb316 on `(p->f1 & 0xf) - 1`, guarded `cmp r3,#9 / bhi`,
 * table at .Lbb330: 10 entries, cases 0..9. count = 10, range = 10, so table
 * again. Case 6 points at the join, i.e. an empty arm. Cases 1,4,5,7,8 share
 * one body as stacked labels.
 *
 * THE `bgt` AT .Lbb4d8 IS NOT A MISSING CASE, exactly as the brief states, and
 * this reconstruction confirms it independently: the nodes are {1,2,3,4},
 * count 4 < 5 so expand_end_case takes the DECISION-TREE path,
 * balance_case_nodes roots at 2, the left subtree is {1} and the right is
 * {3,4} tested at .Lbb4e8. `bgt` is signed, so node 1 has no tight lower bound
 * to collapse against and keeps its explicit `beq`. Nothing is missing; the
 * CASE A reading applies and no `case -1:` exists here.
 *
 * ========================================================================
 * LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ========================================================================
 * 1. THE VARIABLE PARTITION: ONE `i` ACROSS FOUR DISJOINT LOOPS.
 *    size 1856 -> 1864 (EXACT), count 822 -> 826 (8 short -> 4 short).
 *    sp+0x18 is the counter for the loop A scan, the loop B scan, the main
 *    per-candidate loop AND the bubble sort's outer loop. Four disjoint loops,
 *    one slot, therefore ONE source variable -- this is lever 3 read off the
 *    frame rather than off a register. Writing separate `idx`/`i` variables
 *    put two scalars where the ROM has one and cost the size match.
 *    aligncmp moved 35.7% -> 35.4%, i.e. DOWN, which is why this is recorded
 *    as a size-and-count win: the brief's rule to rank size-and-count first
 *    is what makes this the right call, and it is a clean case of aligncmp and
 *    the true metrics disagreeing.
 *
 * 2. THE SPILL-SLOT MAP AS THE DECLARATION LIST. Reading sp+0x1c/0x18/0x14
 *    descending gives arg0, i, n -- so `i` is declared BEFORE `n`, which is
 *    the opposite of the order the code reads in (n is zeroed first, at the
 *    top of the function). Assignment order is not declaration order, and the
 *    frame is the authority on the latter.
 *
 * ========================================================================
 * THE BLOCKER, ATTRIBUTED
 * ========================================================================
 * Size is exact and the function is 4 instructions short, so what remains is
 * small and local. The residue is dominated by REGISTER ASSIGNMENT
 * (local.c/global.c), not structure: the switch tables, the frame, the two
 * scan loops and the sort all line up, and the first difference is at index 9
 * in the prologue -- a pool load against a `movs r0,#0`, i.e. the order in
 * which the prologue materialises `iwram_3001e74` and the two zeroes.
 *
 * Ruled out:
 *   * sched1 -- confirmed not to run in this build.
 *   * the frame -- exact, all three arrays and all eight spill slots.
 *   * both switch shapes -- derived from the tables themselves, 65 and 10
 *     entries, and the decision-tree at .Lbb4d8 accounted for.
 *   * a missing case -- the brief's tree-wide screen found only BufferString
 *     as CASE B, and this function's tables are dense and complete.
 *
 * NEXT MOVE: the 4 missing instructions and the prologue order. The prologue
 * interleaving of `ldr r3,=iwram_3001e74` / `mov r0,#0` / `mov r7,r1` is an
 * EXPAND-order question (sched2 preserves INSN_LUID, so source statement order
 * decides it), so reordering the first three statements is the cheap
 * experiment. The arithmetic the recon warned about -- 104 `add`, 51 `lsl` --
 * turned out NOT to be the hard part: the damage-selection tail
 * (.Lbb4d8-.Lbb572) is four Random()-scaled rank pickers and reproduced
 * readily. The hard part was the frame's third array.
 */
struct Act {
    unsigned char f0; unsigned char f1; unsigned char pad_02; unsigned char f3;
    unsigned char pad_04[4]; unsigned char f8; unsigned char pad_09; unsigned short fa;
};

extern char *iwram_3001e74;
extern unsigned char *_GetUnit(int id);
extern unsigned char *_GetEnemyInfo(int id);
extern int _Func_8079ef8(int a);
extern int Random(void);

int Func_80bae40(int arg0, struct Act *p)
{
    int i;
    int n;
    int *op;
    int *cp;
    int lim;
    int t1;
    int t0;
    int out[6];
    int cand[6];
    unsigned short flag[6];
    char *st;
    unsigned char *u;
    unsigned char *ui;
    unsigned char *uj;
    int count;
    int score;
    int v;
    int sel;
    int j;
    int rank;
    int r;
    int swapped;

    n = 0;
    count = 0;
    st = iwram_3001e74;
    if (p->f0 != 0) {
        swapped = 0;
        if (p->f0 == 2 || p->f0 == 4)
            swapped = 1;
        if ((arg0 > 7) ? (swapped == 0) : (swapped != 0)) {
            i = 0;
            if (*(short *)(st + 0x58) != 0xff) {
                do {
                    v = *(short *)(st + 0x58 + i * 2);
                    if (v != 0xfe && (p->f0 != 4 || v == arg0)) {
                        cand[count] = v;
                        flag[count] = i | 0x100;
                        count++;
                    }
                    i++;
                } while (*(short *)(st + 0x58 + i * 2) != 0xff);
            }
        } else {
            i = 0;
            if (*(short *)(st + 2 + 0x64) != 0xff) {
                do {
                    v = *(short *)(st + 2 + 0x64 + i * 2);
                    if (v != 0xfe && (p->f0 != 4 || v == arg0)) {
                        cand[count] = v;
                        flag[count] = i | 0x180;
                        count++;
                    }
                    i++;
                } while (*(short *)(st + 2 + 0x64 + i * 2) != 0xff);
            }
        }
    }
    if (count == 0)
        return -2;
    for (i = 0; i < count; i++) {
        u = _GetUnit(cand[i]);
        score = 0;
        sel = p->f3;
        switch (sel) {
        case 0: case 1: case 2:
            break;
        case 3:
            if (*(signed char *)(u + 0x131) != 0)
                score = 1;
            break;
        case 4:
            if (u[0x138] != 0) score = 1;
            if (u[0x139] != 0) score++;
            if (u[0x13a] != 0) score++;
            if (u[0x13c] != 0) score++;
            if (u[0x13d] != 0) score++;
            if (u[0x141] != 0) score++;
            break;
        case 5: case 56: case 57:
            if (*(short *)(u + 0x38) == 0)
                score = 0x64;
            break;
        case 6: case 7:
            if (*(signed char *)(u + 0x133) + 1 <= 4) score = 1;
            if (u[0x132] == 1) score++;
            break;
        case 8: case 9:
            if (*(signed char *)(u + 0x133) - 1 >= -4) score = 1;
            if (u[0x132] == 1) score++;
            break;
        case 10: case 11:
            if (*(signed char *)(u + 0x135) + 1 <= 4) score = 1;
            if (u[0x134] == 1) score++;
            break;
        case 12: case 13:
            if (*(signed char *)(u + 0x135) - 1 >= -4) score = 1;
            if (u[0x134] == 1) score++;
            break;
        case 14: case 15:
            if (*(signed char *)(u + 0x137) + 1 <= 4) score = 1;
            if (u[0x136] == 1) score++;
            break;
        case 16: case 17:
            if (*(signed char *)(u + 0x137) - 1 >= -4) score = 1;
            if (u[0x136] == 1) score++;
            break;
        case 18:
            if (*(signed char *)(u + 0x131) == 0)
                score = 1;
            break;
        case 19:
            if (*(signed char *)(u + 0x131) <= 1)
                score = 1;
            break;
        case 23:
            if (u[0x13b] == 0) score = 1;
            break;
        case 24:
            if (u[0x13c] == 0) score = 1;
            break;
        case 26:
            if (u[0x140] == 0) score = 1;
            break;
        case 28:
            if (u[0x141] == 0) score = 1;
            break;
        case 33:
            if (*(signed char *)(u + 0x133) > 0) score = 1;
            if (*(signed char *)(u + 0x135) > 0) score++;
            if (*(signed char *)(u + 0x137) > 0) score++;
            if (*(signed char *)(u + 0x12c) > 0) score++;
            if (*(signed char *)(u + 0x12d) > 0) score++;
            if (*(signed char *)(u + 0x12e) > 0) score++;
            if (*(signed char *)(u + 0x12f) > 0) score++;
            break;
        case 61: case 62:
            if (*(short *)(u + 0x38) < *(short *)(u + 0x34)) score = 1;
            break;
        case 64:
            if (u[0x138] != 0) score = 1;
            if (u[0x139] != 0) score++;
            if (u[0x13a] != 0) score++;
            if (u[0x13c] != 0) score++;
            if (u[0x13d] != 0) score++;
            if (u[0x141] != 0) score++;
            if (u[0x140] != 0) score++;
            if (*(signed char *)(u + 0x131) != 0) score++;
            break;
        default:
            score = 1;
            break;
        }
        if ((unsigned short)*(short *)(u + 0x38) == 0 && _Func_8079ef8(sel) == 0)
            score = 0;
        if (score == 0) {
            switch ((p->f1 & 0xf) - 1) {
            case 0:
                if (*(short *)(u + 0x38) != 0
                    && *(short *)(u + 0x38) < *(short *)(u + 0x34))
                    score++;
                break;
            case 2: case 3:
                if (*(short *)(u + 0x38) != 0) score++;
                break;
            case 9:
                if (*(short *)(u + 0x3a) != 0) score++;
                break;
            case 1: case 4: case 5: case 7: case 8:
                if (p->fa != 0 && *(short *)(u + 0x38) != 0) score++;
                break;
            default:
                break;
            }
        }
        if (score != 0) {
            cand[n] = cand[i];
            out[n] = flag[i];
            n++;
        }
    }
    if (n == 0)
        return -1;
    op = out;
    if (p->f0 == 1 && p->f8 == 1
        && *(signed char *)(_GetEnemyInfo(_GetUnit(arg0)[0x128]) + 0x35) != 2
        && (unsigned)((p->f1 & 0xf) - 3) <= 2) {
        rank = -1;
        for (i = 0; i < n; i++) {
            lim = n - 1;
            for (j = 0; j < lim - i; j++) {
                ui = _GetUnit(cand[j]);
                uj = _GetUnit(cand[j + 1]);
                if (*(signed char *)(_GetEnemyInfo(_GetUnit(arg0)[0x128]) + 0x35) == 0) {
                    t0 = *(short *)(ui + 0x38);
                    t1 = *(short *)(uj + 0x38);
                } else {
                    t0 = *(short *)(ui + 0x34);
                    t1 = *(short *)(uj + 0x34);
                }
                if (t0 < t1) {
                    v = cand[j];
                    cand[j] = cand[j + 1];
                    cand[j + 1] = v;
                    v = out[j];
                    out[j] = out[j + 1];
                    out[j + 1] = v;
                }
            }
        }
        switch (n) {
        case 1:
            rank = 0;
            break;
        case 2:
            r = (Random() * 11) >> 16;
            rank = r <= 5 ? 0 : 1;
            break;
        case 3:
            r = (Random() * 15) >> 16;
            if (r <= 5) rank = 0;
            else if (r > 10) rank = 2;
            else rank = 1;
            break;
        case 4:
            r = (Random() * 18) >> 16;
            if (r <= 5) rank = 0;
            else if (r <= 10) rank = 1;
            else if (r <= 14) rank = 2;
            else rank = 3;
            break;
        default:
            break;
        }
        if (rank >= 0)
            return op[rank];
    }
    return op[(Random() * n) >> 16];
}
