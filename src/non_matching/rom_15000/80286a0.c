/* Func_80286a0 -- 0x080286a0, asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c.s
 *
 * STILL NON-MATCHING, 4 of 85 encodings -- RE-MEASURED batch 322A, the park's
 * figure and the park's DIAGNOSIS both survive.  This is the rare park that was
 * right; the correction here is to its ANATOMY, not its verdict.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80286a0.c \
 *     asm/rom_15000/rom_23178_a_a_a_a_c_c_a_c.s --func Func_80286a0
 *
 * --func: 4 of 85, SIZE IDENTICAL, relocations differ (see the phantom below).
 * --whole: the reference holds TWO functions, ['Func_8028574', 'Func_80286a0'],
 *   so landing needs a two-way split.  SPLIT SHAPE (both re-run this batch):
 *     tools/datacheck.py asm/.../rom_23178_a_a_a_a_c_c_a_c.s -> NO OUTPUT, exit 0
 *       (no TEXT/DATA split; the split is a plain two-function one)
 *     tools/split_s.py ... Func_80286a0 --dry-run ->
 *       rom_23178_a_a_a_a_c_c_a_c_a.s (1 function, 150 lines)   [Func_8028574]
 *       rom_23178_a_a_a_a_c_c_a_c_b.s (1 function,  92 lines)   [Func_80286a0]
 *       removes rom_23178_a_a_a_a_c_c_a_c.s, rewrites stage1.ld
 *       install path on a landing: src/rom_15000/rom_23178_a_a_a_a_c_c_a_c_b.c
 * PINS: 0.  No `register __asm__`, no `__asm__("")`, no per-file flag, no
 *   fakematch row.  The one device is `_CONST_1f`, which const.sym ALREADY
 *   defines -- see below -- and which is a relocation, not a pin.
 *
 * ===================== WHAT THE 4 ARE (re-measured per index) ================
 *
 *     XX  21  ref 4680 mov  r8,r0    | ours 2392 movs r3,#0x92
 *     XX  22  ref 2392 movs r3,#0x92 | ours 199b adds r3,r3,r6
 *     XX  23  ref 199b adds r3,r3,r6 | ours 4680 mov  r8,r0
 *     XX  83  ref 0000001f .word     | ours 00000000 .word + R_ARM_ABS32 _CONST_1f
 *
 * *** INDEX 83 IS A POOL WORD, NOT AN INSTRUCTION. ***  Determined, not assumed:
 * objdump gives it no mnemonic and `-r` puts an R_ARM_ABS32 against `_CONST_1f`
 * at 0xb4.  const.sym defines `_CONST_1f` at 0x1f, so the LINKED word is the
 * ROM's `0000001f` and only `make compare` can adjudicate it.  **The real
 * distance is 3, and all 3 are one rotation.**  Idx 24 `mov fp,r3` and idx 25
 * `b` are IDENTICAL in both, so the whole residue is inside one 5-insn block.
 *
 * ===================== THE DECIDING RUNG, FROM THE DUMP =====================
 *
 * The park said the rotation is priced out and it is right.  `.23.sched2` at
 * production flags, basic block 2 (the join block of `if (target < start)`):
 *
 *     ;;  Ready list (t = 0):   38  252
 *     ;;  0  252 r3=0x92  /  1  41 r3=r3+r6  /  2  38 r8=r0
 *     ;;  3  255 fp=r3    /  4  43 pc=L70
 *
 * priorities 252=3, 41=2, 255=1, 38=1.  **Insn 38 IS in the ready list at t=0
 * and loses on the FIRST rung**, so no tie-break is involved -- which is the
 * opposite of the DisplayMenuArrowCursor case in this same bank, where the ROM's
 * insn was not ready at all.  Grouping the two as "one rotation each" would be
 * wrong; they are decided on different rungs.
 *
 * Priority is the longest dependence path to the block end.  252 -> 41 -> 255
 * is 3.  Insn 38's only in-block dependent is the block-end jump, so it is 1,
 * and `cur` has nothing else in this block to feed: the other four insns all
 * compute `m`.  Reaching 3 would TIE, and a tie then falls to the dependent
 * count, where 252 leads 2 to 1 -- so 38 needs **4**, i.e. a two-deep dependent
 * chain for `cur` inside a block whose instruction count is already exact.
 *
 * ===================== SO THE ATTACK WAS THE BLOCK BOUNDARY =================
 *
 * Batch 321's lever: tail cross-jumping runs in **jump2, AFTER sched2**, so
 * putting a statement at the end of both arms of the `if` gives sched2 a block
 * where it is alone and lets jump2 merge the duplicate back out.  *** THAT DOES
 * NOT HAPPEN HERE, AND THE TELL IS dsize. ***  Measured, one container:
 *
 *     v00_base (park body)                        4   dsize  0
 *     `cur = start` at the tail of BOTH arms      40   dsize +4   <-- NOT merged
 *     the same with the test inverted             37   dsize +4   <-- NOT merged
 *     `cur` and the `m` build both in the arms    77   dsize +12
 *     only the `m` build in the arms              69   dsize  +8
 *     `*c = cur` moved into the join block        46   dsize  +4
 *     `*c = cur` after the `m` build              47   dsize  +4
 *     `*c = cur` before the `if`                  46   dsize  +4
 *     `k = 0x92;` named, m from k                  4   dsize  0   EXACTLY INERT
 *     the same with `k = 0x92` before `cur`        4   dsize  0   EXACTLY INERT
 *
 * **dsize +4 on every duplicated-tail variant means jump2 did not merge them --
 * it kept both copies.**  So the duplicate-tail-plus-cross-jump lever is
 * BOUNDED OFF for this function, with its evidence attached: the two arms here
 * are not a switch's arms ending in a call, they are a 2-insn arm against an
 * empty one, and jump2's cross-jumper did not take it.
 *
 * The named-constant lever (landed sibling src/rom_15000/rom_1de5c_a_c_b.c: a
 * loop-invariant LITERAL is emitted after the source-order preheader statements,
 * naming it promotes it into source-order position) is **exactly inert** at 4
 * both ways.  Per batch 321's framing that is a dividend, not a failure -- but it
 * does not move this residue, because the problem is priority, not position.
 *
 * WHAT WOULD CLOSE IT: an in-block dependent for `cur` that costs no
 * instruction.  Nothing in this brief found one.  The park's three landed levers
 * (the `goto`-into-`do/while` loop form 67->33, the explicit if/else for `j`
 * 33->7, and `step`/`extra` before `c` 7->4) all reproduce and are kept.
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
