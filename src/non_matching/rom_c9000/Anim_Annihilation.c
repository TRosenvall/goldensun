/* Anim_Annihilation (asm/rom_c9000/rom_e7320_c_c.s, 438 instructions) --
 * NON-MATCHING, 450 of 459 encodings differ.  SIZE 1064 against the ROM's 1040
 * (+24) and COUNT 471 against 459 (+12), so THAT FIGURE IS SATURATED, NOT A
 * DISTANCE.  tools/aligncmp.py: 80.2% aligned-equal is NOT an objcmp number and
 * must not be quoted as one.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/e7320_Annihilation.c \
 *     asm/rom_c9000/rom_e7320_c_c.s --func Anim_Annihilation
 *
 * THE RELOCATION SEQUENCE IS ALREADY THE ROM'S -- same symbols in the same order,
 * offsets shifted by the extra bytes.  No wrong symbol, no wrong argument order.
 *
 * SPLIT SHAPE: TEXT-ONLY FOR THIS FUNCTION, NO NEW EXPORTS.  tools/datacheck.py
 * says `Anim_Annihilation reads no data label -> split needs NO new export`, but
 * the STEM has a `.rodata` section (stage1.ld line 1989) that the other seven
 * functions feed on, so the split must keep it.  rom_e7320_c_c.s holds EIGHT
 * functions -- Func_80e7338, Func_80e73a0, BaseAnim_Meteor, Anim_Ramses,
 * Anim_DragonCloud, Anim_Annihilation, Anim_Ragnarok, Anim_TitanBlade -- and
 * Anim_Annihilation is the SIXTH, so landing it is a three-way split:
 *
 *   asm/rom_c9000/rom_e7320_c_c_a.s   the first five  (unchanged asm, keeps .rodata)
 *   src/rom_c9000/rom_e7320_c_c_b.c   THIS FILE
 *   asm/rom_c9000/rom_e7320_c_c_c.s   Ragnarok + TitanBlade (unchanged asm)
 *
 * and stage1.ld's `.text` row (line 1930) becomes those three in order while the
 * `.rodata` row (line 1989) follows whichever piece keeps the data section.  The
 * five dot-labels BaseAnim_Meteor / Anim_Ramses / Anim_Ragnarok / Anim_TitanBlade
 * read must stay visible to their own pieces -- they are intra-piece if the
 * split is cut as above, so NO `.global` is needed for THIS conversion.
 *
 * SHIMS: none.  `python3 tools/shimcount.py` is clean -- zero pins.  No
 * fakematch.txt row needed.  No per-file Makefile flag override applies.
 *
 * ================================================================
 * NAMED BLOCKER: `base` LOSES r11, AND THAT COSTS THE 12
 * ================================================================
 *
 * The ROM keeps `base` in r11 for the whole function (`mov r11, r1` at entry,
 * then `add r5, r11` / `add r2, r11` at every state write).  This candidate
 * SPILLS base to sp+0x1c and reloads it (`ldr r3, [sp, #28] / add r2, r3, r0`),
 * which is one extra instruction at each of the ~10 sites plus the extra frame
 * word: frame 0x5c against the ROM's 0x58, nine spill words against eight.
 *
 * WHAT TAKES r11 INSTEAD is the `slot` pointer, and that is the lever to work:
 * declaring `State **slot` gives it a long live range and r9/r11, and base loses.
 * The ROM DOES NOT HOLD `slot` -- it DERIVES `base + 0x7828` THREE SEPARATE TIMES
 * (r5 in the prologue, a gcse hoist to sp+0x14 for the frame loop, and a fresh
 * `ldr r5,=0x7828 / add r5,r11` inside the frame loop at 0x080e91f4).  That is
 * the recorded "do not cache a struct slot the ROM re-derives" rule, and
 * Anim_Hail's "`slot` named at the top but written INLINE inside the frame loop".
 *
 * MEASURED, and NONE of the three scopings recovers r11:
 *   `slot` local everywhere (this file)               1064 / 471, 450 diff
 *   `*(State **)(base + 0x7828)` inline at all 13 sites
 *                                                    1072 / 475, 450 diff
 *   `slot` for the prologue only, inline from the post-init-loop
 *     GetBattleActorPos2 onward                       1068 / 473, 456 diff
 *   `register unsigned char *base __asm__("r11")`     1072 / 474, 452 diff
 *   dropping the `ab` local (inline the actor deref)   1064 / 471 (INERT)
 *
 * THE PIN IS A NEGATIVE AND IT IS THE DOCUMENTED REASON: `base` is dereferenced
 * at OFFSETS (`*(int *)(base + 0x7824) = 1`), and docs/elevation.md's
 * "DO NOT PIN A POINTER YOU DEREFERENCE AT AN OFFSET TO A HIGH REGISTER" applies
 * -- a hard hi-reg inside the MEM makes reload rebuild the whole address.  An
 * UNPINNED pseudo that global_alloc happens to put in r11 is what is wanted, so
 * the fix is PRESSURE, not a pin, exactly as Anim_Drain recorded.
 *
 * ================================================================
 * SECOND BLOCKER, 4 bytes of frame: THE DMA LOCAL'S SLOT
 * ================================================================
 *
 * The ROM's stack has a FOUR-BYTE object at sp+0x30, between `v` (sp+0x34) and
 * `fns[2]` (sp+0x28), and it is DMA3_FILL's internal `u32 value`:
 *
 *     ldr r3, =0x3f3f3f3f / add r0, sp, #0x30 / str r3, [r0]
 *     ldr r1, [sp, #0x24] / ldr r3, =REG_DMA3SAD / ldr r2, =0x85001000
 *     stmia r3!, {r0, r1, r2} / sub r3, #0xc
 *
 * i.e. `DMA3_FILL(ctx, 0x3f3f3f3f, 0x4000)` -- 0x85000000 | (0x4000/4) is exactly
 * the ROM's 0x85001000, so the helper is right and this file uses it.  What is
 * wrong is the ORDER: an inline's local is allocated when the CALL is expanded,
 * so ours lands BELOW `fns` while the ROM's is ABOVE it.  For the ROM's map,
 * `value` must be allocated between `v` and `fns` -- meaning `fns[2]` is declared
 * in a block entered AFTER the DMA site, or the four bytes are a function-scope
 * `int` declared between them and the transfer goes through DMA3_SET.  Reading
 * the ROM's slot map is the cheapest way in:
 *
 *   0x4c out   0x40 pos   0x34 v   0x30 DMA value   0x28 fns[2]
 *   0x24 ctx   0x20 gfx   0x1c view   0x18 &pos   0x14 base+0x7828
 *   0x10 &out  0x0c view+0xc  0x08 the frame giv
 *
 * and the expand-object block (out, pos, v, value, fns = 48 bytes) plus eight
 * spill words plus eight bytes of outgoing args is exactly the ROM's 0x58.
 * SPILL SLOTS BELOW THE LAST SOURCE LOCAL ARE gcc's: 0x08-0x18 are a giv and
 * three hoisted addresses, so do not try to name them.
 *
 * ================================================================
 * WHAT IS ALREADY RIGHT, AND READ OFF THE REFERENCE
 * ================================================================
 *
 *   - The frame-loop blit source is a STRENGTH-REDUCED giv, not a multiply: the
 *     ROM keeps `base - 0x5100 + frame * 0xd80` at sp+0x08 and adds
 *     `mov r3,#0xd8 / lsl r3,#4` to it each frame.  Written
 *     `base + (frame - 6) * (0xd8 << 4)` -- the SHIFT LEFT UNFOLDED, per the
 *     shape-decides-strength-reduction lever.  sp+0x08 being the LOWEST slot is
 *     the confirmation that it is a giv and not a declared local.
 *   - `0xff` STAYS A BARE LITERAL in the init loop: the ROM hoists it into r7
 *     across the three `Random() & 0xff`, the Anim_Hail bare-literal rule.
 *   - `q->t = i / 4 * 2 + 0x10` for `asr #2 / lsl #1 / add #0x10`, and
 *     `q->vy = ((Random() & 0xff) - 0x7f) << 12` with the subtraction inside.
 *   - `if (q->x > 0) q->vx = -q->vx;` AFTER all six stores, re-reading q->x.
 *   - `int t = p->t;` cached for the guard AND for `n = (t >> 3) + 2`, but
 *     `p->t = p->t - 1` RE-READS the slot after Func_80e38b8 -- the ROM does
 *     both, so the caching rule cuts both ways within one loop body.
 *   - TWO NON-IDENTICAL ARMS ON PURPOSE.  The `(unsigned)(frame - 6) <= 5` blit
 *     has `out.x / 2 - 0x18` in the f4 == 0 arm and `out.x / 2` in the other,
 *     and the ROM keeps a `.pool_aligned` between them.  Per the cross-jumping
 *     hazard in the brief: DO NOT tidy these two arms to look alike -- the ROM
 *     keeps both `bl _call_via_r4` sites.
 *   - `(s * 3 * 8 + s) << 7` for `lsl #1 / add / lsl #3 / add / lsl #7`, and
 *     `off = 0x96 << 6` guarded by `(*slot)->f18`.
 *   - `Func_80d6888(id, 0xa, -1, -1, 0)` twice (frame 6 and frame 0xe) as TWO
 *     separate `if`s -- the ROM emits both bodies, not a shared one.
 *   - `gPtrs[0xb8 / 4]` for `ldr r3,=gPtrs / add r3,#0xb8 / ldr r4,[r3]`; 0xb8 is
 *     past the `ldr [rn, #imm5*4]` range so the `add` is forced, not a lever.
 *
 * NEXT STEP: fix the frame first -- the slot map cannot be read while it is 4
 * bytes long, and every sp offset in the body is wrong until it is.  Then get
 * base back into r11 by REMOVING pressure (the three `slot` scopings above are
 * the wrong axis; try the remaining source locals `tbl`, `pp`, `p`, `i`).
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void _Func_80b82c4(int a, int b, int c, int d);
extern void WaitFrames(unsigned int n);
extern void *_GetBattleActor(int id);
extern int Random(void);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _Func_80bd7dc(int a);
extern void _PlaySound(int id);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80e38b8(Part *g, int a, int b);
extern void Unk_080D655C(unsigned int a);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);

void Anim_Annihilation(void *context)
{
    vec3_t out;
    vec3_t pos;
    vec3_t v;
    DrawFn fns[2];
    void *ctx;
    unsigned char *gfx;
    void *view;
    void **tbl;
    void **pp;
    unsigned char *base;
    State **slot;
    Part *p;
    int *ab;
    int i;
    int frame;

    tbl = iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    gfx = (unsigned char *)tbl[2];
    view = *(void **)((char *)tbl - 0x6c);
    slot = (State **)(base + 0x7828);
    p = gBuffer;
    *slot = (State *)context;
    AnimStart(0);
    LoadVFXFile(FILE_96, base, 1, 1);
    LoadVFXFile(FILE_63, p, 1, 1);
    LoadVFXFile(FILE_73, gfx, 0, 0);
    _Func_80b82c4((*slot)->f8, (*slot)->ids[0], 4, 0);
    WaitFrames(1);
    ab = (int *)_GetBattleActor((*slot)->ids[0]);
    {
        int *a = (int *)*ab;
        Part *q = (Part *)(base + (0xe1 << 7));
        i = 0;
        do {
            q->x = a[2];
            q->y = a[3];
            q->z = a[4];
            q->vx = (Random() & 0xff) << 11;
            q->vy = ((Random() & 0xff) - 0x7f) << 12;
            q->vz = ((Random() & 0xff) - 0x7f) << 12;
            if (q->x > 0) {
                q->vx = -q->vx;
            }
            q->t = i / 4 * 2 + 0x10;
            i++;
            q++;
        } while (i != 0x40);
    }
    GetBattleActorPos2((*slot)->ids[0], &pos);
    StartTask(Task_BlitAnim, 0x90 << 3);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    frame = 0;
    do {
        if (frame == 8) {
            _Func_80bd7dc(0x86);
        }
        if ((*slot)->f18 != 0 && frame == 8) {
            _PlaySound(0xd4);
        }
        GetBattleActorPos2((*slot)->f8, &out);
        if ((unsigned)(frame - 6) <= 5) {
            if ((*slot)->f4 == 0) {
                BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
            } else {
                BuildDraw2DFuncEx(0x2e, 7, 7, 7, 3);
            }
            fns[0] = (DrawFn)gPtrs[0xb8 / 4];
            if ((*slot)->f4 == 0) {
                fns[0](ctx, base + (frame - 6) * (0xd8 << 4),
                       out.x / 2 - 0x18, out.y - 0x18, 0x30, 0x48);
            } else {
                fns[0](ctx, base + (frame - 6) * (0xd8 << 4),
                       out.x / 2, out.y - 0x18, 0x30, 0x48);
            }
            gfree(0x2e);
        }
        if ((unsigned)(frame - 0x10) <= 0x1f) {
            int s = (frame - 0x10) / 2;
            int off;
            BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
            fns[0] = (DrawFn)gPtrs[0xb8 / 4];
            if (s > 2) {
                s = 2;
            }
            off = 0;
            if ((*slot)->f18 != 0) {
                off = 0x96 << 6;
            }
            fns[0](ctx, (char *)gBuffer + off + ((s * 3 * 8 + s) << 7),
                   pos.x / 2 - 0x14, pos.y - 0x30, 0x28, 0x50);
            Unk_080D655C(0x2710);
            gfree(0x2e);
        }
        if (frame == 8) {
            DMA3_FILL(ctx, 0x3f3f3f3f, 0x4000);
        }
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        if (frame > 3) {
            BuildDraw2DFuncs((*slot)->f4, (void **)fns);
            p = (Part *)(base + (0xe1 << 7));
            i = 0;
            do {
                int t = p->t;
                if (t > 0) {
                    int n;
                    int n2;
                    Func_80e3944(p, &v);
                    v.x = v.x >> 1;
                    v.y = v.y + pos.y - 0x70;
                    n = (t >> 3) + 2;
                    n2 = n * 2;
                    fns[(i / 2) & 1](ctx, gfx + Data_ede48[n - 1],
                                     v.x - n / 2, v.y - n, n, n2);
                    Func_80e38b8(p, 0x3c, -0x400);
                    p->t = p->t - 1;
                }
                i++;
                p++;
            } while (i != 0x40);
            gfree(0x2f);
            gfree(0x2e);
        }
        if (frame == 8) {
            _SetBattleActorKnockback((*slot)->ids[0], 4);
            *(int *)(base + 0x77a8) = 4;
        }
        if (frame == 6) {
            Func_80d6888((*slot)->ids[0], 0xa, -1, -1, 0);
        }
        if (frame == 0xe) {
            Func_80d6888((*slot)->ids[0], 0xa, -1, -1, 0);
        }
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x40);
    StopTask(Task_BlitAnim);
    AnimEnd();
}
