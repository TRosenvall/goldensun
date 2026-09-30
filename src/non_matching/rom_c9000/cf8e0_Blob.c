/* BaseAnim_Blob -- 0x080cf8e0, asm/rom_c9000/rom_cf88c_c_c_c_c_c_c_c.s, 670 ROM instructions.
 * NON-MATCHING, 666 of 703 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 666 is NOT a distance: 1564 bytes against
 * the ROM's 1556 (+8) and 709 encodings against 703 (+6).  tools/aligncmp.py
 * reads 344 aligned-equal of 703 (48.9%), 457 differing/ins/del in 150 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/cf8e0_Blob.c \
 *     asm/rom_c9000/rom_cf88c_c_c_c_c_c_c_c.s --func BaseAnim_Blob
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/cf8e0_Blob.c
 *
 * THE SPLIT SHAPE.  tools/datacheck.py: the .s holds ONE function
 * (BaseAnim_Blob) AND a `.rodata` section, so converting it needs a TEXT/DATA
 * SPLIT with the data keeping its own object.  BaseAnim_Blob reads four data
 * labels and THE SPLIT MUST ADD FOUR `.global`s:
 *     .global .Lee0b6   .global .Lee0c4   .global .Lee0d6   .global .Lee0e8
 * They come into C as `extern signed char Lee0b6[] __asm__(".Lee0b6");` and
 * friends -- the idiom already landed in src/rom_c9000/rom_dd2ac_c_c_b.c.
 *
 * SHIMS: tools/shimcount.py reports 6 register pins.  NOT pin-free.  All six are
 * the THREE Func_8001af8 call sites x (r0, r2), the landed
 * src/rom_c9000/rom_cc5d8_a_a_b.c idiom, and they are load-bearing here for the
 * reason cf2a0_Revive.c records: without them cse2 unifies `0xa0 << 19` across
 * all three sites into ONE callee-saved register and THAT is what evicts a
 * whole-function quantity.  Measured: 694 -> 604 differing, 48.5% -> 48.5%
 * aligned but size 1596 -> 1588 and count 723 -> 717.  There is no fakematch.txt
 * row yet; add one when this lands.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * (1) SEPARATE THE FILE-ID LOCAL FROM THE LOOP COUNTER.  The first candidate
 *     reused one `int i` for the 7-way file-id selection AND the two -1 fill
 *     loops AND the second particle loop.  That makes one long-lived allocno with
 *     a huge n_refs which beats `base` in allocno_compare, and `base` -- 20
 *     references, r11 for the whole ROM function -- gets SPILLED to sp+0x18 and
 *     reloaded 15 times.  A dedicated `fid` took size 1596 -> 1588 and count
 *     723 -> 717.  This is the declaration-order rule showing up as a register
 *     EVICTION rather than a permutation.
 *
 * (2) THE THREE Func_8001af8 SITES NEED PER-SITE r0/r2 PINS (above), 694 -> 604
 *     differing.
 *
 * (3) REG_BLDALPHA = 0x1010 MUST GO THROUGH AN `int` LOCAL.  Written directly,
 *     gcc pools 0x1010 as a HALFWORD and emits `ldrh r3,.L82`; the ROM has
 *     `ldr r3,.Lcf954 @ 0x1010` -- a WORD pool.  `bld = 0x1010; REG_BLDALPHA =
 *     bld;` gives the word form.  Worth size 1588 -> 1568 and count 717 -> 711,
 *     aligned 48.5% -> 49.8%.  TRANSFERS: Breath needs it three times
 *     (0x1010 into REG_BLDALPHA, 0 into REG_BLDCNT, 0x55 into REG_BG2PA -- all
 *     three are word pools with their own local labels in the ROM).
 *
 * (4) ONE `State **slot` LOCAL PER REGION, not one for the function and not none.
 *     The ROM derives `base + 0x7828` SEVEN times, each with its own pool word,
 *     but WITHIN the actor-fetch region and again within the gBuffer init loop it
 *     keeps it in r5 across calls and re-loads only the value.  Two locals
 *     (`slotA` for the actor fetch + the f14 guard, `slotB` for the init loop and
 *     its latch) with `*(State **)(base + 0x7828)` written out everywhere else:
 *     size 1580 -> 1564, count 716 -> 709.  This RECONCILES BaseAnim_Tentacle's
 *     "do not cache the battle-state slot" with cf2a0_Revive's "name a
 *     `State **slot`": both are true, and the unit is the REGION.
 *
 * ================================================================
 * THE PIN THAT DID NOT PAY, AND WHY THE RANKING IS AMBIGUOUS
 * ================================================================
 *
 * `register unsigned char *base __asm__("r11")` is the cf2a0_Revive "high-register
 * pin on base" lever and it SPLITS the two ranking views:
 *     with the pin   : size 1580 (+24), count 716 (+13), aligned 52.5%, objcmp 678
 *     without (this) : size 1564 ( +8), count 709 ( +6), aligned 48.9%, objcmp 666
 * Neither is size-and-count exact so neither figure is a distance.  THIS PARK
 * KEEPS THE PIN-FREE FORM, on two grounds: it is +8/+6 against +24/+13 on the two
 * figures that gate a true distance, and on BaseAnim_Breath in the same batch the
 * equivalent pin was CATASTROPHIC (61.7% -> 35.9% aligned, and that one was
 * measured twice).  The pin is recorded, not discarded -- whoever reopens this
 * should re-rank once size and count are exact, because at that point objcmp's
 * count becomes meaningful and can settle it.
 *
 * ================================================================
 * THE BLOCKER, BY PASS
 * ================================================================
 *
 * TWO residues, and the larger one is a CONSEQUENCE, not a cause.
 *
 * (A) CONSTANT-POOL PLACEMENT -- gcc's `arm_reorg` / `dump_table`, not an
 *     optimisation pass.  The ROM dumps its first pool at offset 0x74, 9 words,
 *     behind a `b .Lcf970` that gcc itself inserted; ours defers the whole pool
 *     to 0x354.  Because a Thumb `ldr rd,[pc,#imm]` reaches only 1020 bytes and
 *     this function is 1556, the pool MUST be split, and every `ldr rX,[pc,#N]`
 *     on both sides of the split carries a different N.  That is most of the
 *     457 aligncmp differences and NONE of it is a source-level question:
 *     aligncmp counts a moved pool offset as a difference by design.  It resolves
 *     itself when the instruction stream is right.  Do not chase it directly.
 *
 * (B) TWO 0x40000 CONSTANTS IN THE gBuffer INIT LOOP THAT REFUSE TO STAY
 *     SEPARATE -- local-alloc / reload, not cse.  The ROM computes 0x80 << 11
 *     ONCE before the outer loop (r4, a call-USED register under -fcall-used-r4,
 *     so it is spilled to sp+8 across _GetBattleActor and reloaded) for `e->y`,
 *     and AGAIN fresh inside the inner body (`movs r2,#0x80 / lsls r2,#11`) for
 *     `e->vy`.  Ours unifies them into one callee-saved high register and the
 *     spill/reload pair never appears.  RULED OUT: this is not a cse question --
 *     `amp` as a local plus `0x80 << 11` inline is what this candidate already
 *     writes, and the two still unify, so the two expressions reach local-alloc
 *     as one pseudo.  What differs is PRESSURE: the ROM's inner counter is in
 *     r10 and increments through a low temp (`movs r2,#1 / add sl,r2`), ours is
 *     in r4 (`adds r4,#1`), and a high-register counter is the batch-301 tell for
 *     one EXTRA live quantity at that point.  So the handle is a quantity we are
 *     still missing in that region, not the spelling of the constant.  The
 *     per-region `slotB` (lever 4) was the first half of it and paid; there is at
 *     least one more.
 *
 * SMALLER, ALL MEASURED AND ALL STILL OPEN:
 *   - `_FILE_bf` IS NOT IN include/file_table.h (the table jumps ba -> bb -> bd,
 *     skipping bc and bf).  The ROM's pool word is a bare `0x000000bf`; ours is
 *     0 plus R_ARM_ABS32 to a locally declared `extern int _FILE_bf`.  That is a
 *     relocation FORM difference, NOT a residue -- objcmp compares unlinked
 *     objects -- but a literal `0xbf` is WRONG, because gcc emits `mov r7,#0xbf`
 *     for it and the ROM has `ldr r7,=0xbf`, a pool LOAD.  Only a symbol pools.
 *     LANDING THIS REQUIRES ADDING `_FILE_bf` TO include/file_table.h; the local
 *     `extern int _FILE_bf;` in this file is a placeholder for the measurement.
 *   - THE if-CHAIN BEATS THE switch for the 7-way file selection, 48.5% against
 *     44.5% aligned, even though the switch gets the POOL ORDER right (the ROM
 *     emits _FILE_8d before _FILE_77 because variant 3 and the default share one
 *     block; the if-chain emits 77 first).  Both cross-jump, both produce the
 *     same 5 pool words.  Recorded as a REAL trade, not a mistake.
 *   - `j * 32 + k` indexing ewram_2011c00: the ROM hoists `j << 2` to a stack
 *     slot and shifts it by 3 again in the body, so LICM saw `j*4` as its own
 *     insn.  The batch-298 "distribute the shift by hand" lever is the thing to
 *     try here and this candidate has NOT tried it.
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

extern int _FILE_bf;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern Part gBuffer[];
extern Part ewram_2010018[];
extern Part ewram_2011c00[];
extern Part ewram_2011c18[];
extern signed char   Lee0b6[] __asm__(".Lee0b6");
extern unsigned char Lee0c4[] __asm__(".Lee0c4");
extern unsigned char Lee0d6[] __asm__(".Lee0d6");
extern unsigned short Lee0e8[] __asm__(".Lee0e8");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  DecompressLZ(void *src, void *dst);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int *_GetBattleActor(int id);
extern int  Random(void);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(Part *g, int a, int b);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void BaseAnim_Blob(void *context, int variant)
{
    vec3_t v;
    DrawFn fns[2];
    void *ctx;
    int frame;
    int j;
    int nframes;
    void *cam;
    DrawFn *fp;
    Part *p;
    int m;
    void **g;
    void **pp;
    unsigned char *base;
    unsigned char *file;
    CopyFn copy;
    DrawFn f1;
    State **slotA;
    State **slotB;
    int *actor;
    int *a2;
    Part *q;
    Part *e;
    Part *z;
    int i;
    int k;
    int count;
    int u;
    int amp;
    int id;
    int fid;
    int bld;
    int two;
    int arg;
    int mask;
    int r;
    unsigned char w;
    unsigned char h;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    cam = *(void **)((char *)g - 0x6c);
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    bld = 0x1010;
    REG_BLDALPHA = bld;
    id = (int)&_FILE_bf;
    file = GetFile(id);
    {
        register u32 p0 __asm__("r0");
        register s32 p2 __asm__("r2");
        unsigned char *s;
        p0 = 0xa0;
        p2 = 0x80;
        s = file;
        copy = Func_8001af8;
        file += 0x80;
        p0 <<= 19;
        copy((volatile u16 *)p0, s, p2);
    }
    DecompressLZ(file, base);
    file = GetFile(FILE_9e);
    {
        register u32 p0 __asm__("r0");
        register s32 p2 __asm__("r2");
        unsigned char *s;
        p0 = 0xa0;
        s = file;
        p2 = 0x80;
        p0 <<= 19;
        copy((volatile u16 *)p0, s, p2);
    }
    file += 0x80;
    DecompressLZ(file, base + (0xfa << 6));
    if (variant == 0)
        fid = FILE_9f;
    else if (variant == 1)
        fid = FILE_59;
    else if (variant == 2)
        fid = FILE_a0;
    else if (variant == 3)
        fid = FILE_77;
    else if (variant == 4)
        fid = id;
    else if (variant == 6)
        fid = FILE_8d;
    else
        fid = FILE_77;
    file = GetFile(fid);
    {
        register u32 p0 __asm__("r0");
        register s32 p2 __asm__("r2");
        CopyFn copy2;
        unsigned char *s;
        p0 = 0xa0;
        copy2 = Func_8001af8;
        s = file;
        p2 = 0x80;
        p0 <<= 19;
        copy2((volatile u16 *)p0, s, p2);
    }
    i = 0;
    z = ewram_2010018;
    count = 0x80 << 3;
    do {
        i++;
        z->x = -1;
        z++;
    } while (i != count);
    slotA = (State **)(base + 0x7828);
    actor = (int *)*_GetBattleActor((*slotA)->f8);
    j = 0;
    amp = 0x80 << 11;
    if ((*slotA)->f14 != 0) {
        int off = 0;
        slotB = (State **)(base + 0x7828);
        do {
            a2 = (int *)*_GetBattleActor((*slotB)->ids[j]);
            e = (Part *)((char *)gBuffer + off);
            k = 0;
            do {
                e->y = amp;
                e->x = actor[2];
                e->z = actor[4];
                e->vx = (a2[2] - actor[2]) >> 4;
                e->vy = 0x80 << 11;
                e->vz = (a2[4] - actor[4]) >> 4;
                k++;
                e->t = 0;
                e++;
            } while (k != 0x10);
            j++;
            off += 0xe0 << 1;
        } while (j != (*slotB)->f14);
    }
    i = 0;
    z = ewram_2011c18;
    count = 0x80 << 1;
    do {
        i++;
        z->x = -1;
        z++;
    } while (i != count);
    two = 2;
    if (((State *)context)->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
        fns[0] = (DrawFn)gPtrs[0x2e];
        if (Lee0b6[variant * 2] == 0) {
            BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
            f1 = (DrawFn)gPtrs[0x2f];
            fp = fns;
            fp[1] = f1;
        } else {
            BuildDraw2DFuncEx(0x2f, 7, 7, 7, two);
            f1 = (DrawFn)gPtrs[0x2f];
            fp = fns;
            fp[1] = f1;
        }
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 7, two);
        fns[0] = (DrawFn)gPtrs[0x2e];
        if (Lee0b6[variant * 2] == 0) {
            BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
            f1 = (DrawFn)gPtrs[0x2f];
            fp = fns;
            fp[1] = f1;
        } else {
            BuildDraw2DFuncEx(0x2f, 7, 7, 3, two);
            f1 = (DrawFn)gPtrs[0x2f];
            fp = fns;
            fp[1] = f1;
        }
    }
    *(int *)(base + (0xef << 7)) = two;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    if (Lee0b6[variant * 2] == 0)
        nframes = ((*(State **)(base + 0x7828))->f14 << 3) + 0x48;
    else
        nframes = ((*(State **)(base + 0x7828))->f14 << 3) + 0x38;
    _PlaySound(0x67);
    frame = 0;
    while (frame != nframes) {
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        j = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            p = gBuffer;
            m = 0;
            do {
                if (frame >= m) {
                    if (frame == m + 0x11) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[j], 7, 5, j, 0x10);
                        _Func_80bd7dc(0x85);
                    }
                    if (p->t >= 0) {
                        u = (frame - m) / 3;
                        if (u > 9)
                            u = 9;
                        Func_80e3944((vec3_t *)p, &v);
                        v.x = v.x >> 1;
                        if (u > 4)
                            fns[0](ctx, base + (u * 3 << 8), v.x - 0x10, v.y - 0xc,
                                   0x20, 0x18);
                        else
                            fns[0](ctx, base + (u * 3 << 8), v.x - 0xc, v.y - 0x10,
                                   0x18, 0x20);
                        if (p->t == 0)
                            Func_80e38b8(p, 0x3f, 0xffff8000);
                        if (p->y < 0) {
                            p->y = 0;
                            p->t = 1;
                            if (Lee0b6[variant * 2] == 0)
                                count = 4;
                            else
                                count = 0x10;
                            k = 0;
                            if (count != 0) {
                                mask = 0x3f;
                                do {
                                    q = &ewram_2011c00[j * 32 + k];
                                    q->x = p->x;
                                    q->y = p->y;
                                    q->z = p->z;
                                    if (Lee0b6[variant * 2] == 0) {
                                        q->vx = ((Random() & mask) - 0x20) << 11;
                                        q->vy = 0;
                                        q->vz = ((Random() & mask) - 0x20) << 11;
                                    } else {
                                        q->vx = ((Random() & mask) - 0x20) << 13;
                                        q->vy = ((Random() & 0x1f) + 0x20) << 12;
                                        q->vz = ((Random() & mask) - 0x20) << 13;
                                    }
                                    k++;
                                    q->t = 0;
                                } while (k != count);
                            }
                        }
                    }
                }
                p += 0x10;
                m += 8;
                j++;
            } while (j != (*(State **)(base + 0x7828))->f14);
        }
        q = ewram_2011c00;
        i = 0;
        do {
            if ((unsigned)q->t <= 0x2c && q->y >= 0) {
                Func_80e3944((vec3_t *)q, &v);
                v.x = v.x >> 1;
                if (Lee0b6[variant * 2] == 0) {
                    fp[1](ctx, base + (q->t / 8 * 9 << 7) + (0xfa << 6),
                          v.x - 0xc, v.y - 0x18, 0x18, 0x30);
                } else {
                    u = q->t / 5;
                    if (i & 1)
                        u += 9;
                    r = ((State *)context)->f4;
                    if (q->vx > 0)
                        r ^= 1;
                    w = Lee0c4[u];
                    h = Lee0d6[u];
                    fp[r](ctx, base + Lee0e8[u] + (0xf0 << 5),
                          v.x - (w >> 1), v.y - (h >> 1), w, h);
                }
                if (Lee0b6[variant * 2] == 0)
                    Func_80e38b8(q, 0x3e, 0x80 << 4);
                else
                    Func_80e38b8(q, 0x3e, 0xffff8000);
                q->t = q->t + 1;
            }
            i++;
            q++;
        } while (i != (0x80 << 1));
        UpdateScreenShake(2, 2);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
