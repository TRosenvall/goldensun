/* Anim_CriticalHit -- asm/rom_c9000/rom_e3958_c_c_c_c_a_b.s
 *
 * MATCHING.  1612 bytes, 707 encodings and 84 relocations identical, under
 * `--func` and under `--whole`.  PIN-FREE (tools/shimcount.py reports no rows),
 * DEVICE-FREE, no flag group, no fakematch row.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/rom_c9000/rom_e3958_c_c_c_c_a_b.c \
 *     asm/rom_c9000/rom_e3958_c_c_c_c_a_b.s --whole
 *
 * Landed from a park that had stood at 2 of 707 for seven passes (batches 327,
 * 336, 341 and the 347 harvest).  THE EDIT IS ONE WORD:
 *
 *     typedef void (*ClearFn)(void *dst, int len);     <- was this
 *     typedef int  (*ClearFn)(void *dst, int len);     <- is this
 *
 * plus the matching `extern int Func_80008d4(void *dst, int len);` for source
 * hygiene.  MEASURED SEPARATELY: the typedef ALONE is byte-identical to this
 * file, and changing ONLY the callee's declaration (leaving the typedef `void`)
 * is bit-identical to the old 2-of-707 body.  **THE LEVER IS THE FUNCTION
 * POINTER'S RETURN TYPE, NOT THE CALLEE'S DECLARATION.**
 *
 * ===========================================================================
 * WHERE IT CAME FROM: A LANDED FILE IN THIS BANK NAMED THE LEVER OUTRIGHT
 * ===========================================================================
 * src/rom_c9000/rom_cc5d8_a_b.c -- landed, same bank, calling the SAME callee
 * through the same `_call_via_rN` veneer -- says in its own header:
 *
 *     "This one wants the pointer typed `int (*)(void *, s32)` ... which is the
 *      opposite of its neighbour src/rom_c9000/rom_cd508_a_b.c, calling the SAME
 *      callee with a `void` pointer type.  The two together are the clearest
 *      statement of the rule: the pointer's return type is a per-CALL-SITE
 *      choice that reports what the original translation unit declared, not a
 *      property of the callee."
 *
 * That is the whole answer, and it had been sitting two files away for four
 * batches.  Found by grepping the LANDED half of this bank for `Func_80008d4`
 * -- three landed files use it, and one of them is a documented instance of
 * this exact axis.  The park had never varied the pointer's return type: its
 * recorded rows vary the anchor/barrier placement, the `cl`/`clen`/`ctx`
 * carrier spellings, the flag groups and `p` as an induction variable.
 *   >> ASK WHAT KIND OF THING EVERY RECORDED ROW VARIES, THEN GREP THE LANDED
 *   >> CORPUS FOR THE CALLEE.  A `void` vs non-`void` function POINTER is a
 *   >> free variable that costs nothing to try and that no park here had on its
 *   >> list. <<
 *
 * ===========================================================================
 * WHY IT WORKS, AND IT IS THE SAME FACT BATCH 350 FOUND ON Func_80b2ed8
 * ===========================================================================
 * The residue was the pair at indices 364/365 -- `ldr r5,[pc,#0x90]` against
 * `ldr r0,[sp,#0x34]` -- in basic block 25, the `cl(ctx, clen)` call.  Every
 * `rank_for_schedule` rung above the dependent-count rung ties, and
 * `haifa-sched.c:4097-4110` then prefers MORE DEPENDENTS:
 *   insn 920 = the `*.LC12` pool load for `cl` (RTX_UNCHANGING_P, alias set 4)
 *              -- FOUR dependents
 *   insn 923 = `ctx` reloaded from sp+52 (alias set 0) -- FIVE, because a MEM
 *              read joins `pending_read_insns` unconditionally
 *              (`haifa-sched.c:3553-3556`) and every call flushes that list
 *              (`:3955` -> `:3217-3223`, REG_DEP_ANTI).
 * The park's bound therefore read: reduce the number of CALLS between
 * `cl(ctx, clen)` and the end of the block.
 *
 * A VOID CALL CARRIES A TRAILING `(use (const_int 0))` INSN AND A NON-VOID CALL
 * DOES NOT.  Typing `ClearFn` as returning `int` makes both `cl(...)` sites
 * value-returning, which drops those USE insns and re-shapes block 25's
 * dependence graph -- so the park's bound was satisfied without moving a single
 * statement.  (The park's named escape, getting GetBattleActorPos2 out of the
 * block, is still blocked; it simply was not the only way.)
 *
 * That void/non-void USE asymmetry was established independently in the same
 * batch on `Func_80b2ed8`, where it is what makes `find_cross_jump` refuse a
 * six-insn tail: `jump.c:1448` skips NOTEs and CODE_LABELs but NOT a USE, so
 * `jump.c:1468` breaks on CALL_INSN-against-INSN.  One compiler fact, two
 * functions, opposite signs -- see src/non_matching/rom_b0000/80b2ed8.c.
 *
 * ===========================================================================
 * THE REST OF THE BODY, inherited and unchanged
 * ===========================================================================
 * The two `do { } while (0);` anchors and `p` written as `base + K + i` inside
 * the loop are batch 341's work and all three are load-bearing; the park file
 * being retired carries their derivation.  Anchors are ordinary C and owe
 * nothing to reports/pass3-depin.md.  The shipped body is PIN-FREE and
 * DEVICE-FREE: it was already so at 2, and this edit adds nothing.
 *
 * SPLIT: asm/rom_c9000/rom_e3958_c_c_c_c_a.s held THREE functions --
 * Anim_Attack (parked, 5 of 39), BaseAnim_Attack (parked, 653 of 688) and this
 * one -- so it ships on ONE cut.  tools/datacheck.py prints nothing for that
 * .s, so it is a TEXT-ONLY split needing no new `.global`:
 *     rom_e3958_c_c_c_c_a_a.s   2 functions, 757 lines  (the two parks)
 *     rom_e3958_c_c_c_c_a_b.s   Anim_CriticalHit alone, 723 lines
 * Verified with `--whole` against the cut-out reference before shipping.
 *
 * -- scratch_elev/b350/B
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*ClearFn)(void *dst, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern short iwram_3001ad0[];
extern char *iwram_3001e74;
extern Part gBuffer[];
extern unsigned char ewram_2013840[];
extern unsigned short Data_ede5c[];

extern void _Func_80c0df4(int a, int b, int c);
extern void WaitFrames(unsigned int n);
extern void InitRenderTilemapBG1(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80cd4b4(void);
extern int *_GetBattleActor(int id);
extern int Random(void);
extern void _Actor_SetAnimSpeed(int *actor, int speed);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void _PlaySound(int id);
extern void InitMatrixStack(void);
extern void MatrixRoll(int a);
extern void MatrixPitch(int a);
extern void MatrixYaw(int a);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(Part *p, int a, int b);
extern int Func_80008d4(void *dst, int len);
extern void _Func_80bd7dc(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern int _Func_80b8530(int id);
extern void _Func_80c0700(unsigned short a, int b);
extern void Func_80cdd14(void);
extern void gfree(int tag);

void Anim_CriticalHit(void *context)
{
    vec3_t va;
    vec3_t vb;
    vec3_t vc;
    vec3_t ve;
    DrawFn d[2];
    void *ctx;
    int shake;
    void *base2;
    void *cam;
    int s24;
    int s28;
    int s2c;
    int s34;
    int s48;
    DrawFn *fp;
    DrawFn f1;
    void **g;
    void **pp;
    unsigned char *base;
    State **slot;
    int *actor;
    int *actorB;
    int crit;
    int frame;
    int i;
    int h;
    Part *p;
    Part *q;
    CopyFn cp;
    int clen = 0x80 << 7;
    ClearFn cl;
    int msk;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    base2 = g[2];
    cam = *(void **)((char *)g - 0x6c);
    crit = 1;
    if (((State *)context)->f0 <= 0xc7) {
        crit = 0;
    }
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    _Func_80c0df4(((State *)context)->f8, ((State *)context)->fc, 0x82);
    WaitFrames(1);
    InitRenderTilemapBG1();
    REG_BG1CNT = 0x1f80;
    if ((*slot)->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, 3);
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    }
    d[0] = (DrawFn)gPtrs[0x2e];
    do { } while (0);
    f1 = (DrawFn)gPtrs[0x2f];
    fp = d;
    fp[1] = f1;
    slot = (State **)(base + 0x7828);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_49, base, 1, 0);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_4a, gBuffer, 1, 1);
    if ((*slot)->f8 > 7) {
        void *f = GetFile(FILE_8e);
        cp = Func_8001af8;
        cp((volatile u16 *)(0xa0 << 19), f, 0x80);
    }
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    LoadVFXFile(FILE_76, base2, 0, 0);
    _Func_80c0df4((*slot)->f8, (*slot)->fc, 0x82);
    WaitFrames(1);
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    StartTask(Task_BlitAnim, 0x90 << 3);
    if (crit == 1) {
        actor = (int *)*_GetBattleActor((*slot)->f8);
        i = 0;
        do {
            p = (Part *)(base + (0xe1 << 7)) + i;
            p->x = (Random() & 0x3f) + 0x10;
            p->y = 0;
            p->z = 0;
            p->vx = Random() & 0xffff;
            p->vy = Random() & 0xffff;
            p->vz = Random() & 0xffff;
            i++;
        } while (i != 0x40);
        _Actor_SetAnimSpeed(actor, 0);
        s24 = actor[9];
        s28 = actor[10];
        s2c = actor[11];
        s48 = actor[18];
        s34 = actor[13];
        actor[9] = 0;
        actor[10] = 0;
        actor[11] = 0;
        actor[13] = 0;
        actor[18] = 0;
        GetBattleActorPos2((*(State **)(base + 0x7828))->f8, &va);
        shake = 0x40 - va.x;
        iwram_3001ad0[2] = shake;
        iwram_3001ad0[3] = 0x50;
        *(int *)(base + 0x77b4) = 0x18;
        *(int *)(base + 0x77b8) = 0;
        StartTask(Func_80cd4b4, 0xc8 << 4);
        _PlaySound(0xd4);
    do { } while (0);
        frame = 0;
        do {
            _Func_80c0df4((*(State **)(base + 0x7828))->f8,
                          (*(State **)(base + 0x7828))->fc, 0x82);
            i = 0;
            q = (Part *)(base + (0xe1 << 7));
            do {
                if (q->x >= 0 && frame >= i / 4) {
                    int w = (i & 1) + 5;
                    InitMatrixStack();
                    MatrixRoll(q->vz);
                    MatrixPitch(q->vx);
                    MatrixYaw(q->vy);
                    Func_80e3944((vec3_t *)q, &vc);
                    vc.x = vc.x + 0x40;
                    vc.y = vc.y + va.y + 0x18;
                    if (vc.z < -0x3c) {
                        vc.z = -0x3c;
                    }
                    if (vc.z > 0x3c) {
                        vc.z = 0x3c;
                    }
                    vc.z = vc.z + 0x3c;
                    d[0](ctx, (char *)base2 + Data_ede5c[w - 1], vc.x - w,
                         vc.y - w, w * 2, w * 2);
                    q->x = q->x - 4;
                }
                i++;
                q++;
            } while (i != 0x40);
            *(int *)(base + 0x7824) = 1;
            WaitFrames(1);
            frame++;
        } while (frame != 0x20);
        StopTask(Func_80cd4b4);
        _Actor_SetAnimSpeed(actor, 0x10);
        actor[9] = s24;
        actor[10] = s28;
        actor[11] = s2c;
        actor[13] = s34;
        actor[18] = s48;
    }
    cl = Func_80008d4;
    cl(ctx, clen);
    cl((void *)0x6004000, clen);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    REG_BG1CNT = 0x1f81;
    GetBattleActorPos2((*(State **)(base + 0x7828))->ids[0], &vb);
    if ((*(State **)(base + 0x7828))->f4 == 0) {
        shake = 0x20 - vb.x;
    } else {
        shake = 0x60 - vb.x;
    }
    if (shake > 0) {
        shake = 0;
    }
    if (shake < -0x80) {
        shake = -0x80;
    }
    vb.x = vb.x + shake;
    iwram_3001ad0[3] = 0x50;
    iwram_3001ad0[2] = shake;
    actorB = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    h = _Func_80b8530((*(State **)(base + 0x7828))->ids[0]) / 2;
    i = 0;
    msk = 0xff;
    p = (Part *)(base + (0xe1 << 7));
    do {
        p->x = actorB[2];
        p->y = actorB[3] + h;
        p->z = actorB[4];
        p->vx = (Random() & msk) << 10;
        p->vy = (Random() & msk) << 10;
        p->vz = ((Random() & msk) - 0x7f) << 10;
        if (p->x > 0) {
            p->vx = -p->vx;
        }
        p->t = i + 0x10;
        i++;
        p++;
    } while (i != 0x40);
    frame = 0;
    do {
        if (frame == 5) {
            _Func_80bd7dc(0x86);
        }
        if (frame == 4) {
            _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 0);
        }
        GetBattleActorPos2((*(State **)(base + 0x7828))->f8, &va);
        va.y = va.y + 0x10;
        if (frame <= 1) {
            d[0](ctx, base, 0, 0, 0x78, 0x78);
        } else if (frame <= 3) {
            d[0](ctx, base + (0xe1 << 6), 0, 0, 0x78, 0x78);
        } else if (frame <= 5) {
            d[0](ctx, gBuffer, 0, 0, 0x78, 0x78);
        } else if (frame <= 7) {
            d[0](ctx, ewram_2013840, 0, 0, 0x78, 0x78);
        }
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        if ((unsigned)(frame - 4) <= 0x1b) {
            i = 0;
            do {
                int m = i / 2;
                Part *r = (Part *)(base + m * 28 + (0xe1 << 7));
                int w = r->t;
                if (w > 0) {
                    Func_80e3944((vec3_t *)r, &ve);
                    ve.x = ve.x + shake;
                    w = w >> 3;
                    w = w + 2;
                    ve.y = ve.y + 0x10;
                    fp[m & 1](ctx, (char *)base2 + Data_ede5c[w - 1],
                              ve.x - w, ve.y - w, w * 2, w * 2);
                    Func_80e38b8(r, 0x3c, -0x400);
                    r->t = r->t - 1;
                }
                i++;
            } while (i != 0x40);
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x20);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    iwram_3001ad0[3] = 0x20;
    {
        unsigned short *t;
        frame = 0;
        t = (unsigned short *)(iwram_3001e74 + (0xc9 << 3));
        do {
            _Func_80c0700(*t, 6 - frame);
            WaitFrames(1);
            frame++;
        } while (frame != 7);
    }
    Func_80cdd14();
}
