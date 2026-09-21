/* Func_801fe2c (0x0801fe2c) -- NON-MATCHING, 35 differing of 110.
 * Blocker class: TWO MUTUALLY EXCLUSIVE SOURCE FORMS, decided by pass order.
 * UNREACHABLE from C on the addressing residue -- evidence below.
 * Never attempted before batch 276.
 *
 * asm/rom_15000/rom_1fe2c_a.s (1 function, so landing needs NO split).
 *
 * 107 instructions -- one of the two targets in batch 276's size experiment that
 * did NOT converge, and the useful thing it produced is a proof rather than a
 * number. Relocation symbols are correct and in the ROM's order; all offsets are
 * shifted, i.e. this is a length difference, not wrong symbols.
 *
 * THE MUTUAL EXCLUSION, which is the finding. An explicit `int k` index biv gives
 * the ROM's `ldrsb [k, p]` addressing -- but it makes the loop-top test and the
 * body load syntactically identical, so cse1 commons them and you gain a split
 * `ldrb` + `lsl`/`asr` pair. Writing `p[0x2c + i]` instead kills the commoning,
 * but then loop.c folds `p` into the giv's additive term and the addressing is
 * lost. vA = 41, vB = 36, and NO SPELLING AMONG 22 GETS BOTH.
 *
 * UNREACHABLE-WITH-EVIDENCE on the addressing half. `t3_c3.c.08.loop`:
 *
 *     Insn 31: giv reg 41 src reg 38 benefit 2 lifetime 2 replaceable ncav mult 1 add (reg/v:SI 35)
 *     Insn 33: giv reg 42 src reg 38 benefit 4 lifetime 1 replaceable mult 1 add (plus:SI (reg/v:SI 35)
 *     giv of insn 33 not worth while, 34 vs 67.
 *     giv at 31 reduced to (reg:SI 130)
 *
 * reg 35 is the `p` parameter. With `p[0x2c + i]`, loop.c ALWAYS folds the
 * invariant `p` into the giv's additive term, so `p` can never survive as a
 * separate base register and the ROM's `ldrsb r0, [k, p]` is unreachable from
 * that spelling. Not a spelling gap -- a pass property.
 *
 * AND THE SIGN-EXTEND RESIDUE IS CREATED IN combine, not in the source.
 * `vB.c.13.combine` shows `ashiftrt` 5 -> 1 and `mem:QI` 5 -> 6 across that one
 * pass: combine fuses four of five `(sign_extend (mem))` into `ldrsb` and cannot
 * fuse the fifth, because loop and gcse have left the load and its shifts in
 * DIFFERENT BASIC BLOCKS.
 *
 * REMAINING DIFFERING PAIRS (measured on vB, the clean 36):
 *   sign-extend split   3  rom `ldrsb r0,[r1,r2]` in the body / ours `ldrb r0,[r4,r1]`
 *                          in the bottom block plus `lsl r0,#0x18` / `asr r0,#0x18`
 *   coordinate sums     4  rom `ldr r1,[sp,#0xc]` before the `ldrh`, then
 *                          `add r3,r1,r3` three-operand / ours `ldrh` first, then
 *                          `add r3,r2` two-operand. Same for the y pair.
 *   `k++` placement     2  rom inside the increment group / ours after `str r2,[sp,#4]`
 *   tail schedule       2  rom `lsl r1,#0x4` before `ldr r0,=Func_801ff58` / ours after
 *   register choice    ~3  rom `mov r1,#0xd` / ours `mov r4,#0xd`, and knock-ons
 *
 * MEASURED, about 22 candidates (rom 110):
 *   8 structures, vA..vH                 41, 36, 42, 55, 52, 63, 69, 77
 *   x2 (4th parameter `unsigned int`,
 *       walking value pointer-typed)     35   <- best, below
 *   a `struct W` for the four arrays      36   (inert)
 *   7 expression spellings -- parenthesising the x-sum, both operand orders on x
 *     and on y, naming both sums, naming the table byte, an explicit (int) cast --
 *                                        ALL SEVEN BYTE-IDENTICAL at 36
 *   -fno-gcse                             37
 *   -fno-cse-follow-jumps                 36
 *   -fno-expensive-optimizations          41
 *   -fno-rerun-cse-after-loop             42
 *   -fno-schedule-insns2                  43
 *   -fno-strength-reduce                  61
 *
 * vB's winner shape, worth keeping: an explicit `int k` index biv, the array
 * offsets left as givs of `i`, and a plain `while` -- the guard MUST come from
 * duplicate_loop_exit_test rather than a hand-written `if`, because gcse runs
 * BEFORE loop.c.
 *
 * NEXT: nothing source-level for the addressing half; it is proved out above. The
 * other three residues are worth a look only if someone finds a form that keeps
 * `p` in a register without a register offset, which the dilemma above says does
 * not exist in C.
 */
extern unsigned char *iwram_3001f2c;
extern void *_CreateSprite(void *resource);
extern void _Sprite_SetAnim(void *sprite, int anim);
extern void StartTask(void *task, int prio);
extern void Func_801ff58(void);
extern void *_Func_808b3d0(int a, int b);

struct S {
    unsigned char pad0[9];
    unsigned char b0 : 2;
    unsigned char b2 : 2;
    unsigned char b4 : 4;
    unsigned char pad1[0x1c];
    unsigned char f26;
};

struct P {
    unsigned char pad[0xc];
    unsigned short x;
    unsigned short y;
};

void Func_801fe2c(struct P *a, int b, int c, unsigned int p)
{
    unsigned char *base;
    struct S *spr;
    int i;
    signed char *k;

    base = iwram_3001f2c;
    i = 0;
    k = (signed char *)0x2c;
    while (i < 4 && *(signed char *)(k + p) != -1) {
        spr = (struct S *)_CreateSprite(_Func_808b3d0(*(signed char *)(k + p), *(signed char *)(0x33 + p)));
        if (spr != 0) {
            _Sprite_SetAnim(spr, 1);
            spr->f26 = 0;
            spr->b2 = 0;
        }
        *(void **)(base + 0x114 + i * 4) = spr;
        *(short *)(base + 0x134 + i * 2) = ((b + a->x + i * 3) << 3) + 0x10;
        *(short *)(base + 0x144 + i * 2) = ((a->y + c) << 3) + 0x10;
        *(int *)(base + 0x154 + i * 4) = 0x80 << 9;
        i++;
        k++;
    }
    StartTask(Func_801ff58, 0xc8 << 4);
}
