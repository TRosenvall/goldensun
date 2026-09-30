/* Anim_AstralBlast -- 0x080d0ee0, 490 instructions.  PARKED, CHARACTERISED.
 *
 * NON-MATCHING, 475 of 509 encodings differ.  That is objcmp's figure and IT IS
 * SATURATED, NOT A DISTANCE: the instruction counts differ (ref 509, ours 511)
 * and objcmp also prints SIZE (ref 1136, ours 1140) and RELOCATIONS, so the
 * 475 is the by-index compare of two streams that are two instructions out of
 * step.  The honest measurement is tools/aligncmp.py's kind: a normalised
 * SequenceMatcher over the two disassemblies gives SPAN 297 IN 80 HUNKS, and
 * the frame is FOUR BYTES TOO BIG, which shifts every one of the eighteen
 * spill-slot offsets and accounts for most of those hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/cfef4_AstralBlast.c \
 *     asm/rom_c9000/rom_cfef4.s --func Anim_AstralBlast
 *
 * SPLIT SHAPE: TEXT-ONLY, ONE NEW EXPORT.  asm/rom_c9000/rom_cfef4.s holds SIX
 * functions (Anim_Condemn, Anim_Unused_ScreenMelt, Anim_Bind, Anim_PsyphonSeal,
 * Anim_AstralBlast, Anim_ShiningStar) plus a .rodata tail, so a TEXT/DATA SPLIT
 * is needed.  tools/datacheck.py: Anim_AstralBlast reads .Lee140 and nothing
 * else, so the remainder needs exactly
 *
 *     .global .Lee140
 *
 * immediately before its label.  .Lee140 is 0xee140..0xee158, TWENTY-FOUR bytes
 * = TWO vec3_t: the ROM indexes it as `&Lee140[i & 1]` and the `lsl #1 / add /
 * lsl #2` chain in the reference is gcc's own multiply-by-12.  NO SHIMS, NO
 * PINS (tools/shimcount.py is clean).
 *
 * ================================================================
 * THE BLOCKER: A DEAD-STORE PAIR THE ROM KEEPS AND NO SPELLING PRESERVES
 * ================================================================
 *
 * Inside the ten-iteration matrix loop the ROM writes the SAME two slots twice:
 *
 *     ldr  r3, [r0, #0]      @ r0 = r9 = sp+0x5c, a vec3_t NEVER WRITTEN
 *     adds r2, r2, r3
 *     str  r2, [r5, #12]     @ store 1
 *     ldr  r3, [r0, #4]
 *     ldr  r2, [r6, #4]
 *     adds r2, r2, r3
 *     str  r2, [r5, #16]     @ store 2
 *     ldr  r3, [r6, #0]
 *     str  r3, [r5, #12]     @ store 3, overwrites store 1
 *     ldr  r3, [r6, #4]
 *     str  r3, [r5, #16]     @ store 4, overwrites store 2
 *
 * sp+0x5c is read at +0 and +4 and is assigned nowhere in the whole function --
 * `grep 'sp, #0x5c'` finds only `mov r1,#0x5c / add r1,sp / mov r9,r1`, which is
 * also the ONLY frame address in this function that is NOT built with a single
 * `add rd, sp, #imm` (sv at 0x50 and out at 0x68 both are).  So the ROM reads an
 * uninitialised 12-byte local through a pointer held in r9, and its stores to
 * gBuffer survive because those reads may alias them.
 *
 * gcc deletes stores 1 and 2 in EVERY spelling measured:
 *   - `qv.x` / `qv.y` on a plain uninitialised `vec3_t qv`        511, span 297
 *   - `vec3_t *qp = &qv;` with `qp->x`, qp set in the n-loop body 511, span 297
 *   - the same with qp set before the frame loop                  511
 *   - a ONE-MEMBER UNION read, `union { vec3_t v; } qv; qv.v.x`   511  (INERT)
 *   - `*(int *)((char *)&qv + 0)`                                 511  (INERT)
 * The one-member-union escape does not help because the aliasing decision here
 * is DECL-based, not type-based: gcc can see the pointer's base is a local and
 * gBuffer is a global, and no amount of retyping hides that.  What would hide it
 * is a base gcc cannot trace, which no plausible source shape provides.
 *
 * ================================================================
 * AND THE FRAME: ONE SLOT TOO MANY, BECAUSE r9 HOLDS THE WRONG THING
 * ================================================================
 *
 * `sub sp, #0x78` against the ROM's `sub sp, #0x74`.  The ROM's frame is
 * eighteen scalar words at 0x08..0x4f plus THREE vec3_t at 0x50 (sv), 0x5c (the
 * uninitialised one) and 0x68 (out) -- and ONE OF THOSE EIGHTEEN, sp+0x40, IS
 * NEVER REFERENCED (`grep 'sp, #0x' ` lists 8,c,10,14,18,1c,20,24,28,2c,30,34,
 * 38,3c,44,48,4c and nothing at 0x40).  That is alter_reg's signature for a
 * pseudo that got a stack slot and then had every reference rematerialised --
 * the same REG_EQUIV artefact recorded in src/non_matching/rom_c9000/80db6e0.c,
 * and here it is identifiable: the pseudo is the sp+0x5c POINTER, which the ROM
 * ends up keeping in r9 while its slot stays empty.  We do the opposite -- we
 * put the ten-loop counter in r9 and spill the pointer to a real, referenced
 * slot (sp+0x24 in our layout), which is one word MORE than the ROM allocates.
 * The ROM spills the counter (sp+0xc, reloaded on almost every line of that
 * loop) and keeps the pointer.  Our choice is cheaper and gcc will not be
 * talked out of it.
 *
 * ================================================================
 * THE REST OF THE SPAN, AND WHAT IS ALREADY RIGHT
 * ================================================================
 *
 * Right on the first candidate, and not moved since: the whole prologue
 * (`base = *pp0++`, `ctx = *pp0`, `base2 = g[2]`, `cam = *(void **)((char *)g -
 * 0x6c)` -- the bank's negative-offset lever); `self` and `target` as
 * `(int *)*_GetBattleActor(...)` with `->ids[0]` reached by the register-offset
 * `ldrsh` Thumb has no immediate form for; the three-iteration setup loop
 * including its if/else on `p->vx` whose `bl __divsi3 / str` tail the ROM
 * cross-jumps; the `p->y` and `p->z` RE-READS for vy and vz; the
 * `view->f36 -= d` / `+= d` pair with a cross-jumped `strh`; `d = 0x80; if
 * (frame > 0x27) d = 0x300 - frame * 16;`; the interpolation double loop's
 * `c = &gBuffer[a2 + j]; j++; d = &gBuffer[a2 + j % 0xa];` order; and the hit
 * block down to `Func_80d6888(ids[0], 7, 5, 0, 8)`.
 *
 * Three named residues beyond the two above, each worth naming because each is
 * a known lever pointing the wrong way:
 *
 *  (i) `MatrixRoll(e << 10)` is LICM-HOISTED by us into a spill slot and
 *      recomputed per iteration by the ROM, which keeps only `e` in a slot.
 *      That is one more live quantity on our side and plausibly the reason the
 *      r9 tie above goes against us -- relieving it is the first thing to try.
 *  (ii) Our `c` pointer in the interpolation loop is strength-reduced to a
 *      walking iv rebased by -16 (`adds r5,r3,#0 / subs r5,#16`, then
 *      `ldr r2,[r5,#0]` for what the ROM reads as `ldr r2,[r7,#12]`).  The ROM
 *      RECOMPUTES `gBuffer + (a2 + j) * 28` from the pool every iteration.  This
 *      is the recorded negative-offset iv lever firing where it is not wanted.
 *  (iii) We carry FIVE induction variables out of the n-loop tail against the
 *      ROM's four (a += 0xa, pp += 0x1c, b += 0xc, n += 1); the extra one is
 *      decrementing (`subs r1,#12`) and is a second iv manufactured from the
 *      `gBuffer[n * 0xa + i]` subscripts.  Writing that walker by hand was
 *      measured and is WORSE (517 instructions, +6), so the fix is not a
 *      pointer local -- it is fewer distinct subscript forms.
 *
 * Do not start from a fresh trace: the control flow, the constants and the two
 * cross-jumped arms are all confirmed correct against the reference, and the
 * only things standing between this file and a match are the r9/slot tie, the
 * dead-store pair it enables, and the three iv/LICM items above.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *iwram_3001e80;
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern vec3_t Lee140[] __asm__(".Lee140");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  DecompressLZ(void *src, void *dst);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int *_GetBattleActor(int id);
extern void _Actor_SetAnim(void *actor, int anim);
extern void _Actor_SetAnimSpeed(void *actor, int speed);
extern void _Actor_Stop(void *actor);
extern void _Actor_TravelTo(void *actor, int x, int y, int z);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern void MatrixScalev(vec3_t *v);
extern void MatrixPush(void);
extern void MatrixPop(void);
extern void MatrixRoll(int a);
extern void MatrixYaw(int a);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_AstralBlast(void *context)
{
    vec3_t out;
    vec3_t qv;
    vec3_t sv;
    unsigned char *base;
    void *ctx;
    int frame;
    DrawFn blit;
    int n;
    void *base2;
    void *cam;
    void **g;
    void **pp0;
    int *self;
    int *target;
    CopyFn copy;
    Part *p;
    int two;
    int arg;

    g = iwram_3001eec;
    pp0 = g;
    base = (unsigned char *)*pp0++;
    ctx = *pp0;
    base2 = g[2];
    cam = *(void **)((char *)g - 0x6c);
    self = (int *)*_GetBattleActor(((State *)context)->f8);
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    {
        unsigned char *d;
        int d0;
        d = GetFile(FILE_79);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, d, 0x80);
    }
    DecompressLZ(GetFile(FILE_73), base2);
    _Actor_SetAnim(self, 2);
    _Actor_SetAnimSpeed(self, 0x30);
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    *(int *)(base + (0xef << 7)) = two;
    *(int *)(base + 0x7784) = 0x4b;
    blit = (DrawFn)g[7];
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    self = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->f8);
    target = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    n = 0;
    p = (Part *)(base + (0xe1 << 7));
    do {
        p->x = self[2];
        p->y = self[3] + (0xa0 << 14);
        p->z = self[4];
        if (n == 0) {
            p->vx = (target[2] - self[2]) / 0xc;
        } else {
            p->vx = (target[2] * 2 - self[2]) / 0xc;
        }
        p->vy = (target[3] - p->y + (0xa0 << 14)) / 0xc;
        p->vz = (target[4] - p->z) / 0xc;
        p->t = 0;
        n++;
        p++;
    } while (n != 3);
    frame = 0;
    do {
        if (frame <= 0x2f) {
            void *view;
            int d;
            view = iwram_3001e80;
            d = 0x80;
            if (frame > 0x27) {
                d = 0x300 - frame * 16;
            }
            if ((*(State **)(base + 0x7828))->f4 == 0) {
                *(u16 *)((char *)view + 0x36) = *(u16 *)((char *)view + 0x36) - d;
            } else {
                *(u16 *)((char *)view + 0x36) = *(u16 *)((char *)view + 0x36) + d;
            }
        }
        n = 0;
        do {
            if (frame >= n * 0xc) {
                Part *q = (Part *)(base + (0xe1 << 7)) + n;
                int m;
                int e;
                int sc;
                int a2;
                int s2;
                int s;
                int i;
                e = frame - n * 0xc;
                s = e / 4 + 2;
                if (s > 0xa) {
                    s = 0xa;
                }
                InitMatrixStack();
                MatrixSetLook(cam, (char *)cam + 0xc);
                MatrixTranslatev((vec3_t *)q);
                m = 0;
                sc = (e << 12) + (0x80 << 5);
                i = 0;
                do {
                    int r;
                    MatrixPush();
                    MatrixRoll(e << 10);
                    MatrixYaw(0x80 << 7);
                    sv.x = sc;
                    if (sc > (0x80 << 9)) {
                        sv.x = 0x80 << 9;
                    }
                    sv.y = sv.x;
                    sv.z = sv.x;
                    MatrixScalev(&sv);
                    MatrixRoll(i * 0x199a);
                    r = Func_80e3944(&Lee140[i & 1], &out);
                    if (m < r) {
                        m = r;
                    }
                    out.x >>= 1;
                    gBuffer[n * 0xa + i].vx = out.x + qv.x;
                    gBuffer[n * 0xa + i].vy = out.y + qv.y;
                    gBuffer[n * 0xa + i].vx = out.x;
                    gBuffer[n * 0xa + i].vy = out.y;
                    MatrixPop();
                    i++;
                } while (i != 0xa);
                if (m <= 0x61a7f) {
                    int j;
                    a2 = n * 0xa;
                    s2 = s / 2;
                    j = 0;
                    do {
                        Part *c = &gBuffer[a2 + j];
                        Part *d;
                        int k;
                        j++;
                        d = &gBuffer[a2 + j % 0xa];
                        k = 0;
                        do {
                            int x = c->vx + (d->vx - c->vx) * k / 16;
                            int y = c->vy + (d->vy - c->vy) * k / 16;
                            blit(ctx, (char *)base2 + Data_ede48[s - 1], x - s2,
                                 y - s, s, s * 2);
                            k++;
                        } while (k != 0x10);
                    } while (j != 0xa);
                }
                q->x += q->vx;
                q->y += q->vy;
                q->z += q->vz;
                if (frame == n * 0xc + n + 0xa) {
                    target[0xd] = 0x80 << 10;
                    target[0xc] = 0x80 << 12;
                    target[0xa] = 0xa0 << 11;
                    target[0x12] = 0xab85;
                    *((char *)target + 0x5a) = 0;
                    _Actor_Stop(target);
                    if (target[2] < 0) {
                        _Actor_TravelTo(target, target[2] + 0xffd80000, 0, target[4]);
                    } else {
                        _Actor_TravelTo(target, target[2] + (0xa0 << 14), 0, target[4]);
                    }
                    if (n == 2) {
                        _Func_80bd7dc(0x86);
                    } else {
                        _PlaySound(0x86);
                        Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 5, 0, 8);
                    }
                    *(int *)(base + 0x77a8) = 4;
                }
            }
            n++;
        } while (n != 3);
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x3c);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
