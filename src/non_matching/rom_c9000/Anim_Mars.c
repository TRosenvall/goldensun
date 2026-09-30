/* Anim_Mars -- 0x080e08c0, the SECOND of the two functions left in
 * asm/rom_c9000/rom_e0564_a_a.s (Anim_Venus is the first and is parked as
 * src/non_matching/rom_c9000/Anim_Venus.c at 22 of 381).
 *
 * NON-MATCHING, 37 of 426 encodings differ.
 * SIZE is the ROM's (964 bytes = 964) and INSTRUCTION COUNT is the ROM's
 * (426 = 426), and objcmp prints NO RELOCATIONS line -- all 49 symbols in the
 * ROM's order AND at the ROM's pool offsets.  So 37 IS A TRUE DISTANCE.
 * tools/aligncmp.py reads 391 of 426 aligned-equal (91.8%), 37 differing in
 * 13 hunks.
 * objcmp verbatim:
 *     XX ENCODINGS differ in 37 place(s) (ref 426, ours 426)
 *        first at index 91: ref 49b2  ours 4cb2
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Mars.c \
 *     asm/rom_c9000/rom_e0564_a_a.s --func Anim_Mars
 *   (residue view: tools/aligncmp.py with the same two paths plus `Anim_Mars -v`)
 *
 * SPLIT SHAPE IF IT LANDS.  `python3 tools/datacheck.py
 * asm/rom_c9000/rom_e0564_a_a.s` is SILENT -- the file carries no data section,
 * so this is a plain TEXT split with NO new exports.  Anim_Mars is the LAST of
 * the two functions, so landing it alone gives
 *   asm/rom_c9000/rom_e0564_a_a_a.s   Anim_Venus   (unchanged asm)
 *   src/rom_c9000/rom_e0564_a_a_b.c   Anim_Mars    (this file)
 * and stage1.ld's two `asm/rom_c9000/rom_e0564_a_a.o(...)` lines (.text and the
 * .rodata line under it) each become two, in that order.  Landing Anim_Venus at
 * the same time collapses this to the shape its own park documents.
 * Data_edeb2 / Data_ede9f / Data_edeab / Data_edea5 / Data_ede48 all live in
 * asm/rom_c9000/rom_eda78.s (`.incdata`) and are already read as externs by the
 * landed src/rom_c9000/rom_d9ab8_c_c_c_c_a_b.c (Anim_Flare), so no asm edit.
 * SHIMS: none -- `python3 tools/shimcount.py` is clean, no fakematch.txt row.
 *
 * ================================================================
 * SIX LEVERS TOOK IT 381 -> 37, AND THE FIRST ONE IS WORTH 329 OF THEM
 * ================================================================
 *
 * 381 of 426 (67.6% aligned, 10 instructions LONG)
 *   Pass 1, written from three oracles read before a line of code: the landed
 *   src/rom_c9000/rom_e0564_a_b.c (Anim_Hail, same original .s), the landed
 *   src/rom_c9000/rom_d9ab8_c_c_c_c_a_b.c (Anim_Flare, same four Data_ede
 *   tables) and src/non_matching/rom_c9000/Anim_Jupiter.c (same `fns[2]` /
 *   `fp[1]` / `boff` frame).  The RELOCATION SEQUENCE was identical on pass 1.
 *
 * 381 -> 52 (89.7%)  THE ids LOOP'S COUNTER IS THE SAME VARIABLE AS `i`.
 *   > The family rule is "pointers split, counters unify", and here the
 *   > unification reaches FOUR loops, not three: the 9-slot setup loop, the
 *   > 9-slot blit loop, the 0x90-particle draw loop AND the Func_80d6888 /
 *   > _SetBattleActorKnockback loop all share ONE `i`.  Only the 0x10-particle
 *   > inner loop gets its own counter (`k`).
 *   Writing the ids loop over a separate `k` cost r7: `i` went to r8 and every
 *   one of its ~20 references grew a `mov rX, r8` staging copy, which is where
 *   the ten extra instructions came from.  SIZE and COUNT both became the ROM's
 *   with this one edit.
 *
 * 52 -> 49  OPERAND ORDER ON THE FIRST PARTICLE STORE.
 *   `qq->x = ((Random() & 0xf) + p->x - 8) << 16;` -- the mask FIRST.  The ROM
 *   stages `p->x` in r2 and the mask in r3 (`ldr r2,[r6] / movs r3,#15 /
 *   ands r3,r0 / adds r3,r2`); `p->x + (Random() & 0xf) - 8` gives the two
 *   registers the other way round.  The add is commutative so only the source
 *   order decides.
 *
 * 49 -> 42  THE DRAW LOOP'S `- n / 2` MUST BE INLINE IN ARGUMENT 3.
 *   This is Anim_Flare's pool-ORDER lever in its other form.  The ROM computes
 *   `((short *)g)[1] + ((sin(g->z) * 4) >> 16)` as a statement (it has to -- a
 *   call sits in it) and then subtracts `n / 2` INSIDE the argument, so
 *   `Data_ede48` gets its `ldr` before the subtraction:
 *       int sx = ((short *)r)[1] + ((sin(r->z) * 4) >> 16);
 *       fp[1](ctx, vfx + Data_ede48[n - 1], sx - n / 2, ...);
 *   Folding the `- n/2` into `sx` moves four encodings.  `n / 2` stays a plain
 *   signed divide: combine knows `(i & 1) + 3` has its sign bit clear and emits
 *   the ROM's `lsr`, so no unsigned type is needed.
 *
 * 42 -> 40  NAME THE 0xffff MASK, AND IT IS A POOL-ORDER FIX.
 *   > A CONSTANT THAT NEEDS A POOL WORD IS ORDERED BY THE FINAL INSTRUCTION
 *   > STREAM, NOT BY THE SOURCE -- so a hoisted invariant and a source-level
 *   > pointer init in the same preheader compete for the first `ldr`, and the
 *   > loser's pool word moves.
 *   With `Random() & 0xffff` written inline twice, loop.c hoists the constant as
 *   a movable, which is BORN AFTER every source statement in the preheader, so
 *   `ldr r3,=gBuffer` (from `qq = (Part *)((char *)gBuffer + boff)`) gets the
 *   first `ldr` and the two pool words come out swapped -- objcmp reported
 *   `RELOCATIONS differ` on gBuffer's offset alone, plus four consequential
 *   `ldr [pc,#N]` encodings at three distant sites and the pool's tail padding.
 *   Assigning the mask to a named local BEFORE the pointer init
 *       msk = 0xffff;
 *       qq = (Part *)((char *)gBuffer + boff);
 *       k = 0;
 *   puts its `ldr` first, and the relocation line disappears.  The cost is that
 *   `msk` then has a longer live range than the 0x7f movable, so allocno_compare
 *   ranks it lower and the two invariants swap r9 and fp -- four encodings back.
 *   Net +2, and it is the only spelling found that fixes the pool.  Naming the
 *   0x7f as well, in three placements, is INERT (all 37).
 *
 * 40 -> 38  `StartTask(Task_BlitAnim, 0x90 << 3)` WITHOUT THE `arg` LOCAL.
 *   Anim_Hail needs `arg = 0x90; arg <<= 3;`; Anim_Mars does not, and the
 *   difference is one position of `ldr r0,=Task_BlitAnim`.
 *
 * 38 -> 37  THE OUTER LOOP'S INCREMENTS GO `i++ / p++ / boff += 0xe0 << 1`.
 *   All six permutations were measured: `i,p,boff` 37, the natural
 *   `boff,i,p` 38, `boff,p,i` and `i,boff,p` 39 (as 42/42/41 on the earlier
 *   base).  Unlike Anim_Jupiter's k-loop, where every permutation was
 *   byte-identical, here the spread is real but small.
 *
 * ALSO LOAD-BEARING, from pass 1 and never moved:
 *   - `pp = g; base = *pp++; ctx = *pp; vfx = g[2];` for `ldmia r3!, {r1}`.
 *   - `fp = fns;` assigned BEFORE `BuildDraw2DFuncs(0, (void **)fp)`, then
 *     `fns[0](...)` through the array and `fp[1](...)` through the pointer --
 *     the ROM reads sp+0x18 directly for one and sp+0xc then [r0,#4] for the
 *     other, in the SAME basic block, so this is not a style choice.
 *   - THE DECLARATION LIST IS THE FRAME MAP, DESCENDING: fns 0x18, ctx 0x14,
 *     vfx 0x10, fp 0xc, boff 0x8, frame `sub sp, #0x20`.
 *   - `if (frame >= 0x14 && frame <= 0x1f)` for the ROM's
 *     `sub r3,#0x14 / cmp r3,#0xb / bhi` (fold makes the unsigned test).
 *   - `if ((unsigned)life <= 0x2f)` around a SIGNED `life / 8` -- Anim_Flare's
 *     rule, unchanged.
 *   - THE BLIT ARGUMENT BLOCK IS ANIM_FLARE'S EMBEDDED ASSIGNMENT VERBATIM:
 *     `q->x - ((w = Data_ede9f[idx]) >> 1)` with `unsigned char w`, which is
 *     what orders Data_edeb2 before Data_ede9f in the pool.
 *   - `fp[1](ctx, base, 0x92 - frame * 4, frame * 8 - 0xac, 0x14, 0x28)` with
 *     BOTH terms inline.  `frame * 8 - 0xac` is strength-reduced into the ROM's
 *     r9 giv (`movs r4,#0xac / negs r4,r4`, then `add r9,r2` with r2 = 8) while
 *     `0x92 - frame * 4` is NOT reduced -- asymmetric, and the asymmetry is
 *     gcc's, not the source's: replacing the y term with an explicit
 *     `yy = -0xac; ... yy += 8;` counter reads 41, four WORSE.
 *   - `copy = Func_8001af8; copy((volatile u16 *)pal, data, 0x80);` with
 *     `pal = 0xa0; ... pal <<= 19;` -- src/rom_c9000/rom_e0524.c's indirect-call
 *     idiom, needed for `ldr r3,=Func_8001af8 / bl _call_via_r3`.
 *   - `while (i != (*(State **)(base + 0x7828))->f14)` for the ids loop, with
 *     base + 0x7828 RE-DERIVED inside (three pool words, as Anim_Venus records).
 *
 * MEASURED INERT, do not repeat (all 37, byte-identical to this file):
 *   - `qq++; k++;` instead of `k++; qq++;`
 *   - `int msk;` declared first in the list instead of last
 *   - `(frame - 0x18) * 8 + 0x14` instead of `frame * 8 - 0xac`
 *   - naming the 0x7f mask, in three placements (before msk, after msk, after
 *     the `k = 0`)
 * MEASURED WORSE:
 *   - the inner particle loop INDEXED so its pointer becomes a giv
 *     (`qq = (Part *)((char *)gBuffer + boff) + k`): 93, and it kicks `base`
 *     out of sl back into r9, taking the whole function with it.  This is the
 *     Anim_Hail preheader-transposition lever and it does NOT apply here.
 *   - `qq = ...` before `k = 0`: 44.
 *
 * ================================================================
 * THE BLOCKER: 37 ENCODINGS IN FIVE BLOCKS, ALL LOW-REGISTER TIES
 * ================================================================
 *
 * Every role register is the ROM's -- sl base, r7 the shared counter, r6 the
 * Part walker, r8 the inner counter and then frame, r9 the y giv and then the
 * 0xffff mask, fp the 0x7f mask -- every branch target, the whole spill map,
 * the relocation sequence and the pool order.  What is left is WHICH low
 * register stages a short-lived value, in five blocks, and it is ONE ROTATION:
 *
 *   inner particle loop   ref stages fp,fp,r9,r9 through r4,r1,r2,r3
 *                        ours through r3,r4,r1,r2
 *   boff increment block  ref r1/r2      ours r0/r1
 *   frame/giv init        ref r4 (0xac), r3 (0)   ours r3, r2
 *   frame staging         ref r0 then r1  ours r4 then r0
 *
 * In every block ours uses the register ONE EARLIER in
 * REG_ALLOC_ORDER {3,2,1,0,12,14,4,5,...} than the ROM, i.e. the ROM behaves as
 * if one more low register were already spoken for at each of those points.
 * Per the standing finding this is NOT an order question -- the order is
 * correct and proven -- it is an INPUT question: one additional live quantity,
 * or one different conflict edge, in local_alloc's graph.  The two remaining
 * one-position scheduling ties (`str rX,[sp,#8]` and `mov rX,r8` before the
 * `cmp`) are consequences of the same thing.
 *
 * NOT TRIED, and the cheapest next experiment: the documented scratch-register
 * pin on the first staging copy of the inner particle loop ("one
 * scratch-register pin can settle two distant clusters").  It was left alone
 * because this file is pin-free and the brief asked for pin-free.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, s32 len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern Part gBuffer[];
extern unsigned char Data_ede9f[], Data_edea5[], Data_edeab[];
extern unsigned short Data_edeb2[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80e3908(Part *p, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);
extern void Func_8001af8(volatile u16 *dst, void *src, s32 len);


void Anim_Mars(void *context)
{
    DrawFn fns[2];
    void *ctx;
    unsigned char *vfx;
    DrawFn *fp;
    int boff;
    void **g;
    void **pp;
    unsigned char *base;
    Part *p;
    Part *q;
    Part *qq;
    Part *r;
    int i;
    int k;
    int frame;
    int arg;
    int msk;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    vfx = (unsigned char *)g[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    fp = fns;
    BuildDraw2DFuncs(0, (void **)fp);
    LoadVFXFile(FILE_73, vfx, 0, 0);
    LoadVFXFile(FILE_8e, base, 1, 0);
    LoadVFXFile(FILE_b7, base + (0xc8 << 2), 1, 1);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    boff = 0;
    i = 0;
    p = (Part *)(base + (0xe1 << 7));
    do {
        int ang = i << 11;
        p->x = (sin(ang) * 24) >> 16;
        p->y = ((cos(ang) * 4) >> 16) + 0x34;
        if (i & 1) {
            p->x = 0x20 - p->x;
        } else {
            p->x = p->x + 0x20;
        }
        p->t = -(i * 2);
        msk = 0xffff;
        qq = (Part *)((char *)gBuffer + boff);
        k = 0;
        do {
            qq->x = ((Random() & 0xf) + p->x - 8) << 16;
            qq->y = ((Random() & 7) + 0x60) << 16;
            qq->vx = ((Random() & 0x7f) - 0x40) << 11;
            qq->vy = ((Random() & 0x7f) - 0x40) << 10;
            qq->z = Random() & msk;
            qq->vz = Random() & msk;
            k++;
            qq++;
        } while (k != 0x10);
        i++;
        p++;
        boff += 0xe0 << 1;
    } while (i != 9);
    _PlaySound(0x88);
    frame = 0;
    do {
        if (frame == 0x38) {
            _Func_80bd7dc(0x85);
        }
        if (frame <= 0x17) {
            fns[0](ctx, base + frame / 4 * 0x640 + (0xc8 << 2), 0x28, 0x14, 0x28, 0x28);
        }
        if (frame == 0x14) {
            u32 pal;
            CopyFn copy;
            void *data = GetFile(FILE_8e);
            pal = 0xa0;
            copy = Func_8001af8;
            pal <<= 19;
            copy((volatile u16 *)pal, data, 0x80);
        }
        if (frame >= 0x14 && frame <= 0x1f) {
            if (frame > 0x17) {
                fp[1](ctx, base, 0x92 - frame * 4, frame * 8 - 0xac, 0x14, 0x28);
            } else {
                fns[0](ctx, base, 0x32, 0x14, 0x14, 0x28);
            }
        }
        if (frame == 0x20) {
            _PlaySound(0x91);
            *(int *)(base + 0x77a8) = 8;
            LoadVFXFile(FILE_b4, base, 1, 1);
        }
        if (frame > 0x1f) {
            i = 0;
            q = (Part *)(base + (0xe1 << 7));
            do {
                int life = q->t;
                if ((unsigned)life <= 0x2f) {
                    int idx = life / 8;
                    unsigned char w;
                    fns[0](ctx, base + Data_edeb2[idx],
                           q->x - ((w = Data_ede9f[idx]) >> 1),
                           q->y + Data_edeab[idx], w, Data_edea5[idx]);
                }
                q->t = q->t + 1;
                i++;
                q++;
            } while (i != 9);
        }
        r = gBuffer;
        i = 0;
        do {
            if (frame >= i / 16 * 2 + 0x28) {
                int n = (i & 1) + 3;
                int sx = ((short *)r)[1] + ((sin(r->z) * 4) >> 16);
                fp[1](ctx, vfx + Data_ede48[n - 1], sx - n / 2, ((short *)r)[3] - n, n, n * 2);
                Func_80e3908(r, 0x40, -0x2000);
                r->z += 0x80 << 4;
                if (r->z > 0xffff) {
                    r->z -= 0xffff;
                }
            }
            i++;
            r++;
        } while (i != 0x90);
        if (frame == 0x26) {
            i = 0;
            while (i != (*(State **)(base + 0x7828))->f14) {
                Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 0x10);
                _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[i], 6);
                i++;
            }
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x70);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
