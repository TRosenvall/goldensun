/* BaseAnim_Breath -- 0x080dbc30, asm/rom_c9000/rom_dbbdc_c_c_c_c_c_c_c.s,
 * 623 ROM instructions.
 * NON-MATCHING, 647 of 660 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 647 is NOT a distance -- but it is CLOSE:
 * 1476 bytes against the ROM's 1468 (+8) and 669 encodings against 660 (+9).
 * tools/aligncmp.py reads 407 aligned-equal of 660 (61.7%), 315
 * differing/ins/del in 127 hunks, and that 61.7% is the figure to beat.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/dbc30_Breath.c \
 *     asm/rom_c9000/rom_dbbdc_c_c_c_c_c_c_c.s --func BaseAnim_Breath
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/dbc30_Breath.c
 *
 * THE SPLIT SHAPE.  tools/datacheck.py: the .s holds FIVE functions
 * (BaseAnim_Breath, Anim_Unused_ElementOrbs at 0x080dc1ec, Anim_Unused_Haunt at
 * 0x080dc454, Anim_Unused_SkullCloud at 0x080dc6bc, Anim_Atalanta at 0x080dc968)
 * AND a `.rodata` section, so converting BaseAnim_Breath needs a TEXT/DATA SPLIT
 * with the data keeping its own object.
 *   *** BaseAnim_Breath READS NO DATA LABEL, so the split needs NO NEW `.global`.
 * (Only Anim_Atalanta in this file reads one, `.Leeb40`; leave it alone.)
 * BaseAnim_Breath is the FIRST function in the file, so the cut is two ways, not
 * three -- tools/split_s.py leaves it in <stem>_a.s with the other four plus the
 * data behind it.
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE, and that is a RESULT, not an accident -- see below.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * (0) THE WHOLE TEMPLATE TRANSFERRED ON THE FIRST CANDIDATE, 664 of 660 at size
 *     1496 (+28) / 669 (+9).  Every idiom came from the landed siblings
 *     src/rom_c9000/rom_cd508_c_b.c (Anim_Confuse) and
 *     src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c (Anim_Fireball) unchanged:
 *     `g = &iwram_3001ef0; ctx = g[0];` with
 *     `base = *(unsigned char **)((char *)g - 4)` (the negative-offset lever --
 *     iwram_3001ef0 and the word four bytes below it are ONE symbol, so ONE pool
 *     word plus a `sub r3,#4`, exactly as 80cd104.c records);
 *     `DrawFn fns[2]` with [0] written through the array, the g[7] value loaded
 *     into `f1` BEFORE `fp = fns`, then `fp[1] = f1` (reload inheritance);
 *     `BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, (void **)fp)` with the
 *     SAME expression serving both `fp = fns` and argument 2, which is what makes
 *     the ROM's single `mov r1,sp / add r1,#0x28 / str r1,[sp,#0x14]` come out;
 *     `arg = 0x90; arg <<= 3;` for StartTask.
 *     DECLARATION ORDER IS THE FRAME MAP and it was right first time: the four
 *     aggregates descend pos(0x9c), posarr[8](0x3c), t(0x30), fns(0x28), then the
 *     spilled scalars descend in declaration order ctx(0x24), frame(0x20),
 *     slotC(0x1c), pp(0x18), fp(0x14), e(0x10).
 *
 * (1) THE `variant` PARAMETER MUST REACH r11, AND IT DOES SO ON ITS OWN ONCE
 *     `base` IS NOT PINNED.  The first candidate pinned `base` to r9 (the ROM's
 *     register) and `variant` was spilled to sp+0x28 instead of the ROM's
 *     `mov fp, r1`.  Pinning `variant` to r11 as well only moved it to 35.9%.
 *     DROPPING BOTH PINS took it to 61.7% -- `variant` lands in r11 and `base`
 *     lands in r10 (a high-register ROTATION against the ROM's r9, which is the
 *     batch-301 "one EXTRA allocno" tell, and it is the residue below).
 *
 * (2) REG_BLDALPHA / REG_BLDCNT / REG_BG2PA ALL WANT AN `int` LOCAL.  Same lever
 *     as BaseAnim_Blob in this batch: written directly to a `volatile u16 *`, gcc
 *     pools the constant as a HALFWORD (`ldrh`); the ROM has three WORD pools at
 *     their own local labels (`.Ldbc6c @ 0x1010`, `.Ldbd5c @ 0`, `.Ldbe84 @ 0x55`).
 *     An `int` carrier for each gives `ldr`.
 *
 * (3) `two = 2` IS SCOPED TO THE variant==7 ARM.  The ROM's `mov r5,#2` serves
 *     BOTH BuildDraw2DFuncEx calls in that arm and NOT the two
 *     `*(int *)(base + (0xef << 7)) = 2` stores, which get their own `mov r3,#2`.
 *     MEASURED INERT on the current base (identical figures), so it is evidence
 *     only against this candidate -- keep it, it is the ROM's shape.
 *
 * (4) THE FILE SELECTION IS A REAL `switch`, NOT AN if-CHAIN -- and this is the
 *     OPPOSITE of BaseAnim_Blob, where the if-chain beat the switch by 4 points.
 *     The tell is in the reference: `cmp r1,#4 / bhi .Ldbd8c / ldr r2,=.Ldbd40 /
 *     lsl r3,r1,#2 / ldr r3,[r3,r2] / mov pc,r3` with a FIVE-entry `.word` table.
 *     A jump table is proof of a switch; Blob's seven `cmp`/`beq` pairs are proof
 *     of a chain.  The fifth entry equals the default target, so the source is
 *     `case 4: default:` -- four distinct cases plus a shared fifth is what makes
 *     gcc size the table at 5.  READ THE DISPATCH OFF THE ASM; do not guess.
 *
 * (5) THE TWO `*(int *)(base + (0xef << 7)) = 2` STORES GO INSIDE BOTH ARMS.  The
 *     ROM duplicates the store in both and cross-jumps only the trailing
 *     `add r2,r9 / str r3,[r2]` (.Ldbdc4).  Hoisting the store above the `if`
 *     destroys that.  This is the BaseAnim_Tentacle family warning working in the
 *     documented direction: do NOT tidy two arms to look alike.
 *
 * ================================================================
 * THE PIN THAT MEASURED WORSE -- TWICE
 * ================================================================
 *
 * `register unsigned char *base __asm__("r9")` is the cf2a0_Revive "high-register
 * pin on base" lever and here it is CATASTROPHIC:
 *     no pins (this)      : size 1476 (+8),  count 669 (+9),  aligned 61.7%
 *     base pinned to r9   : size 1500 (+32), count 681 (+21), aligned 35.9%
 * Measured twice, on two different bases (with and without the `variant` pin),
 * same 35.9% both times.  Pinning the register the ROM actually uses forces r9
 * live across the whole function and reload pays for it everywhere.  With the
 * batch-295 and batch-301 instances this is now the SIXTH and SEVENTH time a pin
 * has measured worse, and BaseAnim_Blob in this same batch is the counter-example
 * where the equivalent pin helps one view and hurts the other.  THE PIN IS A
 * HYPOTHESIS PER FUNCTION, NEVER A FAMILY DEFAULT.
 *
 * Also measured INERT (so untested, not disproved): an unpinned `int variant`
 * copy of the parameter; three separate `int` carriers for the three register
 * writes instead of one reused `bld`; per-site r0/r2 pins on the single
 * Func_8001af8 call (Breath has only ONE such site, so the Blob lever has nothing
 * to defeat here -- that is WHY Breath can be pin-free and Blob cannot).
 *
 * ================================================================
 * THE BLOCKER, BY PASS
 * ================================================================
 *
 * (A) ONE EXTRA SPILLED SCALAR -- local-alloc/reload, and it is the whole +8/+9.
 *     Our frame is 0xac against the ROM's 0xa8 and EVERY slot below the
 *     aggregates is shifted down by four (ctx 0x28 vs 0x24, fns 0x2c vs 0x28,
 *     fp 0x18 vs 0x14).  The ROM has exactly eight scalar slots (0x08 and 0x0c
 *     are reload temps, 0x10..0x24 are e/fp/pp/slotC/frame/ctx); we have nine.
 *     RULED OUT as the source of it: `two` and `bld` (removing both is byte-inert,
 *     measured), and the pinned-parameter shadow (v7 drops the pin entirely and
 *     the frame is still 0xac).  So the ninth slot is a reload temp we are
 *     generating and the ROM is not, which pairs with (B).
 *
 * (B) THE HIGH-REGISTER ROTATION r9 <-> r10 ON `base`.  Per batch 301 a
 *     high-register rotation is ONE EXTRA allocno, not an ordering problem, and
 *     (A) says where the extra one is spending itself.  The candidate to hunt is
 *     a per-region quantity the ROM names and we do not: the ROM re-derives
 *     `base + 0x7828` at least SIX times with its own pool word each time, but
 *     inside the knockback loop of the variant==5 arm it keeps it in r5 AND keeps
 *     `base + 0x77a8` in r10 across the Func_80d6888 call, while the other arm
 *     uses the register+register form `ldr r3,[r2,r1]` with 0x7828 cse'd into r1.
 *     This candidate names `slotD` and `hitp` in the first arm only.  The
 *     ROM's asymmetry between the two arms is the next thing to reproduce.
 *
 * (C) Constant-pool placement, the same arm_reorg consequence BaseAnim_Blob's park
 *     documents at length: 1468 bytes needs the pool split (Thumb `ldr rd,[pc]`
 *     reaches 1020), the ROM dumps at 0x3c/0x48 and 0x14c, we defer, and every
 *     `ldr rX,[pc,#N]` on both sides carries a different N.  Most of the 315
 *     aligncmp differences are this.  It is downstream of (A) and (B).  Do not
 *     chase it.
 *
 * ONE SPELLING NOT YET TRIED: the particle blit reads the HIGH HALFWORD of the
 * 16.16 x and y (`ldrsh r2,[r5,#2]`, `ldrsh r3,[r5,#6]`).  This candidate spells
 * that `((short *)&p->x)[1]`, which works but puts the load in the struct's alias
 * set; a `short xl, xh` pair, or the one-member-union escape from batch 295, are
 * both untried and either could move the store/load ordering in the update tail.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern unsigned char *iwram_3001ef0;

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void BuildDraw2DFuncs(int mode, void **out);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int  Random(void);
extern void GetBattleActorPos(int unit, int *dest);
extern void GetBattleActorPos2(int unit, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int n);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void BaseAnim_Breath(void *context, int variant)
{
    vec3_t pos;
    vec3_t posarr[8];
    vec3_t t;
    DrawFn fns[2];
    void *ctx;
    int frame;
    State **slotC;
    vec3_t *pp;
    DrawFn *fp;
    Part *e;
    unsigned char **g;
    unsigned char *base;
    unsigned char *buf2;
    DrawFn f1;
    CopyFn copy;
    Part *p;
    int *hitp;
    int i;
    int k;
    int u;
    int tt;
    int fid;
    int two;
    int arg;
    int w;
    int h;
    int lim;

    g = &iwram_3001ef0;
    ctx = g[0];
    base = *(unsigned char **)((char *)g - 4);
    *(State **)(base + 0x7828) = (State *)context;
    buf2 = g[1];
    AnimStart(0);
    { int b0 = 0x1010; REG_BLDALPHA = b0; }
    if (variant == 7) {
        two = 2;
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
        fns[0] = (DrawFn)g[6];
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, two);
        f1 = (DrawFn)g[7];
        fp = fns;
        fp[1] = f1;
    } else {
        fp = fns;
        BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, (void **)fp);
    }
    LoadVFXFile(FILE_ce, base, 1, 0);
    if (variant == 5) {
        LoadVFXFile(FILE_5a, base + 0xc56, 1, 1);
    } else if (variant == 7) {
        LoadVFXFile(FILE_54, base + 0xc56, 1, 1);
    } else {
        LoadVFXFile(FILE_7d, base + 0xc56, 1, 1);
        LoadVFXFile(FILE_73, buf2, 0, 0);
        if (variant == 6) {
            volatile u16 *pal;
            int d0;
            d0 = 0xa0;
            i = 0;
            d0 <<= 19;
            pal = (volatile u16 *)d0;
            do {
                int c = i / 4;
                i++;
                *pal = (c << 10) | (c << 5) | c;
                pal++;
            } while (i != 0x40);
            { int b1 = 0; REG_BLDCNT = b1; }
        } else {
            switch (variant) {
            case 0:
                fid = FILE_7d;
                break;
            case 1:
                fid = FILE_b9;
                break;
            case 2:
                fid = FILE_6e;
                break;
            case 3:
                fid = FILE_a1;
                break;
            case 4:
            default:
                fid = FILE_8d;
                break;
            }
            {
                void *s;
                int d0;
                s = GetFile(fid);
                d0 = 0xa0;
                copy = Func_8001af8;
                d0 <<= 19;
                copy((volatile u16 *)d0, s, 0x80);
            }
        }
    }
    if (variant == 7) {
        *(int *)(base + (0xef << 7)) = 2;
        *(int *)(base + 0x7784) = 0x32;
    } else {
        *(int *)(base + (0xef << 7)) = 2;
        *(int *)(base + 0x7784) = 0x4b;
    }
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    p = (Part *)(base + 0x7080);
    i = 0;
    do {
        i++;
        p->t = -1;
        p++;
    } while (i != 0x40);
    pp = &pos;
    GetBattleActorPos((*(State **)(base + 0x7828))->f8, (int *)pp);
    if (variant == 3)
        pos.y -= 0x10;
    if (variant == 4) {
        if ((*(State **)(base + 0x7828))->f4 == 1)
            pos.x += 0x1c;
        else
            pos.x -= 0x1c;
    }
    if (variant == 7) {
        if ((*(State **)(base + 0x7828))->f4 == 1)
            pos.x += 0x10;
        else
            pos.x -= 0x10;
    }
    if (variant == 5) {
        pos.x = pos.x / 3;
        { int b2 = 0x55; REG_BG2PA = b2; }
    }
    i = 0;
    if ((*(State **)(base + 0x7828))->f14 != 0) {
        vec3_t *ap = posarr;
        do {
            GetBattleActorPos2((*(State **)(base + 0x7828))->ids[i], ap);
            i++;
            ap++;
        } while (i != (*(State **)(base + 0x7828))->f14);
    }
    slotC = (State **)(base + 0x7828);
    e = (Part *)(base + (0xe1 << 7));
    frame = 0;
    do {
        k = frame % (*slotC)->f14;
        if (frame == 4)
            _PlaySound(0x88);
        if (variant == 6) {
            if (frame == 0x3c)
                _Func_80bd7dc(0x86);
        } else {
            if (frame == 0x18)
                _Func_80bd7dc(0x86);
        }
        if (variant == 5) {
            if ((*slotC)->f4 == 1) {
                u = frame / 3 % 3;
                fns[0](ctx, base + (u * 9 << 9) + 0xc56, pos.x - 2, pos.y - 0x20,
                       0x48, 0x3e);
            } else {
                u = frame / 3 % 3;
                fns[0](ctx, base + (u * 9 << 9) + 0xc56, pos.x - 0x46, pos.y - 0x20,
                       0x48, 0x3e);
            }
        } else {
            t.x = posarr[k].x + (Random() & 0x1f) - 0x10;
            t.y = posarr[k].y + (Random() & 0x3f) - 0x10;
            if (frame <= 0x2f) {
                e->x = pp->x << 15;
                e->y = pp->y << 16;
                e->vx = (t.x - pp->x) << 11;
                e->vy = (t.y - pp->y) << 11;
                e->t = 0;
            }
        }
        p = (Part *)(base + (0xe1 << 7));
        i = 0;
        w = 0x20;
        h = 0x40;
        do {
            tt = p->t;
            if (tt >= 0) {
                if (variant == 7) {
                    if (tt > 5)
                        fp[(*slotC)->f4](ctx, base + 0xc56,
                                         ((short *)&p->x)[1] - 0x10,
                                         ((short *)&p->y)[1] - 0x20, w, h);
                } else if (variant == 4) {
                    if (tt > 5)
                        fns[0](ctx, base + (tt / 4 << 11) + 0xc56,
                               ((short *)&p->x)[1] - 0x10,
                               ((short *)&p->y)[1] - 0x20, w, h);
                } else if (variant != 5) {
                    if (tt > 1)
                        fns[0](ctx, base + (tt / 4 << 11) + 0xc56,
                               ((short *)&p->x)[1] - 0x10,
                               ((short *)&p->y)[1] - 0x20, w, h);
                }
                p->x += p->vx;
                p->y += p->vy;
                p->t = p->t + 1;
                if (p->t == 0x18)
                    p->t = -1;
            }
            i++;
            p++;
        } while (i != 0x40);
        if (variant == 5) {
            i = 0;
            if ((*slotC)->f14 != 0) {
                State **slotD;
                k = frame & 7;
                hitp = (int *)(base + 0x77a8);
                slotD = (State **)(base + 0x7828);
                lim = 2;
                do {
                    if (frame >= lim && k == i) {
                        *hitp = 8;
                        Func_80d6888((*slotD)->ids[i], 7, 5, i, 4);
                    }
                    i++;
                    lim += 4;
                } while (i != (*slotD)->f14);
            }
        } else {
            i = 0;
            if ((*(State **)(base + 0x7828))->f14 != 0) {
                k = frame & 7;
                do {
                    if (frame >= i * 4 + 0x10 && k == i) {
                        *(int *)(base + 0x77a8) = 8;
                        if (variant == 6)
                            Func_80d6888((*(State **)(base + 0x7828))->ids[i],
                                         0xe, 5, i, 4);
                        else
                            Func_80d6888((*slotC)->ids[i], 7, 5, i, 4);
                        _SetBattleActorKnockback((*slotC)->ids[i], 4);
                    }
                    i++;
                } while (i != (*(State **)(base + 0x7828))->f14);
            }
        }
        UpdateScreenShake(4, 4);
        if (variant != 6)
            Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        e++;
        frame++;
    } while (frame != 0x40);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
