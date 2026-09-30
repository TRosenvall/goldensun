/* Anim_Ground -- 0x080e1040, 561 instructions.  PARKED, size and count EXACT.
 *
 * NON-MATCHING, 226 of 588 encodings differ.
 *
 * MEASUREMENT.  objcmp prints NO SIZE line: the byte SIZE matches exactly and
 * the instruction COUNT matches exactly (588 = 588), so 226 IS A TRUE DISTANCE
 * and not a saturated figure.  aligncmp (position-tolerant, masks nothing)
 * reads 435 of 588 aligned-equal = 74.0%, 169 differing/ins/del in 90 hunks --
 * so the residue is dominated by ONE systematic offset error, not by 226
 * independent mistakes (see THE BLOCKER below).
 *
 * objcmp DOES print a RELOCATIONS line, and read as the two separate findings
 * the discipline requires it is small and it is the SAME allocation story:
 *   - SYMBOL SEQUENCE, 7 rows of 56 differ.  FIVE are `_call_via_rN` veneer
 *     REGISTERS (ref r4,r5,r5,r5,r5 against ours r7,r4,r7,r7,r9 at sequence
 *     indices 19, 26, 27, 28, 41).  This is NOT the recorded ".call_via IS AN
 *     INLINE VENEER gcc NEVER EMITS" structural blocker -- both streams emit a
 *     veneer at every one of the six indirect calls, and only the register
 *     differs.  The remaining TWO are one TRANSPOSITION of a pool pair:
 *     sequence index 37/39 is .Leeca1 then .Leec74 in the ROM and .Leec74 then
 *     .Leeca1 in ours, i.e. the ROM pools the arg-4 table before the arg-3 one
 *     in the first sprite call.  No symbol is missing and none is extra.
 *   - OFFSETS: exactly FIVE rows shift, all by -2 bytes, and only between
 *     Func_80e3944 (ref 0x27a, ours 0x278) and gfree (ref 0x3c2, ours 0x3c0).
 *     Everything before and everything after -- Task_BlitAnim at 0x518
 *     included -- agrees, so this is ONE 16-bit instruction moved and moved
 *     back, the insert/delete pair aligncmp reports at the particle-init loop
 *     tail, not a length error.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Ground.c \
 *     asm/rom_c9000/rom_e0564_a_c.s --func Anim_Ground
 *
 * SPLIT SHAPE: NONE NEEDED, AND NO NEW EXPORTS.  This is the cheapest landing
 * shape in the family.  asm/rom_c9000/rom_e0564_a_c.s holds Anim_Ground ALONE
 * (it is already the third split of the rom_e0564_a stem, cut when Anim_Hail
 * landed as src/rom_c9000/rom_e0564_a_b.c).  `tools/datacheck.py
 * asm/rom_c9000/rom_e0564_a_c.s` is SILENT and exits 0 -- the file carries no
 * data section of its own -- so converting it deletes the .s outright and
 * nothing is stranded.  All six .rodata tables it reads (.Leec70, .Leec74,
 * .Leec7d, .Leec86, .Leec98, .Leeca1) are ALREADY `.global` in
 * asm/rom_c9000/rom_e0564_c.s, so NOT ONE ASM EDIT IS REQUIRED.
 *
 * `tools/split_s.py` was deliberately NOT run: it rewrites the .s files and the
 * linker script in place, and this was measured from a read-only workspace.
 * The landing edit is the two stage1.ld lines that name this object --
 * line 1902 `asm/rom_c9000/rom_e0564_a_c.o(.text)` and line 1984
 * `asm/rom_c9000/rom_e0564_a_c.o(.rodata)` -- each becoming
 * `src/rom_c9000/rom_e0564_a_c.o(...)`.  BOTH lines belong to this function:
 * the recorded batch-300 hazard (a second function in the file owning the
 * .rodata line) does not apply, because there is no second function and no
 * .rodata.  The .rodata line is vestigial, kept to preserve the stem's shape,
 * exactly as Anim_Hail's landed note describes for its own piece.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE, no
 * barriers, no per-file flag override, so no fakematch.txt row is needed.
 *
 * ================================================================
 * THE TABLES, AS READ OFF THE ROM
 * ================================================================
 *
 * Nine sprite entries, parallel arrays in asm/rom_c9000/rom_e0564_c.s:
 *   .Leec74  0xeec74..0xeec7d   9 bytes   width
 *   .Leec7d  0xeec7d..0xeec86   9 bytes   height
 *   .Leec86  0xeec86..0xeec98  18 bytes   9 HALFWORDS, a byte offset into base
 *   .Leec98  0xeec98..0xeeca1   9 bytes   x offset
 *   .Leeca1  0xeeca1..0xeecaa   9 bytes   y offset
 * plus .Leec70 0xeec70..0xeec74, 4 bytes, indexed by `Random() & 3` -- the
 * spark's flag word.  Every one is read with `ldrb` (`ldrh` for .Leec86) and
 * never sign-extended, so all are UNSIGNED.
 *
 * Two arrays live in the VFX scratch block at `base`:
 *   lanes[8]    at base + (0xe1 << 7) = base + 0x7080, stride 0x1c, 7 ints
 *   sparks[8][4] at base + (0xe8 << 7) = base + 0x7400, stride 0x1c
 * The lane loop runs 6 of the 8 lanes; the spark init loop runs all 0x20.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * (1) THE ORACLE, BEFORE WRITING A LINE: src/rom_c9000/rom_e0564_a_b.c
 *     (Anim_Hail) is the SAME STEM and matching.  Copying its
 *     `tbl = (char **)iwram_3001eec; pp = tbl; base = *pp++; ctx = *pp;`
 *     prologue, its `Anim_Djinni(context, N, (*slot)->f4, 2, &cx, &cy)` call,
 *     its `X = ((-sin(ang) * 4) >> 16) + cx / 2 - K` / `Y = ((cos(ang) * 2)
 *     >> 16) + cy - K` pair, its `arg = 0x90; arg <<= 3; StartTask(...)` idiom
 *     and its `*(State **)(base + 0x7828)` INLINE-inside-the-loop rule put the
 *     FIRST candidate at 230 of 588 with SIZE AND COUNT ALREADY EXACT.  The
 *     whole 561-instruction reconstruction cost one compile.  That is the
 *     "LOOK UP A SHAPE IN THE SOLVED CORPUS BEFORE INVENTING A CONSTRUCT" rule
 *     paying its largest dividend so far in this family.
 *
 * (2) `fn2` AS A SECOND NAMED DrawFn LOCAL: 230 -> 226.  The spark draw loop
 *     loads iwram_3001f0c EARLY (right after BuildDraw2DFuncEx) and spills it
 *     to sp+0x24, then reloads it ten instructions later for the call.  An
 *     early load plus a spill is the signature of a SOURCE-LEVEL LOCAL, not of
 *     a value materialised at the call.  Proof by removal: inlining
 *     `iwram_3001f0c(...)` at the call site (candidate g4) breaks SIZE (1312 vs
 *     1308) and COUNT (589 vs 588) and drops aligncmp to 71.1%.  The local is
 *     load-bearing.
 *
 * MEASURED INERT (evidence only against this base, not disproved in general):
 *   - swapping the `fn2` / `fn` declaration positions (g5): 226, aligncmp
 *     identical at 435/74.0%.  The declaration ORDER of those two is not the
 *     handle.
 *   - `n2 = n;` as a second named lane-offset local: gcc copy-propagates it
 *     away.  It changes nothing and does NOT create the ROM's sp+0x14 slot.
 *
 * MEASURED WORSE:
 *   - `DrawFn fns[2]` with fns[0] = gPtrs[0x2e] and fns[1] = iwram_3001f0c
 *     (g3), the d9ab8_StatDown / 80ecef4 array idiom that puts both call
 *     targets in memory at 0x20/0x24: 348 of 588, aligncmp 436 (74.1%) in 95
 *     hunks.  It does move FOUR of the six `_call_via` veneers onto the ROM's
 *     r5 -- the ROM has r4,r4,r5,r5,r5,r5 and g3 has r4,r7,r5,r5,r5,r7 against
 *     g2's r4,r4,r7,r7,r9 -- but the array does NOT reach a declared frame slot
 *     (frame stays 0x44) and the rest of the stream regresses.  Recorded
 *     because the veneer improvement is real and may combine with a fix for
 *     the blocker below.
 *   - inlining `iwram_3001f0c(...)` (g4): SIZE and COUNT both break.
 *
 * ================================================================
 * THE BLOCKER, BY PASS: local_alloc/global_alloc -- ONE EXTRA REGISTER ALLOCNO,
 * AND IT COSTS A WHOLE FRAME SLOT
 * ================================================================
 *
 * ALL 226 differing encodings trace to a single allocation fact, and the first
 * one objcmp reports says so outright:
 *
 *     index 12:  ref b092  sub sp, #72  (0x48)
 *                ours b091  sub sp, #68  (0x44)
 *
 * The ROM's frame is ONE WORD LARGER than ours, so EVERY sp-relative load and
 * store in 561 instructions carries a wrong offset.  That is why objcmp reads
 * 226 while aligncmp -- which cannot forgive a changed offset either, but does
 * not let one shift poison later indices -- reads only 169 in 90 hunks, and why
 * the two numbers must be read together here.
 *
 * THE FRAME MAP IS FULLY DECODED, both sides, from the hunks:
 *
 *   ROM   0x3c v(12 bytes)  0x38 cx  0x34 cy  0x30 ctx  0x2c i  0x28 frame
 *         0x24 fn2  0x20 fn  0x1c cam  0x18 dst  0x14 n2  0x10 look  0x0c n
 *         0x08 q      + 0x00/0x04 outgoing args      = 0x48
 *   OURS  0x38 v(12 bytes)  0x34 cx  0x30 cy  0x2c ctx  0x28 i  0x24 frame
 *         0x20 fn  0x1c cam  0x18 dst  0x14 look  0x10 n  0x0c q
 *         0x08 <reload spill of k>  + 0x00/0x04 outgoing   = 0x44
 *
 * Read that against the recorded rule -- "gcc assigns spill slots in ascending
 * pseudo number to descending sp offset", so declared locals descend from the
 * top in declaration order and reload's own slots come LAST, at the bottom.
 * Both maps obey it exactly.  cam sits at 0x1c in BOTH, which is the anchor
 * that makes the rest unambiguous.
 *
 * WHAT DIFFERS IS ONE ALLOCNO, NOT THIRTEEN OFFSETS.  In the ROM `fn2` is
 * memory-resident at 0x24 and the spark loop's counter `k` lives in r11; in
 * ours `fn2` WON a register (r9) and `k` was spilled to 0x08 instead.  The
 * register rotation confirms the direction and the doc's reading of it -- "high
 * = one EXTRA allocno": ours puts `base` in fp/r11 where the ROM puts it in r9,
 * a uniform one-position-HIGH rotation across the whole function
 * (`mov fp,r0 / add r5,fp` against `mov r9,r0 / add r5,r9`, at nine sites).
 * We are carrying one register quantity the ROM spends on memory.
 *
 * WHY IT IS NOT REACHABLE FROM THE SOURCE HERE.  The ROM's r11 holds `k`, a
 * three-reference tight-loop counter with a high allocno priority; `fn2` has
 * two references in one straight-line block.  greg should prefer `k` -- and in
 * the ROM it does.  Ours inverts the pick, which means the priorities are close
 * and the tie broke the other way.  Both handles the doc records for this were
 * measured and neither moves it:
 *   - RAISING fn2's memory residency through an aggregate type (the
 *     `unsigned char value[4]` / expand_decl corollary, tested as `DrawFn
 *     fns[2]`) does not reach a declared slot at all -- the frame stays 0x44 --
 *     so the "give it an array type of the right width" corollary does not
 *     extend to a two-element pointer array here.
 *   - MAKING THE VALUE BLOCK-LOCAL to leave the greg race is unavailable:
 *     fn2's definition and its use are ALREADY in one basic block.
 * And the recorded caution applies directly -- "the slot order is a CONSEQUENCE
 * of the allocation, not a handle on it": reordering the declarations to the
 * ROM's map (g2 does exactly that, and g5 permutes it again) changed the
 * offsets not at all.
 *
 * SO THE CLASS IS THE RECORDED ONE: "REGISTER ALLOCATION: it is systematic, and
 * it is not reachable from C."  What is NEW and worth the row is the
 * SYMPTOM-TO-CAUSE CHAIN, which is cheap to test on any long candidate:
 *
 *   > A FRAME ONE WORD SHORT PLUS A UNIFORM ONE-POSITION-HIGH REGISTER ROTATION
 *   > IS ONE EXTRA REGISTER ALLOCNO, NOT A MISSING DECLARATION.  Decode both
 *   > frame maps against the ascending-pseudo/descending-sp rule, find the
 *   > variable the ROM keeps in memory and you keep in a register, and the
 *   > whole residue collapses to that one tie.
 *
 * RULED OUT, with what was measured:
 *   - NOT a missing statement or a mis-read program: SIZE and COUNT are exact
 *     on the FIRST candidate and stayed exact through every revision; the
 *     relocation symbol sequence and its offsets are identical.
 *   - NOT the outgoing-argument area: the widest call takes six arguments
 *     (sp+0x00 and sp+0x04 only), so 8 bytes is right and the extra word is a
 *     local.
 *   - NOT the vec3_t: `Func_80e3944` writes through `add r5, sp, #0x3c` and
 *     reads [r5] and [r5,#4]; 0x3c + 12 = 0x48 lands exactly on the frame end,
 *     so the aggregate is 12 bytes and fully accounted for.
 *   - NOT a second lane-offset variable: `n2 = n;` is copy-propagated away and
 *     produces no slot, so the ROM's sp+0x14 is a reload spill of a pseudo that
 *     exists only because fn2 took the register k should have had.
 *
 * NEXT MOVE, if this is re-attempted: do not chase the frame.  Find one more
 * way to cost `fn2` its register -- lengthening its live range past a second
 * call, or giving `k` a fourth reference to raise its priority -- and the 226
 * should collapse in one step, because everything else already agrees.
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
    int x, y, z, vx, vy, vz, t;
} Part;

typedef struct {
    int a0, a4, a8, kind, flags, a14, a18;
} Spark;

extern int *iwram_3001eec[];
extern DrawFn iwram_3001f0c;
extern void *gPtrs[];
extern unsigned char Leec70[] __asm__(".Leec70");
extern unsigned char Leec74[] __asm__(".Leec74");
extern unsigned char Leec7d[] __asm__(".Leec7d");
extern unsigned short Leec86[] __asm__(".Leec86");
extern unsigned char Leec98[] __asm__(".Leec98");
extern unsigned char Leeca1[] __asm__(".Leeca1");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int **_GetBattleActor(int id);
extern unsigned int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void Func_80e46f0(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void Func_80e3944(Part *in, vec3_t *out);
extern void _PlaySound(int sfx);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Ground(void *context)
{
    vec3_t v;
    int cx;
    int cy;
    void *ctx;
    int i;
    int frame;
    DrawFn fn2;
    DrawFn fn;
    void *cam;
    int *dst;
    int n2;
    void *look;
    int n;
    u8 *q;
    char **tbl;
    char **pp;
    u8 *base;
    State **slot;
    Part *p;
    Part *lp;
    Spark *s;
    int *src;
    int k;
    int t;
    int arg;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    cam = *(void **)((char *)tbl - 0x6c);
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    Anim_Djinni(context, 0, (*slot)->f4, 2, &cx, &cy);
    REG_BLDALPHA = 0x1010;
    if ((*slot)->f4 == 1) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    }
    fn = (DrawFn)gPtrs[0x2e];
    LoadVFXFile(FILE_a7, base, 1, 0);
    LoadVFXFile(FILE_94, base + 0x65c0, 1, 1);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    src = *_GetBattleActor((*(State **)(base + 0x7828))->f8);
    dst = *_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    i = 0;
    t = 0;
    p = (Part *)(base + (0xe1 << 7));
    do {
        p->x = src[2];
        p->y = 0x84 << 15;
        p->z = src[4];
        p->vx = t >> 5;
        p->vy = (((int)(Random() & 0x7f) - 0x40) << 16) >> 6;
        p->vz = (((int)(Random() & 0xff) - 0x7f) << 16) >> 5;
        if (p->x > 0) {
            p->vx = -p->vx;
        }
        p->t = 1;
        i++;
        t += 0xa0 << 15;
        p++;
    } while (i != 8);
    look = (char *)cam + 0xc;
    frame = 0;
    do {
        if (frame > 0x10) {
            Func_80e46f0(FILE_a7);
        }
        if ((*(State **)(base + 0x7828))->f1c == 1) {
            int ang = frame << 11;
            int X = ((-sin(ang) * 4) >> 16) + cx / 2 - 0xa;
            int Y = ((cos(ang) * 2) >> 16) + cy - 0x16;
            if (frame > 0x10) {
                Y = Y - frame * 2 + 0x20;
            }
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                BuildDraw2DFuncEx(0x2f, 7, 7, 7, 3);
            } else {
                BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
            }
            if (frame <= 3) {
                iwram_3001f0c(ctx, base + 0x65c0, X, Y, 0x14, 0x28);
            }
            gfree(0x2f);
            fn(ctx, base + 0x65c0, X, Y, 0x14, 0x28);
        }
        if ((frame & 1) == 0) {
            i = 0;
            s = (Spark *)(base + (0xe8 << 7));
            do {
                s->kind = Random() % 6 + 3;
                s->flags = Leec70[Random() & 3];
                i++;
                s++;
            } while (i != 0x20);
        }
        InitMatrixStack();
        MatrixSetLook(cam, look);
        i = 0;
        n = 0;
        q = base;
        lp = (Part *)(base + (0xe1 << 7));
        do {
            if (lp->t == 1) {
                n2 = n;
                if (frame > n) {
                    int X;
                    int Y;
                    Func_80e3944(lp, &v);
                    v.x = v.x >> 1;
                    X = v.x - 0xc;
                    Y = v.y - 0x18;
                    fn(ctx, base, X, Y, 0x18, 0x30);
                    if ((frame & 3) <= 1) {
                        fn(ctx, base + Leec86[1], X + Leec98[1], Y + Leeca1[1],
                           Leec74[1], Leec7d[1]);
                    } else {
                        fn(ctx, base + Leec86[2], X + Leec98[2], Y + Leeca1[2],
                           Leec74[2], Leec7d[2]);
                    }
                    k = 0;
                    s = (Spark *)(q + (0xe8 << 7));
                    do {
                        int px;
                        int py;
                        BuildDraw2DFuncEx(0x2f, 7, 7, s->flags, 2);
                        fn2 = iwram_3001f0c;
                        if (s->flags & 4) {
                            px = X - Leec74[s->kind] - Leec98[s->kind] + 0x18;
                        } else {
                            px = X + Leec98[s->kind];
                        }
                        if (s->flags & 8) {
                            py = Y - Leec7d[s->kind] - Leeca1[s->kind] + 0x30;
                        } else {
                            py = Y + Leeca1[s->kind];
                        }
                        fn2(ctx, base + Leec86[s->kind], px, py,
                            Leec74[s->kind], Leec7d[s->kind]);
                        gfree(0x2f);
                        k++;
                        s++;
                    } while (k != 4);
                    lp->x += lp->vx;
                    lp->y += lp->vy;
                    lp->z += lp->vz;
                }
                if (frame > n + 0x10) {
                    int nvx;
                    int nvy;
                    int nvz;
                    nvx = lp->vx + ((dst[2] - lp->x) >> 8);
                    nvy = lp->vy + (((0xa0 << 13) - lp->y) >> 8);
                    lp->vx = nvx;
                    lp->vy = nvy;
                    nvz = lp->vz + ((dst[4] - lp->z) >> 8);
                    lp->vz = nvz;
                    if (frame < n2 + 0x55) {
                        lp->vx = nvx * 60 / 64;
                        lp->vy = nvy * 60 / 64;
                        lp->vz = nvz * 60 / 64;
                    }
                    if (lp->y <= 0x13ffff) {
                        *(int *)(base + 0x77a8) = 8;
                        lp->t = 0;
                        _PlaySound(0x86);
                        Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 5, 0, 4);
                        _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 4);
                    }
                }
            }
            n += 2;
            q += 0x70;
            i++;
            lp++;
        } while (i != 6);
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x60);
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
