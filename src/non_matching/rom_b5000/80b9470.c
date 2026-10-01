/* Func_80b9470 (0x080b9470) -- NON-MATCHING, 37 of 105 encodings. SIZE EXACT.
 * Blocker class: global_alloc -- an eighth allocno in a function with seven
 * callee-saved registers. Never attempted before batch 277.
 *
 * asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_c.s (4 functions after batch 277's two
 * splits, so landing needs another split).
 *
 * THE FIRST LOOP IS EXACT EXCEPT ONE INSTRUCTION; the sort loop is a register
 * permutation. Remaining differing pairs:
 *
 *     3rd byte test   rom `mov r3, r2 / cmp r3, #0x35`   ours `cmp r2, #0x35`
 *     `n - 1`         rom `mov r9, r7`                   ours `str r7, [sp, #0]`
 *     temp address    rom `mov r0, sp` ... `mov r1, sp`  ours `mov r0, r9` ...
 *     frame           rom `sub sp, #0x10`                ours `sub sp, #0x14`
 *
 * THE WALL, PRICED WITH THE PASSES' OWN ARITHMETIC -- and it is a chain of three
 * passes, which is why no spelling reaches it:
 *
 *   1. `.00.rtl` holds TWO separate `(plus virtual-stack-vars -16)`, one per
 *      `&tmp` call argument.
 *   2. cse1 COMMONS them into one pseudo -- `.03.cse` insn 182, one def and two
 *      uses with REG_EQUAL on the copies.
 *   3. `.08.loop` then always hoists it: threshold*savings*lifetime is 20*1*10
 *      against an insn_count of 27.
 *
 * So it becomes an EIGHTH global allocno in a function with exactly seven
 * callee-saved registers under -fcall-used-r4, and `.18.greg` shows the contest:
 *
 *     ;; 12 regs to allocate: 106 62 110 36 35 34 39 41 37 33 32 38
 *
 * 41 is `&tmp` (3 references) and takes r9; 38 is `n - 1` (2 references), is
 * processed LAST, and spills -- which is also the `sub sp, #0x14`. The ROM is the
 * other way round.
 *
 * THIS IS THE DOCUMENTED BOUNDS CASE FOR THE STRUCT LEVER, and it was confirmed
 * rather than assumed: it does NOT reach a global_alloc reference-count priority.
 * Declaration order both ways is 74/74, and four placements of the temp pointer
 * (function top 104, before `top:` 74, inside the outer loop 74, inside the inner
 * loop 73) never flip it, because IT IS NOT A TIE.
 *
 * EIGHT SPELLINGS INTENDED TO KEEP THE TWO `&tmp` UNCOMMONED WERE ALL
 * BYTE-IDENTICAL TO EACH OTHER at 70 differing: a `struct` temp, `char tmp[0x10]`
 * with array decay, a `union` with two members at offset 0, typed against `void *`
 * copy parameters, and a pointer local assigned twice, once before each use. cse
 * commons `(plus sfp K)` regardless of spelling.
 *
 * FLAGS, singly: -fno-gcse 69, -fno-rerun-cse-after-loop 77, -fno-strength-reduce
 * 66 but it destroys loop 1's dbra and the sort's givs, -fno-schedule-insns2 78,
 * -fno-expensive-optimizations 75, -fno-peephole 72, -fno-regmove and
 * -fno-optimize-register-move 72, and -fno-cse-follow-jumps, -fno-cse-skip-blocks,
 * -fno-thread-jumps, -fno-force-mem, -fno-rerun-loop-opt, -fomit-frame-pointer all
 * 69 (inert). -O1 is 70 and loses the dbra.
 *
 * FLAGS IN PAIRS, because batch 276 showed a one-at-a-time sweep can miss a pair:
 * -fno-gcse -fno-rerun-cse 75; -fno-gcse -fno-sched2 79; -fno-rerun-loop-opt
 * -fno-gcse 69; -fno-expensive-optimizations -fno-gcse 69; -fno-rerun-loop-opt
 * -fno-rerun-cse 75; -fno-sched2 -fno-rerun-cse 84. NO PAIR BEATS THE DEFAULT.
 *
 * WHAT IS RIGHT AND MUST BE KEPT: the outer `goto` loop is REQUIRED -- the ROM
 * recomputes `a + i*16` inside it, and a `do/while` hoists that out and spills two
 * values (80 -> 70 with the goto). A separate `last = n - 1` variable rather than
 * `n--` was also 80 -> 70. And `unsigned char b` is what buys the QImode range
 * test: `b == 0x2e || b == 0x2f || b == 0x35` on an `unsigned char` folds in QImode
 * and emits the ROM's `add r3,#0xd2 / lsl r3,#24 / cmp r3, (0x80<<17)`, where `int
 * b` folds in SImode to `sub r3,#0x2e / cmp r3,#1`. The `<= 1` needs the shift and
 * the `== 0x35` does not -- that asymmetry is the tell.
 *
 * AND ONE THING NOT TO ADD: the pooled `0xf` at 0x080b949e is a GENUINE POOL WORD
 * (`4b0d` -> `.word 0x0000000f` at 0x080b94d4), one of only two in the tree, and
 * plain `& 0xf` on a value read by `ldrh` reproduces it EXACTLY with no relocation.
 * That is const.sym's own documented halfword exception firing. Do NOT add a
 * _CONST_f entry for it.
 *
 * NEXT: nothing source-level. A POOLED `sfp+K` TEMP ADDRESS CANNOT BE KEPT
 * UNCOMMONED -- two at expand, one after cse, always; and once commoned, LICM's
 * arithmetic makes the hoist unavoidable in any loop under roughly 200
 * instructions. If the ROM rematerialises `mov r0, sp` at each use and the function
 * has no spare callee-saved register, the function is allocation-blocked and the
 * search should stop rather than continue through temp spellings -- eight of them
 * produce byte-identical objects.
  *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_b5000/80b9470.c \
 *     asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_c.s --func Func_80b9470
 *
 * RECIPE ADDED IN BATCH 311.  This park carried an "N of M" figure with no way to
 * re-measure it, so its number could never be caught lying -- the dangerous half of
 * what parkcheck used to lump into one UNCHECKABLE verdict.  A tree-wide sweep found
 * eight such parks; this is one of them.  The figure above is NOT re-measured by the
 * act of adding this line: run it.
 *
 * ================= BATCH 316: THE ARITHMETIC ABOVE IS WRONG =================
 * Still 37 of 105, SIZE EXACT. The blocker class is right, the pricing is not.
 * This park says "41 is `&tmp` (3 references)" and "38 is `n - 1` (2
 * references)", i.e. it reads the contest as a near-tie and concludes "it is
 * NOT a tie". *** `REG_N_REFS` IS LOOP-DEPTH WEIGHTED. *** gcc-2.96's flow.c
 * does `REG_N_REFS (i) += loop_depth`, with loop_depth 1 at function level and
 * +1 per enclosing loop, so a reference inside one loop counts 2 and inside two
 * counts 3. `.17.lreg` prints the weighted figure, and it is:
 *
 *     41 (&tmp)   7 refs / 30 insns   floor_log2(7)*7/30 = 0.4667
 *     38 (n - 1)  3 refs / 31 insns   floor_log2(3)*3/31 = 0.0968
 *
 * &tmp's two uses are both at loop depth 2 (weight 3 each, plus 1 for the def);
 * `last`'s single use is at depth 1 (weight 2, plus 1). So it is not a tie by a
 * factor of 4.8, and `allocno_compare` reproduces `.18.greg` exactly:
 *   112 3.478 | 68 2.4 | 116 1.909 | 36 1.607 | 35 1.324 | 34 0.66 | 39 0.5 |
 *   41 0.4667 | 37 0.4375 | 33 0.2286 | 32 0.1212 | 38 0.0968
 *   `;; 12 regs to allocate: 112 68 116 36 35 34 39 41 37 33 32 38`.
 * For 38 to win, 41 needs <= 2 refs, or live_length(41) > 144 in a 105-insn
 * function, or 38 needs 8 refs. All three are out of reach. A TIE WOULD HAVE
 * BEEN ENOUGH (allocno_compare breaks ties by allocno number and 38 < 41) --
 * which is what made the old near-tie reading look workable. It is not a tie.
 *
 * THE RESIDUE, EXACTLY (side-by-side normalised listing, batch 316):
 *   1. ROM `adds r3,r2,#0 / cmp r3,#53`, ours `cmp r2,#53` -- +1 instruction,
 *      and it is why the ROM's pool carries a `.short` pad: 2 encodings, plus
 *      every branch displacement after it.
 *   2. ROM `mov r9,r7` / `mov r7,r9` for `n-1`, ours `add r3,sp,#4 /
 *      str r7,[sp,#0] / mov r9,r3 / ldr r7,[sp,#0]`, and `sub sp,#20` against
 *      `#16`. OUR `&tmp` SITS AT sp+4 ONLY BECAUSE THE SPILL SLOT TOOK sp+0,
 *      which is why the ROM's `mov r0,sp` is free and our `mov r0,r9` is not.
 *   3. prologue and second-call argument order -- sched2, downstream of 2.
 * Items 2 and 3 are one fact. Item 1 is independent and survives everything.
 *
 * 54 CROSSED VARIANTS (9 spellings of the `== 0x35` test x 6 shapes/placements
 * of `&tmp` and `last`), one container, 2.4 s. **36 of the 54 are BYTE-IDENTICAL
 * to the base at 37**, including a second read of `mi[3]`, `(b|0)`, `(b^0x35)==0`,
 * `b-0x35==0`, an `int` carrier, and `m = last` inside the inner loop (which
 * raises 38's weighted refs but not enough). NEGATIVE: `t = &tmp` before
 * `last = n-1` 78 (this park's declaration-order figure, reproduced); a `switch`
 * 85; the test split into `if`/`else if` 100.
 * **The "stop rather than continue through temp spellings" verdict STANDS.**
*/
struct Act {
    short id;
    short f2;
    short prio;
    short kind;
    unsigned short move;
    short fa;
    short fc;
    short fe;
};

extern unsigned char *_GetUnit(int id);
extern int _Func_807a5b0(int a, int b);
extern unsigned char *_GetMoveInfo(int x);
extern void Func_8001af8(void *dst, void *src, int n);

void Func_80b9470(struct Act *a, int n)
{
    struct Act tmp;
    void (*copy)(void *, void *, int);
    struct Act *p;
    int i;
    int k;
    int last;
    int swapped;
    int m;
    struct Act *t;
    unsigned char b;

    p = a;
    for (k = 0; k < n; k++) {
        if (p->kind == 5) {
            _GetUnit(p->id);
            b = _GetMoveInfo(_Func_807a5b0(((short)p->move >> 8) & 0xf, p->move & 0xff))[3];
            if (b == 0x2e || b == 0x2f || b == 0x35)
                p->prio += 0x2710;
        }
        p++;
    }
    last = n - 1;
    t = &tmp;
top:
        swapped = 0;
        for (i = last; i > 0; i--) {
            if (a[i].prio > a[i - 1].prio) {
                copy = Func_8001af8;
                copy(t, &a[i], 0x10);
                copy(&a[i], &a[i - 1], 0x10);
                copy(&a[i - 1], t, 0x10);
                swapped++;
            }
        }
    if (swapped != 0) goto top;
}
