/* Func_80286a0 -- 0x080286a0, asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c.s
 *
 * STILL NON-MATCHING, 4 of 85 encodings -- RE-MEASURED batch 326B, third time
 * running.  The park's figure, anatomy AND verdict all survive again.  Real
 * distance 3; idx 83 is a pool word adjudicable only by `make compare`.
 *
 * VERIFY: python3 tools/objcmp.py src/non_matching/rom_15000/80286a0.c asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c.s --func Func_80286a0
 *
 * SIZE 188 = 188.  ENCODINGS 85 = 85.  INSTRUCTIONS **81 = 81** (see the tool
 * note below -- objcmp's INSTRUCTION COUNT line FALSE-POSITIVES here and must
 * not be believed on this function).  Relocations differ by `_CONST_1f` only.
 * PINS: 0.
 *
 * ============== DO NOT TRUST objcmp's INSTRUCTION COUNT LINE HERE ==========
 *
 * objcmp prints, at production flags:
 *     XX INSTRUCTION COUNT  ref 84, ours 81  (excluding 1/4 trailing pad word(s))
 *        NOTE: size and encoding count MATCH -- a pad is absorbing the
 *        difference, so the positional figure below measures MISALIGNMENT
 * **IT DOES NOT.**  Both sides are 81 instructions.  The two objects' tails,
 * dumped from the very objects objcmp built:
 *       81  0000       0000          <- gas halfword align pad
 *       82  00000000   00000000      <- pool word, R_ARM_ABS32 iwram_3001f38
 *       83  0000001f   00000000  XX  <- pool word, R_ARM_ABS32 _CONST_1f
 *       84  00000000   00000000      <- pool word, R_ARM_ABS32 .L373ef
 * `_insns()` (tools/objcmp.py:420-424) strips trailing `00000000` BY VALUE.  On
 * the reference idx 83 carries the assembled literal `0000001f`, so stripping
 * stops there (84).  On ours idx 83 is an UNLINKED RELOCATION PLACEHOLDER, so
 * stripping runs on through 84, 83, 82 and the genuine pad at 81 (81).  Three
 * of the four words it called pad are POOL WORDS CARRYING RELOCATIONS.
 *
 * The tool's own dump() comment already names this population -- "the
 * relocation placeholder words of the symbol-address technique" -- as the reason
 * `-dz` is required; the same population now breaks the newer guard above it.
 * The guard is right in general and wrong for the `_CONST_*` / `_MSG_*` /
 * `_FILE_*` family: any park whose pool ENDS in a placeholder whose reference
 * holds a non-zero literal will read as misaligned when it is not.  A fix would
 * be for `_insns` to refuse to strip a word that has a relocation against its
 * byte offset; `a_rel` / `b_rel` are already in hand at the call site.
 *
 * ===================== WHAT THE 4 ARE (per index, re-measured) ==============
 *
 *     XX  21  ref 4680 mov  r8,r0    | ours 2392 movs r3,#0x92
 *     XX  22  ref 2392 movs r3,#0x92 | ours 199b adds r3,r3,r6
 *     XX  23  ref 199b adds r3,r3,r6 | ours 4680 mov  r8,r0
 *     XX  83  ref 0000001f .word     | ours 00000000 .word + R_ARM_ABS32 _CONST_1f
 *
 * const.sym defines `_CONST_1f` at 0x1f, so the LINKED word is the ROM's.  Idx
 * 24 `mov fp,r3` and idx 25 `b` are identical both sides: the whole residue is
 * inside one five-insn block, and it is ONE ROTATION -- the ROM issues insn 38
 * (`cur = start`) FIRST, we issue it third.
 *
 * ===================== THE DECIDING RUNG, RE-CONFIRMED ======================
 *
 * `.23.sched2`, production flags, basic block 2 (the join block of
 * `if (target < start)`):
 *     ;;  Ready list (t = 0):   38  252
 *     ;;  0  252 r3=0x92  /  1  41 r3=r3+r6  /  2  38 r8=r0
 *     ;;  3  255 fp=r3    /  4  43 pc=L70
 * priorities 252=3, 41=2, 255=1, 38=1.  Insn 38 IS ready at t=0 and loses on
 * rank_for_schedule's FIRST rung (haifa-sched.c:4040, priority), so no
 * tie-break is reached.  Priority is the longest dependence path to the block
 * end: 252 -> 41 -> 255 is 3; insn 38's only in-block dependent is the
 * block-end jump, so it is 1.  Reaching 3 would merely TIE, and the tie falls
 * to the dependent-count rung (:4096) where 252 leads 2 to 1 -- so **38 needs
 * priority 4**, i.e. a two-deep dependent chain for `cur` inside a block whose
 * instruction count is already exact.
 *
 * Below that rung the ladder would have favoured us: the bottom rung is
 * `return INSN_LUID (tmp) - INSN_LUID (tmp2);` (:4112) and the LOWER LUID wins,
 * and `cur = start;` stands before the `m` build in the source.  The whole
 * difficulty is that rung 1 settles it first.
 *
 * ===================== WHAT BATCH 326 ADDED: ONE MORE CLASS CLOSED ==========
 *
 * The park's analysis says the block must stop containing the `m` build.  Every
 * previous attempt pushed statements DOWN INTO THE ARMS of the `if` (and all
 * showed dsize +4, proving jump2's cross-jumper did not merge them back).
 * **The opposite move -- HOISTING the `m` build ABOVE the `if`, which empties
 * block 2 of its competitor outright -- had never been tried.  It is worse,
 * four ways, at EXACTLY EQUAL encoding and instruction counts:**
 *
 *     m build immediately above the `if`                17   (was 4)
 *     m build above `*c = start`                        17
 *     m build before `c = ...`                          17
 *     m build above the store, cur still after the if   17
 *     cur hoisted above the `if`                        40   (+2 insns)
 *     cur AND the m build both above the `if`           47   (+2 insns)
 *
 * So the block-boundary dimension is now closed from BOTH directions, with
 * figures: pushing the competitor down does not merge, and pulling it up costs
 * 13.  Hoisting `m` leaves the preheader at 17 because `m` then has to survive
 * the compare and the branch, which reload pays for elsewhere in the block.
 *
 * ALSO STILL TRUE (batch 321/322, all re-confirmed as the base of the sweep
 * above): `cur = start` at the tail of both arms 40 (dsize +4, NOT merged); the
 * same with the test inverted 37 (+4); `cur` and the `m` build both in the arms
 * 77 (+12); only the `m` build in the arms 69 (+8); `*c = cur` in the join
 * block 46, after the `m` build 47, before the `if` 46 (all +4); `k = 0x92;`
 * named with m from k, 4, EXACTLY INERT, both orderings.
 *
 * WHAT WOULD CLOSE IT: an in-block dependent for `cur` that costs no
 * instruction, raising insn 38's priority to 4.  Three batches have now looked
 * and none has found one.  The park's three landed levers (the `goto`-into-
 * `do/while` loop form 67->33, the explicit if/else for `j` 33->7, and
 * `step`/`extra` before `c` 7->4) all reproduce and are kept.
 *
 * SPLIT SHAPE, re-run batch 326: the reference holds TWO functions,
 * ['Func_8028574', 'Func_80286a0'], so landing needs a two-way split.
 *   tools/datacheck.py asm/.../rom_23178_a_a_a_a_c_c_a_c.s -> NO OUTPUT, exit 0
 *   tools/split_s.py ... Func_80286a0 --dry-run ->
 *     rom_23178_a_a_a_a_c_c_a_c_a.s (1 function, 150 lines)   [Func_8028574]
 *     rom_23178_a_a_a_a_c_c_a_c_b.s (1 function,  92 lines)   [Func_80286a0]
 *   install path on a landing: src/rom_15000/rom_23178_a_a_a_a_c_c_a_c_b.c
 */
struct Ui {
    unsigned char pad0[0x78];
    void *box;
};

extern unsigned char *iwram_3001f38;
extern unsigned char L373ef[] __asm__(".L373ef");
extern int _CONST_1f;

extern void Func_8016478(void *w);
extern void Func_801e7c0(int id, void *w, int a, int y);
extern void WaitFrames(int n);
extern void _PlaySound(int id);

int Func_80286a0(int start, int target)
{
    struct Ui *u;
    short *c;
    short *m;
    unsigned char *t;
    int cur;
    int step;
    int extra;
    int id;
    int k;
    int d;
    int j;

    u = (struct Ui *)iwram_3001f38;
    step = 1;
    extra = 0xc;
    c = (short *)((unsigned char *)u + 0x8c);
    *c = start;
    if (target < start)
        step = -1;
    cur = start;
    m = (short *)((unsigned char *)u + 0x92);
    goto entry;
    do {
        *c += step;
        _PlaySound(0x6f);
        extra = 0;
        cur += step;
    entry:
        Func_8016478(u->box);
        if (*m != 0) {
            id = *m + *c;
        } else {
            k = *c + 0x84;
            id = *((unsigned char *)u + k) + (int)&_CONST_1f;
        }
        Func_801e7c0(id, u->box, 0, 0);
        t = L373ef;
        d = *c - target;
        if (d >= 0)
            j = d;
        else
            j = target - *c;
        WaitFrames(t[j] + extra);
    } while (cur != target);
    WaitFrames(0x30);
    _PlaySound(0x70);
    return target;
}
