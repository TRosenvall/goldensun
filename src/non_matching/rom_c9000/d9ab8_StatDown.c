/* BaseAnim_StatDown -- NON-MATCHING, 502 encodings of 562, size 1240 against the ROM's
 * 1248 (-8), 558 instructions against 562.  531 instructions.
 *
 * READ THE STRUCTURE, NOT THE 502.  FRAME `sub sp, #0x64` IS EXACT AND ALL TWELVE SPILL
 * SLOTS LAND ON THE ROM'S EXACT OFFSETS:
 *
 *   0x34 variant | 0x30 base | 0x2c ctx | 0x28 frame | 0x24 n | 0x20 xbias
 *   0x1c &fns    | 0x18 view | 0x14 view+0xc | 0x10 lvl*0x2b8 | 0xc t-0x18 | 0x8 n*0x380
 *
 * plus the aggregates zv 0x58, ov 0x4c, tv 0x40, fns 0x38.  THE RELOCATION SEQUENCE IS
 * IDENTICAL -- 49 symbols in the same order, offsets shifted only by the 8-byte deficit,
 * so no symbol, pool-base or veneer mismatch remains.  tryc --align: 267 instructions in
 * disagreeing regions of 568, of which a classifier says 51 of 108 windows are PURE
 * REGISTER RENAMES.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/d9ab8_StatDown.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_a.s --func BaseAnim_StatDown
 * TWO functions; a TEXT-ONLY split is required (no .rodata in the file -- .Leea08,
 * .Leea20 and .Leea2c are .global in rom_d9ab8_c_c_c_c_c.s).  Element types confirmed from
 * the ROM's indexing: Leea08 is unsigned short[12] (`ldrh [r2,r3]`, r3 = idx*2, 24 bytes),
 * Leea20 and Leea2c are unsigned char[12].  No new symbols needed.
 *
 * ================================================================
 * `base` MUST BE *SPILLED* HERE, NOT PINNED -- AND THE THING THAT SPILLS IT IS THE BLIT
 * FUNCTION POINTER
 * ================================================================
 *
 * The opposite of cf2a0_Revive.c's advice, and the THIRD measured caution against this
 * bank's headline pin rule.  The ROM's seven callee-saved registers are all spoken for
 * (r5 idx, r6 ab/sx/px/m, r7 mag/pp, r8 t, r9 i, r10 sy/py, r11 &ov), so `base` -- 17
 * refs over a function-long range, the LOWEST global_alloc priority -- gets nothing.
 *
 * Reproducing that needed a CHAIN, not a pin:
 *   sx and sy must be ONE VARIABLE EACH, shared between the target-loop body and the draw
 *   loop.  Alone this took the frame 0x60 -> 0x64 but left base in r11.
 *   THE BLIT CALLEE MUST BE LOADED LATE -- `((DrawFn)fns[0])(...)` called directly through
 *   the array, never `DrawFn f = fns[0]; f(...)`.  A named `f` is live across the
 *   __modsi3 in the argument list, so it takes a CALLEE-SAVED register (measured:
 *   _call_via_r9), which is exactly what lets base keep r11.  With direct array calls the
 *   pointer takes r4 (call-clobbered under -fcall-used-r4), sy escapes to r10, and base
 *   spills.  All three sites then emit _call_via_r4 like the ROM.
 *
 * Path: frame 0x60 / 337 -> shared sx,sy: 0x64 but base in fp / 361 -> single DrawFn f:
 * base spilled but _call_via_r9 / 311 -> direct array calls: 0x64, base at sp+0x30, all
 * _call_via_r4 / 334 -> 267 after the rest.
 *
 * AND THE DECISIVE EVIDENCE THAT base IS A RELOAD SPILL RATHER THAN AN ADDRESS-TAKEN
 * LOCAL: sp+0x30 sits BELOW the parameter spill at 0x34, and expand-time slots are always
 * allocated ABOVE reload's.  `(void)&base;` did not set addressability (base stayed in
 * r11, 321); `unsigned char *volatile base` DID reproduce the clamp loop's three-add shape
 * but cost frame 0x6c, _call_via_sl and 352.
 *
 * `&fns` AT sp+0x1c IS A REAL SOURCE-LEVEL POINTER LOCAL.  The ROM reaches fns[0] as
 * `ldr r4,[sp,#0x38]` (a direct frame ref) but fns[1] as
 * `ldr r0,[sp,#0x1c]; ldr r4,[r0,#4]`.  That only happens if the source has BOTH an array
 * and a pointer to it: `void *fns[2]; void **fp = fns;`.  Drop `fp` and the slot and frame
 * are wrong.
 *
 * THE TWO STACK-RESIDENT IVs' SLOT ORDER IS SET BY DECLARATION POSITION, and `boff` must
 * be declared at the TOP of the target-loop body -- writing `pp = gBuffer + n*0x380` where
 * it is USED puts 0xc and 0x8 the wrong way round.  This REFINES the per-target-IV rule:
 * the ROM's four are `n`, `t = frame - n*4`, `t - 0x18` and `boff`; `t` and `boff` are
 * explicit locals while `t - 0x18` MUST STAY AN EXPRESSION, because that is what makes gcc
 * fold `(t-0x18)+3` into the ROM's `mov r3,r8 / sub r3,#0x15`.
 *
 * THE 4-INSTRUCTION DEFICIT IS ENTIRELY THE CLAMP LOOP (ROM instructions 63-106); window
 * deltas sum to +3 there and 0 elsewhere.  The ROM rebuilds 0x2580 INSIDE the outer loop
 * where ours CSEs it with an identical LoadVFXFile argument in the same basic block --
 * unavoidable from C, since the two 9600s are the same expression and gcse unifies them.
 * The ROM also has `mov r12, r4`, a copy of `clamp` into ip, which no source form survived
 * copy-propagation; and it holds the inner bound in lr and rebuilds the step inline.
 *
 * Also confirmed/needed: AnimStart before REG_BG2PA; a 3-arm variant ladder (a 2-arm
 * ternary folds and loses `cmp #0`); `p->y` duplicated in BOTH arms of the particle loop;
 * `xbias = ...` BEFORE `REG_BG2X = ...` (-37 instructions); `cos(ang) * mag` not
 * `mag * cos(ang)` -- the multiply-operand lever firing the other way here; and
 * `sy = ov.y;` before `sx = ov.x + xbias;` in the draw loop but the REVERSE order in the
 * target body (-13).
 *
 * MEASURED NEGATIVES: three per-loop counters 373 (one function-scope counter for
 * loop1-outer/particle/draw and one for loop1-inner/target is right); an explicit
 * `int t2 = t - 0x18;` 363 then 284; five clamp-loop spellings 270-320, where only
 * `dst = base + 0x2580; dst += i*lim;` removes the base-absorbing giv (267);
 * `int c = 64 - i*7` as a strength-reduced clamp 312; REG_BG2X before xbias 306.
 *
 * AN INSTRUMENT WORTH ADOPTING FOR THIS WHOLE BANK: `xgcc ... -dg` writes
 * `x.c.18.greg`, whose `;; N regs to allocate:` line IS the global_alloc PRIORITY ORDER
 * and whose `;; Register dispositions:` block gives the final hard-register assignment.
 * That is how the f->r4 / sy->r10 / base->spill chain was IDENTIFIED rather than guessed
 * -- pseudo 35 was base, at position 30 of 46 with {5,6,7,8,9,10} taken and r11 free.
 *
 * No per-file Makefile flag override applies to this stem; no flag probes were run.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef struct {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
    int f1c;
    int f20;
    short ids[4];
} State;

typedef struct {
    int x;
    int y;
    int z;
    int p3;
    int p4;
    int p5;
    int t;
} Part;

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

extern int *iwram_3001eec[];
extern void *iwram_3001e80;
extern Part gBuffer[];

extern unsigned short Leea08[] __asm__(".Leea08");
extern unsigned char Leea20[] __asm__(".Leea20");
extern unsigned char Leea2c[] __asm__(".Leea2c");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern int sin(int a);
extern int cos(int a);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);
extern void *_GetBattleActor(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixTranslatev(vec3_t *v);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(void *p, int a, int b);

void BaseAnim_StatDown(void *context, int variant)
{
    vec3_t zv;
    vec3_t ov;
    vec3_t tv;
    void *fns[2];
    unsigned char *base;
    void *ctx;
    int frame;
    int n;
    int xbias;
    void **fp;
    void *view;
    int **tp;
    int fid;
    int i;
    int clamp;
    int arg;
    State **slot;

    tp = (int **)iwram_3001eec;
    base = (unsigned char *)*tp++;
    ctx = (void *)*tp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BG2PA = 0x100;
    if (variant == 0) {
        LoadVFXFile(FILE_9c, base, 1, 1);
    } else {
        LoadVFXFile(FILE_9b, base, 1, 1);
    }
    if (variant == 0) {
        fid = FILE_bb;
    } else if (variant == 1) {
        fid = FILE_b7;
    } else {
        fid = FILE_bb;
    }
    {
        register int q0 __asm__("r0");
        register int q1 __asm__("r1");
        register int q2 __asm__("r2");
        CopyFn copy;
        q1 = (int)GetFile(fid);
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, (void *)q1, q2);
    }
    LoadVFXFile(FILE_9d, base + 0x2580, 0, 0);

    clamp = 0x39;
    for (i = 1; i != 8; i++) {
        unsigned char *src;
        unsigned char *dst;
        int lim = 0x2b8;
        n = 0;
        src = base + 0x2580;
        dst = base + 0x2580;
        dst += i * lim;
        do {
            int v = *src++;
            if (v > clamp) {
                v = clamp;
            }
            if (v < 0) {
                v = 0;
            }
            *dst++ = v;
            n++;
        } while (n != lim);
        clamp -= 7;
    }

    if ((*(State **)(base + 0x7828))->f4 == 1) {
        xbias = -0x70;
        REG_BG2X = 0xffff9000;
    } else {
        xbias = 0;
        REG_BG2X = 0;
    }

    {
        Part *p = gBuffer;
        int mag = 0xc0;
        i = 0;
        do {
            int ang = Random() & 0xffff;
            p->x = 0;
            if (variant == 0) {
                p->y = ((i & 0x1f) / 4 * 3 << 17) + 0xfff60000;
                p->z = ((i % 4) << 17) + 0xfffe0000;
            } else {
                p->y = ((i & 0x1f) / 4 * 3 << 17) + 0xfff60000;
                p->z = ((i % 4) << 19) + 0xfff00000;
            }
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                p->p3 = 0x80 << 10;
            } else {
                p->p3 = 0xfffe0000;
            }
            p->p4 = ((cos(ang) * mag) >> 6) + (0x80 << 9);
            p->p5 = (sin(ang) * mag) >> 6;
            p->t = Random() & 0xff;
            i++;
            p++;
        } while (i != (0x80 << 2));
    }

    slot = (State **)(base + 0x7828);
    fp = fns;
    BuildDraw2DFuncs((*slot)->f4, fp);
    *(int *)(base + (0xef << 7)) = 2;
    arg = 0x90;
    *(int *)(base + 0x7784) = 0x32;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);

    frame = 0;
    while (frame != (*(State **)(base + 0x7828))->f14 * 4 + 0x40) {
        view = iwram_3001e80;
        if (frame == 0x48) {
            _Func_80bd7dc(0);
        }
        n = 0;
        while (n != (*(State **)(base + 0x7828))->f14) {
            int t = frame - n * 4;
            int boff = n * 0x380;
            int *actor;
            int ab;
            actor = (int *)_GetBattleActor((*(State **)(base + 0x7828))->ids[n]);
            ab = *actor;
            if (t > 0) {
                int sx;
                int sy;
                InitMatrixStack();
                MatrixSetLook(view, (char *)view + 0xc);
                tv.x = *(int *)(ab + 8);
                tv.y = 0xa0 << 13;
                tv.z = *(int *)(ab + 0x10);
                InitMatrixStack();
                MatrixSetLook(view, (char *)view + 0xc);
                MatrixTranslatev(&tv);
                zv.x = 0;
                zv.y = 0;
                zv.z = 0;
                Func_80e3944(&zv, &ov);
                sx = ov.x + xbias;
                sy = ov.y;
                if (variant == 0) {
                    if (t <= 0x1a) {
                        ((DrawFn)fns[0])(ctx, base + (t / 4) % 7 * 960, sx - 0xc, sy - 0x14, 0x18, 0x28);
                    }
                } else {
                    if (t <= 0x17) {
                        ((DrawFn)fp[1])(ctx, base + (t / 4) % 6 * 1600, sx - 0x14, sy - 0x14, 0x28, 0x28);
                    }
                }
                if (t == 0x18) {
                    _PlaySound(0x8f);
                }
                if ((unsigned int)(t - 0x18) <= 0x24) {
                    int lvl = 0;
                    Part *pp;
                    if (t > 0x1c) {
                        lvl = (t - 0x18) / 4;
                        if (lvl > 7) {
                            lvl = 7;
                        }
                    }
                    pp = (Part *)((char *)gBuffer + boff);
                    i = 0;
                    do {
                        int m = i % 4 * 3;
                        int r = ((pp->t + t) / 8) % 3;
                        int idx;
                        Func_80e3944((vec3_t *)pp, &ov);
                        idx = m + r;
                        sy = ov.y;
                        sx = ov.x + xbias;
                        ((DrawFn)fns[0])(ctx, base + (lvl * 0x2b8 + Leea08[idx]) + 0x2580, sx, sy,
                          Leea20[idx], Leea2c[idx]);
                        Func_80e38b8(pp, 0x3c, 0);
                        i++;
                        pp++;
                    } while (i != 0x18);
                }
            }
            n++;
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
