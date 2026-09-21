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
