/* Anim_Venus -- 0x080e0564, first of the four functions in
 * asm/rom_c9000/rom_e0564_a_a.s (Anim_Venus, Anim_Mars, Anim_Hail, Anim_Ground).
 *
 * NON-MATCHING, 22 of 381 encodings differ.
 * SIZE and INSTRUCTION COUNT are both the ROM's (860 bytes, 381 encodings), so
 * objcmp's 22 IS a true distance here; tools/aligncmp.py reads 95.8% aligned-equal.
 * objcmp verbatim:
 *     XX ENCODINGS differ in 22 place(s) (ref 381, ours 381)
 *        first at index 38: ref 6a2d  ours 9306
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/e0564_Anim_Venus.c \
 *     asm/rom_c9000/rom_e0564_a_a.s --func Anim_Venus
 *   (and for the residue view, tools/aligncmp.py with the same two paths plus
 *    `Anim_Venus -v`)
 *
 * SPLIT SHAPE IF IT LANDS: Anim_Venus is the FIRST function of rom_e0564_a.s, so
 * it needs only a two-way split -- `_a_b.c` for the function and `_a_c.s` for
 * Anim_Mars + Anim_Hail + Anim_Ground -- EXCEPT that Anim_Hail (this batch,
 * MATCHING) splits the same stem.  Landing both gives
 *   src/rom_c9000/rom_e0564_a_a.c  Anim_Venus
 *   asm/rom_c9000/rom_e0564_a_b.s  Anim_Mars
 *   src/rom_c9000/rom_e0564_a_c.c  Anim_Hail  (renumbered from _a_b.c)
 *   asm/rom_c9000/rom_e0564_a_d.s  Anim_Ground
 * `python3 tools/datacheck.py asm/rom_c9000/rom_e0564_a_a.s` is SILENT; no data
 * moves and no new exports are needed (Data_ede48 lives in rom_eda78.s).
 *
 * ================================================================
 * FIVE LEVERS TOOK IT 312 -> 22, AND THE ORDER MATTERS
 * ================================================================
 *
 * 312 of 381 (65.9% aligned, 8 instructions short)
 *   Pass 1 off src/rom_c9000/rom_d82b0_b.c (Anim_Break): same bank, same
 *   `iwram_3001eec` walk, same Random/sin/cos particle loop, same Data_ede48
 *   blit.  The RELOCATION SEQUENCE was already identical on pass 1 and stayed so.
 *
 * 169 -> 112 (76.4%)  ONE COUNTER ACROSS *FOUR* LOOPS, not three.
 *   Anim_Break records "a counter reused across several loops is one variable".
 *   Here the ROM keeps r10 across the 0x20-slot init loop, the 0x200-slot `-1`
 *   loop, the TEN-target loop and the 0x200-particle draw loop -- so the
 *   per-target `i` and the sweep `j` are THE SAME VARIABLE.  Splitting them cost
 *   r9 for `base`, r11 for `frame` and the whole spill map.
 *
 * 112 -> 95 (80.1%)  THE SPILL MAP IS REVERSE DECLARATION ORDER.
 *   Ascending slot = LAST declared first.  The ROM's 0x8..0x24 run is
 *   k, boff, ang, gfx, blitA, blitB, frame, ctx, so the declarations go
 *   ctx, frame, blitB, blitA, gfx, ang, boff with `k` innermost.  Eight slot
 *   numbers land in one edit.
 *
 * 95 -> 58 (87.9%)  THE ids LOOP IS A `while`, NOT `if (c) { do } while (c)`.
 *   This is the sharpest measurement in the file and it fixed TWO things at once:
 *   `duplicate_loop_exit_test` runs AFTER gcse, so the guard it copies is never
 *   gcse'd -- which is why the ROM's guard reads the state pointer with the
 *   INDEXED form `mov r7,r9 / ldr r3,[r7,r3]` and a pool load of 0x7828 all its
 *   own, while the loop body gets a SEPARATE `ldr r5,=0x7828 / add r5,r9` hoisted
 *   into the preheader.  The `if`-guarded do-while commons the two (one pool
 *   load, one address) and cannot be made to diverge.  It also brought SIZE and
 *   COUNT to the ROM's for the first time.
 *
 * 60 -> 33 (93.2%)  NAME THE SECOND Random() RESULT.
 *   The ROM interleaves the `g->x = p->x << 16` store INTO the `mag` computation
 *   (`bl Random / mov r1,r8 / ldr r3,[r1] / lsl r3,#16 / str r3,[r7] / ldr r5,=0x1ff
 *   / ... / and r5,r0`), which is only reachable if the call's result survives the
 *   store in r0.  Written `mag = (Random() & 0x1ff) + 0x100;` gcc consumes r0
 *   first and takes r0 for the pointer copy, so the store can never move up.
 *   `int rv = Random(); g->x = p->x << 16; mag = (rv & 0x1ff) + 0x100;` puts the
 *   pointer copy in r1 and the ROM's schedule falls out.  27 encodings.
 *
 * 63 -> 60 and 33 -> 24  TWO MORE giv REWRITES, same lever as Anim_Hail's closer.
 *   The 0x20-slot init loop's base (`base + (0xe1 << 7)`) must be a
 *   strength-reduced giv, not a source-level pointer, or its `add r5,r9` is born
 *   before the hoisted 0x3f/0x68 instead of between them:
 *   `((Part *)(base + (0xe1 << 7)))[i].x = ...`.  A FULL 24-PERMUTATION SWEEP of
 *   the per-target loop's four increment statements then found `th, p, boff, i`
 *   at 24 against 35 for the worst and 33 for the shape pass 1 used -- the spread
 *   is real and the winner is not the natural order.
 *
 * 24 -> 22  THE Anim_Vine StartTask PIN.
 *   `register void *tf __asm__("r0"); tf = Task_BlitAnim; arg <<= 3;
 *   StartTask(tf, arg);` -- copied verbatim from the landed
 *   src/rom_c9000/rom_dd2ac_c_c_b.c, where the same call needs it.  Measured
 *   load-bearing: 24 without, 22 with.  ONE pin, at a fixpoint
 *   (`tools/shimcount.py` = 1; a fakematch.txt row is due if this ever lands).
 *
 * ================================================================
 * THE BLOCKER: 22 ENCODINGS, ALL LOW-REGISTER TIES IN ONE BASIC BLOCK
 * ================================================================
 *
 * Every role register is the ROM's -- r9 base, r10 the shared counter, r11 th,
 * r8 p, r5/r6/r7 as the ROM uses them, the whole spill map, every branch target,
 * every relocation.  What is left is WHICH low register stages three short-lived
 * values, and three one-slot scheduling ties:
 *
 *   ref  ldr r2,=gBuffer   / movs r3,#0x80 (<<7) / movs r0,#0x80 (<<1)
 *   ours ldr r3,=gBuffer   / movs r2,#0x80 (<<7) / movs r2,#0x80 (<<1)
 *
 * The 0x80 pair sits in ONE basic block (the particle-init loop body has no
 * branches), so this is local_alloc's `find_free_reg` walking
 * REG_ALLOC_ORDER {3,2,1,0,...}: the ROM spends r3 on the first constant and then
 * r0 -- leaving r2 UNUSED IN THAT LOOP ENTIRELY -- while we take r2 twice.  The
 * remaining three are single-position sched2 ties: `str r3,[sp,#0x18]` one insn
 * early (blitA's store, ref pairs the two `tbl[7]`/`tbl[8]` loads first),
 * `adds r5,#22` one insn early, and `mov r4,sl` vs `mov r2,sl` for `i & 1`.
 *
 * MEASURED INERT, so do not repeat them (all 22, byte-identical to this file):
 *   - operand order on `(Random() & 0x7fff) + 0x4000` and on the `+ 0x100`
 *     (both directions, 3 combinations);
 *   - `g = (Part *)(boff + (char *)gBuffer)` instead of `(char *)gBuffer + boff`;
 *   - `a2` / `rv` / `mag` declared at function scope instead of in the loop body;
 *   - `k` declared before `g` in the inner block;
 *   - `(i & 1) != 0` instead of `i & 1`.
 * MEASURED WORSE: DrawFn returning void (27), BuildDraw2DFuncEx void (31),
 *   LoadVFXFile returning int (43), `gBuffer[i]` indexing in the DRAW loop (182,
 *   and 10 instructions long -- the walking pointer is right there), the
 *   particle-init loop indexed (265), `mag` split into mask-then-add in any of
 *   three placements (187).
 *
 * The next thing to try is the documented scratch-register pin on the 0x80
 * constants ("one scratch-register pin can settle two distant clusters"), which
 * was not attempted here because the pin budget was being kept at one.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x;
    int y;
    int z;
    int vx;
    int vy;
    int vz;
    int t;
} Part;

extern int *iwram_3001eec[];
extern int ewram_2010018;
extern Part gBuffer[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80e3908(Part *g, int a, int b);
extern void UpdateScreenShake(int w, int h);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Venus(void *context)
{
    void *ctx;
    int frame;
    DrawFn blitB;
    DrawFn blitA;
    unsigned char *gfx;
    int ang;
    int boff;
    unsigned char *base;
    int **tbl;
    int **pp;
    int i;
    int th;
    Part *p;
    int arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    gfx = (unsigned char *)tbl[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BLDALPHA = 0x1010;
    BuildDraw2DFuncEx(0x2e, 7, 7, 0xb, 2);
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
    blitA = (DrawFn)tbl[7];
    blitB = (DrawFn)tbl[8];
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_94, base, 1, 1);
    LoadVFXFile(FILE_6f, base + (0xbe << 2), 1, 0);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    {
        register void *tf __asm__("r0");
        tf = (void *)Task_BlitAnim;
        arg <<= 3;
        StartTask(tf, arg);
    }
    {
        i = 0;
        do {
            ((Part *)(base + (0xe1 << 7)))[i].x = Random() & 0x3f;
            ((Part *)(base + (0xe1 << 7)))[i].y = 0x68;
            i++;
        } while (i != 0x20);
    }
    {
        int *q = &ewram_2010018;
        i = 0;
        do {
            i++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (i != (0x80 << 2));
    }
    _PlaySound(0x8d);
    frame = 0;
    ang = 0x80 << 8;
    do {
        if (frame <= 0x4f) {
            int x;
            int y;

            x = ((sin(ang) * 24) >> 16) + 0x16;
            y = (((0x40 - frame * 2) * cos(ang)) >> 16) + 0x1d;
            blitB(ctx, base, x, y, 0x14, 0x26);
        }
        if (frame == 0x38) {
            _Func_80bd7dc(0x85);
        }
        i = 0;
        boff = 0;
        th = 0x10;
        p = (Part *)(base + (0xe1 << 7));
        do {
            if (frame >= th) {
                blitA(ctx, base + (0x9e << 4), p->x - 0x11, p->y - 0x20, 0x22, 0x41);
                if (frame == th) {
                    Part *g;
                    int k;

                    g = (Part *)((char *)gBuffer + boff);
                    k = 0;
                    do {
                        int a2 = (Random() & 0x7fff) + (0x80 << 7);
                        int rv = Random();
                        int mag;
                        g->x = p->x << 16;
                        mag = (rv & 0x1ff) + (0x80 << 1);
                        g->y = (p->y + 0x10) << 16;
                        g->vx = (sin(a2) * mag) >> 7;
                        g->vy = (cos(a2) * mag) >> 6;
                        g->t = (Random() & 0xf) + 0x20;
                        k++;
                        g++;
                    } while (k != 0x10);
                    if (i & 1) {
                        _PlaySound(0x85);
                    }
                    *(int *)(base + 0x77a8) = 4;
                    k = 0;
                    while (k != (*(State **)(base + 0x7828))->f14) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[k], 7, 5, k, 6);
                        _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[k], 6);
                        k++;
                    }
                }
                p->y -= 0xc;
            }
            th += 4;
            p = (Part *)((char *)p + 0x1c);
            boff += 0xe0 << 2;
            i++;
        } while (i != 0xa);
        {
            Part *g = gBuffer;
            i = 0;
            do {
                if (g->t != -1) {
                    int n = g->t / 16 + 2;
                    unsigned char *src = gfx + Data_ede48[n - 1];
                    int w2 = n * 2;
                    int x = *(short *)((char *)g + 2) - n / 2;
                    int y = *(short *)((char *)g + 6) - n;
                    blitB(ctx, src, x, y, n, w2);
                    Func_80e3908(g, 0x3e, 0x80 << 6);
                    g->t -= 1;
                }
                i++;
                g++;
            } while (i != (0x80 << 2));
        }
        UpdateScreenShake(4, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        ang -= 0x800;
        frame++;
    } while (frame != 0x60);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
