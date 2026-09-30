/* Anim_Volcano -- 0x080d4ce8, 591 instructions.  PARKED.
 *
 * NON-MATCHING, 560 of 624 encodings differ.  THAT FIGURE IS SATURATED AND
 * RANKS NOTHING: SIZE is NOT exact (ref 1392 bytes, ours 1428) and the COUNT is
 * NOT exact (ref 624, ours 641 -- SEVENTEEN *OVER*, not short).  tools/aligncmp.py
 * reads
 *
 *     aligned-equal 408 (65.4% of ref), 292 differing/ins/del in 116 hunks
 *
 * RELOCATIONS: the SYMBOL SEQUENCE IS ALREADY IDENTICAL -- all 60 relocations,
 * same symbols in the same order, ON THE FIRST CANDIDATE.  Only the offsets
 * differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Volcano.c \
 *     asm/rom_c9000/rom_d45ec_c_c.s --func Anim_Volcano
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT NEEDED, THREE NEW EXPORTS.  tools/datacheck.py on
 * asm/rom_c9000/rom_d45ec_c_c.s reports a .rodata section and TWO functions,
 * BaseAnim_Nova and Anim_Volcano (second).  Anim_Volcano reads three data
 * labels, so the split
 *     *** MUST EXPORT: .global .Lee29a
 *     ***              .global .Lee29d
 *     ***              .global .Lee2a9
 * and nothing else.  BaseAnim_Nova reads a DISJOINT pair (.Lee262, .Lee294), so
 * whichever way round the split goes, neither side needs the other's labels.
 * `.Lee29d` is read with `ldrsb`, so it is SIGNED char -- that is load-bearing
 * and easy to get wrong.
 * NO SHIMS, NO PINS: tools/shimcount.py is clean.
 *
 * WARNING: tools/split_s.py HAS NO --dry-run.  It rewrites asm/ and stage1.ld on
 * the spot; the flag is ignored, not honoured.
 *
 * ================================================================
 * THE HEADLINE: COUNTERS UNIFY, WALKING POINTERS SPLIT -- IN THE SAME FUNCTION
 * ================================================================
 *
 * This ROM is the cleanest instance in the corpus of the two halves of that rule
 * pulling in OPPOSITE directions, and both halves were measured here:
 *
 *   ONE COUNTER ACROSS FIVE DISJOINT LOOPS, worth 352 -> 363 aligned-equal.
 *   The ROM keeps the ewram-init counter, the four-particle counter, the spawn
 *   counter, the tail-draw counter AND the knockback counter ALL in r8.  Two
 *   separate counter variables (`i` and `n`) is what the first candidate had,
 *   and it is one allocno too many.
 *
 *   TWO WALKING POINTERS FOR THE SAME ARRAY, worth 363 -> 402 aligned-equal --
 *   THE BIGGEST SINGLE LEVER IN THE FUNCTION.  gBuffer is walked twice, once by
 *   the spawn loop and once by the tail-draw loop, and the ROM uses r7 for the
 *   first and r6 for the second.  ONE shared `Part *q` costs 39 aligned
 *   encodings; declaring `q` and `q2` and giving each loop its own recovers
 *   them.
 *
 * So the discriminator is not "is it reused" but WHAT IT CARRIES: a counter is
 * one variable however many loops it serves, and a POINTER INTO DATA is one
 * variable PER REGION.  The register file states it plainly -- r8 for every
 * counter, two different registers for the two walks of one array -- and the
 * two levers are worth +11 and +39 in the same candidate.
 *
 * ================================================================
 * THE OTHER LEVERS
 * ================================================================
 *
 * (3) `parity` INLINE AS `(frame & 1)` AT ITS SINGLE USE, worth 402 -> 408.  The
 *     ROM does compute it once into sp+0x14 in the inner-loop preheader, so a
 *     local looks right; it measures 6 encodings worse anyway.  Recorded because
 *     the frame slot is misleading evidence here.
 *
 * (4) `nx >> 8` IS A SHIFT AND `ny / 256` IS A DIVISION, in the same statement
 *     pair.  The ROM has a bare `asr r7, r4, #8` for the x screen coordinate and
 *     the full `cmp/bge/add #0xff/asr #8` rounding sequence for the y one.  Both
 *     operands are plain ints, so this is purely which OPERATOR the source used
 *     -- Anim_Froth's `v.x >>= 1` rule, and here the two forms sit four lines
 *     apart, which makes this function the controlled pair on it.
 *
 * (5) `q->z * 60 / 64` LEFT UNFOLDED.  The ROM emits `lsl #4 / sub / lsl #2`
 *     for the 60 and then the rounding sequence for the 64, exactly as written;
 *     no hand distribution is needed here (contrast batch 298's lever, where the
 *     ROM did NOT fold and the source had to).
 *
 * (6) `*(unsigned short *)(Data_ede48 + h - 2)` -- a BYTE offset into an
 *     unsigned char array, reinterpreted.  The ROM has `sub r3, r4, #2` off the
 *     already-computed `h` and then `ldrh r1, [r2, r3]`.  The arithmetically
 *     equal `Data_ede48_u16[w - 1]` costs a `lsl #1` because gcc would have to
 *     re-scale, and `h` is right there.
 *
 * ALSO LOAD-BEARING, right on the first candidate:
 *   - `pp = g; base = *pp++; ctx = *pp;` and `sheet = g[2]` for the third entry.
 *   - `int two = 2;` shared by both BuildDraw2DFuncEx stack arguments (dropping
 *     it for two literals is INERT here, but the ROM shares r6 so keep it).
 *   - `actor` IS a named local: inlining its single use costs 10 aligned
 *     encodings (398), because the ROM holds the actor pointer in r6 across the
 *     whole four-particle loop and reads `actor[2]` each iteration.
 *   - the ewram-init loop written against `&ewram_2010018` AS ITS OWN SYMBOL,
 *     not as `((Part *)gBuffer)[i].t`.  Those are the same ADDRESS
 *     (gBuffer + 0x18) but NOT the same relocation, and objcmp compares
 *     relocation symbols: the gBuffer spelling would emit `.word gBuffer+0x18`
 *     where the ROM has `.word ewram_2010018`.  The landed rom_d82b0_b.c already
 *     declares it `extern int ewram_2010018;` and takes its address, which is
 *     the form used here.
 *   - the palette load duplicated in BOTH arms of the f18 test (f18==0 takes
 *     FILE_86, f18==2 takes FILE_87, f18==1 takes neither) with its own `copy`
 *     assignment per arm -- the ROM loads the Func_8001af8 pool word twice.
 *     Hoisting `copy` to a single function-scope local is INERT, so the
 *     duplication is not what costs; it is recorded only so nobody "fixes" it.
 *   - `while (j != Lee29a[(*(State **)(base + 0x7828))->f18])` with the bound
 *     RE-DERIVED, never read through a cached `State **` -- Anim_Whirlwind's
 *     lever 2, and the same for the knockback loop's `f14` bound.
 *   - the spawn loop's two `break`s (at cnt == 0xc8 on the sound frame and
 *     cnt == 4 otherwise) as real breaks out of a `do {} while (i != 0x400)`.
 *   - `ang = (Random() & 0x7fff) + 0xffffc000;` -- the ROM pools 0xffffc000 and
 *     ADDS it rather than subtracting 0x4000, and `- 0x4000` is not the same
 *     instruction.
 *
 * ================================================================
 * THE BLOCKER: `base` IS SPILLED WHERE THE ROM KEEPS IT IN r11
 * ================================================================
 *
 * ATTRIBUTED TO global-alloc.  The frame is the whole statement of it: the ROM
 * is `sub sp, #0x40` and we are `sub sp, #0x48`, EIGHT BYTES OVER, and the extra
 * slot at sp+0x14 holds `base`.  The ROM's map is
 *
 *     0x00-0x04  outgoing arguments        0x1c  sheet
 *     0x08       (Part *)(base+0xe1<<7)+j, a loop.c giv
 *     0x0c       Lee2a9 + j, a second giv  0x20  j
 *     0x10       base + 0xdd0              0x24  frame
 *     0x14       frame & 1                 0x28  ctx
 *     0x18       fp                        0x2c-0x33  fns[2]
 *                                          0x34-0x3f  vec3_t v
 *
 * with `base` in r11 and NOT on the stack at all.  Because we spill it, every
 * one of the many `base + K` expressions pays an extra `ldr r1,[sp,#0x14]`, and
 * that is where the seventeen EXTRA instructions come from -- we are long, not
 * short, and the length is all reload traffic.  The register files are
 *
 *     ROM   r8 = counter   r9 = &v    r10 = cnt    r11 = base
 *     ours  r8 = counter   r10 = &v   r11 = counter-ish, base SPILLED
 *
 * WHAT RULES OUT THE ALTERNATIVES.  Two of allocno_compare's three inputs have
 * been moved without effect: declaration order (probed) and n_refs (probed by
 * inlining `parity`, `src2`, `copy` and `two`, each of which removes an
 * allocno -- all four INERT at 408 or worse).  `base` has the highest reference
 * count in the function, so priority already favours it; it loses only because
 * the allocator has run out of high registers, and the competitor it loses to is
 * the hoisted `&v`.  The ROM hoists `&v` ONLY for the inner-loop region (r9, set
 * in that loop's preheader) and reads v.x/v.y SP-RELATIVE at the draw calls;
 * ours hoists `&v` across everything and reads it through the register at every
 * site.  Getting the ROM's mixed addressing is the one thing that would free
 * r11, and NO source spelling tried here splits it -- `&v` is taken once, by the
 * Func_80e3944 call, and gcc decides on its own how far to carry the address.
 *
 * WHERE TO GO NEXT.  This is the same REG_ALLOC_ORDER class as the other 78
 * parks, but unlike most of them the rest of the program is now known good (the
 * relocation sequence was exact from the first candidate and three independent
 * structural levers have landed).  The untried lever is a candidate that copies
 * `v.x` and `v.y` into scalars immediately after Func_80e3944 returns, so the
 * address pseudo dies at the call and the draw arguments come from plain
 * locals -- that is the only remaining way to shorten `&v`'s live range from the
 * C level, and it was not measured here.
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
    int x, y, z, fc, vy, f14, t;
} Part;

extern void *iwram_3001eec[];
extern void *iwram_3001e80;
extern void *gPtrs[];
extern unsigned char gBuffer[];
extern int ewram_2010018;
extern unsigned char Data_ede48[];
extern unsigned char Lee29a[] __asm__(".Lee29a");
extern signed char Lee29d[] __asm__(".Lee29d");
extern unsigned char Lee2a9[] __asm__(".Lee2a9");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int *_GetBattleActor(int id);
extern int  Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _Func_80bd7dc(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern int  sin(int a);
extern int  cos(int a);
extern void _PlaySound(int id);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Volcano(void *context)
{
    vec3_t v;
    DrawFn fns[2];
    void *ctx;
    int frame;
    int j;
    void *sheet;
    DrawFn *fp;
    unsigned char *src2;
    void **g;
    void **pp;
    unsigned char *base;
    CopyFn copy;
    DrawFn f1;
    int *actor;
    Part *p;
    Part *q;
    Part *q2;
    int i;
    int two;
    int cnt;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    sheet = g[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    REG_BLDALPHA = 0x1010;
    LoadVFXFile(FILE_85, base, 1, 1);
    LoadVFXFile(FILE_73, sheet, 0, 0);
    if ((*(State **)(base + 0x7828))->f18 == 0) {
        void *pal = GetFile(FILE_86);
        int d0;
        copy = Func_8001af8;
        d0 = 0xa0;
        d0 <<= 19;
        copy((volatile u16 *)d0, pal, 0x80);
    } else if ((*(State **)(base + 0x7828))->f18 == 2) {
        void *pal = GetFile(FILE_87);
        int d0;
        copy = Func_8001af8;
        d0 = 0xa0;
        d0 <<= 19;
        copy((volatile u16 *)d0, pal, 0x80);
    }
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    fns[0] = (DrawFn)gPtrs[0xb8 / 4];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, two);
    f1 = (DrawFn)gPtrs[0xbc / 4];
    fp = fns;
    fp[1] = f1;
    i = 0;
    do {
        *(int *)((char *)&ewram_2010018 + i * 0x1c) = 0;
        i++;
    } while (i != 0x400);
    actor = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    i = 0;
    p = (Part *)(base + (0xe1 << 7));
    do {
        int x = ((Random() & 0xf) + 0x48) << 16;
        p->y = 0;
        p->x = x;
        p->z = Lee29d[(*(State **)(base + 0x7828))->f18 * 4 + i] << 16;
        if (actor[2] < 0) {
            p->x = -x;
        }
        i++;
        p++;
    } while (i != 4);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    StartTask(Task_BlitAnim, 0x90 << 3);
    frame = 0;
    do {
        void *cam = iwram_3001e80;
        if ((*(State **)(base + 0x7828))->f18 == 2) {
            if (frame <= 0x3f) {
                if ((*(State **)(base + 0x7828))->f4 == 0) {
                    *(u16 *)((char *)cam + 0x36) = *(u16 *)((char *)cam + 0x36) + 0xc0;
                } else {
                    *(u16 *)((char *)cam + 0x36) = *(u16 *)((char *)cam + 0x36) - 0xc0;
                }
            }
        }
        if (frame == 0x10) {
            _Func_80bd7dc(0x86);
        }
        InitMatrixStack();
        MatrixSetLook(cam, (char *)cam + 0xc);
        if (frame <= 0x3f) {
            j = 0;
            src2 = base + (0xdd << 4);
            while (j != Lee29a[(*(State **)(base + 0x7828))->f18]) {
                Func_80e3944((vec3_t *)((Part *)(base + (0xe1 << 7)) + j), &v);
                v.x = v.x / 2;
                v.y = v.y - 8;
                if (frame == Lee2a9[j]) {
                    _PlaySound(0x91);
                }
                if (frame >= Lee2a9[j] + 4) {
                    int m = (frame * 16 + j * 25) % 0x68;
                    fp[j & 1](ctx, base, v.x - 0x11, v.y - m - 0x68, 0x22, 0x68);
                    fp[j & 1](ctx, base, v.x - 0x11, v.y - m, 0x22, m);
                    if ((frame & 1) != 0) {
                        fns[0](ctx, src2, v.x - 0x14, v.y - 0x18, 0x14, 0x25);
                        fp[1](ctx, src2, v.x, v.y - 0x18, 0x14, 0x25);
                    } else {
                        unsigned char *src3 = base + 0x10b4;
                        fns[0](ctx, src3, v.x - 0x14, v.y - 0x18, 0x14, 0x25);
                        fp[1](ctx, src3, v.x, v.y - 0x18, 0x14, 0x25);
                    }
                }
                if (frame == Lee2a9[j] || frame >= Lee2a9[j] + 0x10) {
                    cnt = 0;
                    i = 0;
                    q = (Part *)gBuffer;
                    do {
                        if (q->t == 0) {
                            int r = Random() & 0x3ff;
                            int ang = (Random() & 0x7fff) + 0xffffc000;
                            q->x = v.x << 8;
                            q->y = (v.y << 8) + (0x80 << 5);
                            r += 0x20;
                            q->z = (r * sin(ang)) >> 15;
                            q->vy = -((r * cos(ang)) << 1) >> 15;
                            cnt++;
                            if (frame == Lee2a9[j]) {
                                q->t = (Random() & 7) + 0x30;
                                if (cnt == 0xc8) {
                                    break;
                                }
                            } else {
                                q->t = (Random() & 7) + 0x18;
                                if (cnt == 4) {
                                    break;
                                }
                            }
                        }
                        i++;
                        q++;
                    } while (i != (0x80 << 3));
                }
                if (frame == Lee2a9[j]) {
                    *(int *)(base + 0x77a8) = 2;
                    i = 0;
                    while (i != (*(State **)(base + 0x7828))->f14) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[i], 0xa, 5, i, 8);
                        _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[i], 1);
                        i++;
                    }
                }
                j++;
            }
        }
        i = 0;
        q2 = (Part *)gBuffer;
        do {
            int t = q2->t;
            if (t > 0) {
                int nx, ny, sy;
                q2->t = t - 1;
                nx = q2->x + q2->z;
                ny = q2->y + q2->vy;
                q2->x = nx;
                q2->y = ny;
                q2->z = q2->z * 60 / 64;
                q2->vy = q2->vy * 60 / 64 - 0x10;
                sy = ny / 256;
                if (sy > 0x78) {
                    q2->vy = -q2->vy / 2;
                } else if (nx >= 0 && (nx >> 8) <= 0x7e && ny >= 0) {
                    int w = (t - 0x11) / 8;
                    int h;
                    if (w <= 0) {
                        w = 1;
                    }
                    h = w * 2;
                    fp[i & 1](ctx,
                              (char *)sheet + *(unsigned short *)(Data_ede48 + h - 2),
                              (nx >> 8) - w / 2, sy - w, w, h);
                }
            }
            i++;
            q2++;
        } while (i != (0x80 << 3));
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x60);
    gfree(0x2f);
    gfree(0x2e);
    StopTask(Task_BlitAnim);
    AnimEnd();
}
