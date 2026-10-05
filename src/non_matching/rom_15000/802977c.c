/* Debug_FaceTest (0x0802977c) -- NON-MATCHING: 38 of 185 encodings differ.
 * Reference 185 instructions, ours 185; SIZE and RELOCATIONS IDENTICAL, so the
 * 38 is a TRUE DISTANCE.  Figure re-derived in batch 324; BODY UNCHANGED.
 * The DIAGNOSIS below replaces the previous one, which had the causality
 * inverted and carried three rows of evidence that measure nothing.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/802977c.c \
 *     asm/rom_15000/rom_23178_c_c_c_c.s --func Debug_FaceTest
 *
 * ================ THE 38 IS SEVEN RUNS, NOT ONE PROBLEM ================
 *
 *   A   1 enc  idx 14: `p` -- rom `mov r11, r3`, ours `mov r9, r3`
 *   B   4 enc  CreateUIBox args: the rom hoists `mov r2,#0xe` to just after
 *              `mov r3,#0x2`; we sink it below the two `str`s and `mov r1,#0xa`.
 *              Pure sched2 permutation.
 *   C  10 enc  count-loop preamble 1
 *   D  10 enc  count-loop preamble 2
 *   E   2 enc  `m` -- rom r9, ours r11 (`mov r9,r1` and `mov r2,r9`)
 *   F   7 enc  the `p + 0x12f2` load.  rom `ldr r1,=0x12f2 / mov r2,r11 /
 *              ldrh r3,[r2,r1]`; ours `ldr r3,=0x12f2 / add r3,r9 /
 *              ldrh r3,[r3,#0x0]`, plus 4 lines of argument-setup rotation.
 *              Downstream of A.
 *   G   4 enc  Func_801e7c0 args: rom `ldr r0,=0xdd2 / add r0,r5,r0 /
 *              mov r2,#0x18 / mov r3,#0x0`; ours loads into r2 and emits the
 *              two `mov`s in the other order.
 *
 * A + E + F = 10 are the register swap.  B + G = 8 are argument scheduling.
 * C + D = 20 -- MORE THAN HALF -- are a THIRD, INDEPENDENT cause, and they
 * involve only the LOW registers r0/r1/r2/r5/r12.  The old header's "every one
 * of the 38 differences is downstream of that single swap" is refuted.
 *
 * NOT A POOL PROBLEM, with evidence: size and all relocations are identical at
 * 185/185, so the pool order already matches.  The only pool-touching diffs are
 * WHICH REGISTER receives `=0x12f2` and `=0xdd2`.  `.26.mach` has nothing to
 * say here.
 *
 * ================ RUNS C AND D: THE PSEUDO SPLIT ================
 *
 *   rom   mov r7,r0 / ldr r0,=Data_367e4 / mov r5,#0 / ldrsh r3,[r0,r5] /
 *         mov r2,#1 / neg r2,r2 / mov r1,#0 / cmp r3,r2 / beq L0 /
 *         mov r12,r2 / mov r2,r0
 *   ours  ldr r2,=Data_367e4 / mov r7,r0 / mov r0,#0 / ldrsh r3,[r2,r0] /
 *         mov r0,#1 / neg r0,r0 / mov r1,#0 / mov r5,#0 / cmp r3,r0 /
 *         beq L0 / mov r12,r0
 *
 * The ROM has THREE pseudos where we have TWO: the table address (r0), the -1
 * (r2, dead at `mov r12,r2`) and the walking pointer (r2 again, initialised by
 * `mov r2,r0`).  We COMBINE the table address and the walking pointer, which
 * gives that pseudo 4+ refs and a live range spanning the loop -- a
 * high-priority allocno that takes r2 (REG_ALLOC_ORDER = {3,2,1,0,12,14,4,5,
 * 6,7,8,10,9,11}, so r2 is handed out before r0) and pushes the -1 to r0.  The
 * ROM's split also frees `cur`'s (loop 1) and `n`'s (loop 2) zero to serve as
 * the `ldrsh` zero index; Thumb-1 `ldrsh` is register-offset ONLY, so a zero
 * register is mandatory and that sharing is worth the balance of the 20.
 *
 * ELEVEN SPELLINGS MEASURE EXACTLY 38, dsize 0, relocations clean -- and
 * b1's diff SHAPE in the preamble is line-for-line the base's, so this is real
 * inertness, not two defects cancelling:
 *   guard through the named pointer; `q` set before the guard; `q =
 *   &Data_367e4[0]` with `q[0]`; `q[0]` in the condition; a bare
 *   `while (*q != -1)` with no explicit guard; `Data_367e4 + 0`; two separate
 *   pointers q1/q2; `q` declared first; `q` declared last; and (both at 39)
 *   `cur = 0` before `n = 0`, `n = 0` moved next to the guard.
 * WORSE: guard subscripted by the zero-valued variable 159; index-carried
 *   count loops 152.
 *
 * ================ RUNS A/E/F: THE SWAP IS NOT A WALL ================
 *
 * `.18.greg` for this body:
 *   ;; 18 regs to allocate: 34 44 42 43 53 66 48 59 61 72 32 37 40 41 33 45 38 39
 *   ;; Register dispositions: 32 in 6  33 in 9  34 in 2  37 in 7  40 in 8
 *                             41 in 10  42 in 1  43 in 5  44 in 0  45 in 11 ...
 * Pseudo numbers follow DECLARATION order, so the map is exact: 32=k->r6,
 * 33=p->r9, 34=q->r2, 37=box1->r7, 38=box2 and 39=flag spilled, 40=n1->r8,
 * 41=total->r10, 42=n->r1, 43=cur->r5, 44=face->r0, 45=m->r11.  The high
 * callee-saved regs are handed out r8, r10, r9, r11, so the ROM's n1->r8,
 * total->r10, m->r9, p->r11 needs the allocno order `40 41 45 33`; we get
 * `40 41 33 45`.
 *
 * MEASURED, SCREENED ON `.18.greg` RATHER THAN ON THE FIGURE:
 *   `m = 2` at the very top        order `40 41 45 33`, p -> R11   figure 81
 *   `m = 2` after `flag = 1`       order `40 41 45 33`, p -> R11   figure 80
 *   `m = 2` after the box2 call    order `40 41 45 33`, p -> R11   figure 157
 *   `m = 2` after the box1 call    order `40 41 33 38 45`, p->r9   figure 149
 *   `m = 2` after the zeros        order `40 41 33 38 45`, p->r9   figure 149
 *   `m = 2` after `n1 = n`         order `40 41 33 38 45`, p->r9   figure 149
 *   `m = 2` before `total = ...`   order `40 41 33 45`,    p->r9   figure 118
 *   `m = 2` after `k = &gKeyRepeat` order unchanged,       p->r9   figure 38
 *   four declaration permutations (m first, m before p, p last, m+p first):
 *                                  orders all CHANGE, p -> r9 every time
 *
 * So `allocno_compare`'s `live_length` denominator is the discriminator, m's
 * live range has to start before p's for m to win, and THE ROM'S ASSIGNMENT IS
 * REACHABLE -- but every placement that reaches it costs 42-119 encodings in
 * the prologue, because materialising the 2 that early rewrites the argument
 * scheduling of both UI-box calls.
 *
 * THE OLD HEADER'S THREE "m ALSO USED AT Func_801ea08 AND BOTH CloseUIBox"
 * ROWS ARE NOT INERT LEVERS -- THE EDIT NEVER HAPPENED.  Passing `m` instead
 * of the literal 2 at one, two or all three of those call sites leaves
 * `.18.greg` BIT-IDENTICAL: same "regs to allocate" line, same dispositions.
 * The argument constant is rematerialised, so `REG_N_REFS` does not move
 * (docs/elevation.md's "a second name cannot change REG_N_REFS").
 *
 * AND THE CAUSALITY IS BACKWARDS.  The ROM materialises its 2 LATE --
 * `mov r1,#2 / mov r9,r1` sits after BOTH count loops, which is exactly where
 * the "after `k = &gKeyRepeat`" row puts it -- and still allocates m before p.
 * So in the ROM's compile p's priority is lower for a reason that is NOT m's
 * placement, and the only other input difference is runs C+D, whose pseudo
 * split changes the loop's register pressure and therefore the recomputed
 * `live_length`s.
 *
 * NEXT: RUNS C+D ARE THE PREREQUISITE.  Split the table address from the
 * walking pointer and re-read `.18.greg` before touching m or p again.  Do not
 * spend another round on declaration order or on extra names for m; both are
 * measured not to reach the allocator.
 *
 * ================ WHAT IS RIGHT, and is the reusable part ================
 *
 *   - The two -1-terminated 4-byte-record tables are counted with a GUARDED
 *     do-while walking a POINTER (`q += 2`), the opposite carrier from
 *     src/non_matching/rom_15000/8019d2c.c, which needs an index in the same bank.
 *   - n1 and the loop accumulator are DIFFERENT VARIABLES: the ROM's `mov r8, r1`
 *     after the first loop is the copy from the shared accumulator into n1's home.
 *     One variable for both costs a `mov r3,#1 / add r8,r3` inside loop 1.
 *   - `n = 0;` BEFORE `cur = 0;` is worth 1 (39 -> 38).
 *   - Func_801ea08 must be declared `void`, not `int`: the ROM writes r0 FIRST
 *     among the argument moves. 41 -> 38. This is the batch-286 return-type lever
 *     running in the OPPOSITE direction from its usual use.
 *   - gKeyRepeat's address must be lifted into a `volatile unsigned int *k` local
 *     before the loop (the rom_1aeec_a_a_c_a_c_b.c lever); named directly it is
 *     reloaded per test.
 *   - THE r9 CONSTANT CANNOT BE A loop.c HOIST -- ARITHMETIC, NOT A GUESS. The
 *     .08.loop dump for the no-m version says "Insn 253: regno 89 (life 1),
 *     move-insn savings 1 not desirable" with "possible biv, reg 89, const =2",
 *     and the loop is "Loop from 159 to 395: 90 real insns". move_movables needs
 *     threshold*savings*lifetime >= insn_count with threshold = (has_call ? 1 : 2)
 *     * (1 + n_non_fixed_regs) ~ 15; 15*1*1 = 15 < 90 unconditionally. So the
 *     ROM's hoisted 2 is a SOURCE VARIABLE, which is what m is.
 */

#include "gba/types.h"

extern s16 Data_367e4[];
extern s16 Data_3680c[];
extern volatile unsigned int gKeyRepeat;
extern unsigned char *iwram_3001e8c;

extern int Func_8019da8(int a, int b, int c, int d);
extern int CreateUIBox(int x, int y, int w, int h, int opts);
extern void CloseUIBox(int box, int mode);
extern int Func_8016478(int box);
extern int LoadPortrait(int id, int b, int *v, int *t, int e, int f);
extern void Func_801ea08(int a, int b, int box, int d, int e);
extern int Func_801e7c0(int id, int box, int x, int y);
extern int WaitFrames(int n);

int Debug_FaceTest(void)
{
    volatile unsigned int *k;
    unsigned char *p;
    s16 *q;
    int v;
    int t;
    int box1, box2;
    int flag;
    int n1, total;
    int n;
    int cur;
    int face;
    int m;

    p = iwram_3001e8c;
    flag = 1;
    box2 = Func_8019da8(0, 0, 0xa, 5);
    box1 = CreateUIBox(0xa, 0xa, 0xe, 3, 2);
    n = 0;
    cur = 0;
    if (Data_367e4[0] != -1) {
        q = Data_367e4;
        do {
            q += 2;
            n++;
        } while (*q != -1);
    }
    n1 = n;
    n = 0;
    if (Data_3680c[0] != -1) {
        q = Data_3680c;
        do {
            q += 2;
            n++;
        } while (*q != -1);
    }
    total = n + n1;
    m = 2;
    k = &gKeyRepeat;
    while (1) {
        if (*k & 0x20) {
            flag = 1;
            cur--;
        }
        if (*k & 0x10) {
            flag = 1;
            cur++;
        }
        if (*k & 0x200) {
            flag = 1;
            cur -= 10;
        }
        if (*k & 0x100) {
            flag = 1;
            cur += 10;
        }
        if (*k & 1)
            break;
        if (*k & m)
            break;
        if (flag != 0) {
            flag = 0;
            cur = (cur + total) % total;
            Func_8016478(box1);
            if (cur < n1)
                face = Data_367e4[cur * 2 + 1];
            else
                face = Data_3680c[(cur - n1) * 2 + 1] + 0x80;
            v = *(unsigned short *)(p + 0x12f2);
            LoadPortrait(face, 0, &v, &t, 0xf, 1);
            Func_801ea08(cur, 2, box1, 0, 0);
            Func_801e7c0(cur + 0xdd2, box1, 0x18, 0);
        }
        WaitFrames(1);
    }
    CloseUIBox(box1, 2);
    CloseUIBox(box2, 2);
    WaitFrames(1);
    return 0;
}
