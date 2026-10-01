/* Anim_Nereid (asm/rom_c9000/rom_d2d98.s:93, 0x080d2d98, 675 ROM instructions) --
 * NON-MATCHING, 619 of 707 encodings differ.
 *
 * SIZE IS 1580 AGAINST THE ROM'S 1576 (+4) and COUNT IS 708 AGAINST 707 (+1),
 * so NEITHER IS EXACT and the objcmp figure above is SATURATED -- it is not a
 * distance and cannot rank two candidates.  Rank with tools/aligncmp.py:
 * 56.6% aligned-equal (400 of 707, 418 in 121 hunks).  That is an aligncmp
 * figure and must never be quoted on the claim line.
 *
 * THE RELOCATION SYMBOL SEQUENCE IS EXACT: all 63 relocations, same symbols in
 * the same order, on the FIRST candidate -- every call target, every argument
 * order and every data symbol is right.  Only the offsets differ.
 *
 * FRAME IS EXACT: `sub sp, #0x3c` both sides, and every spill slot is on the
 * ROM's offset -- ctx 0x30, base 0x2c, gfx 0x28, &fns 0x24, py 0x20, px 0x1c,
 * vy 0x18, vx 0x14, yo 0x10, fo 0x0c, plus the r4 spill at 0x08 and fns[2] at
 * 0x34.  Do not re-derive the slot map; it is settled.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Nereid.c \
 *     asm/rom_c9000/rom_d2d98.s --func Anim_Nereid
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT, TWO NEW EXPORTS.  tools/datacheck.py: the stem
 * has a `.rodata` section and Anim_Nereid reads `.Lee1ac` and `.Lee1b4` and
 * nothing else.  `tools/split_s.py --dry-run` REFUSES until they are exported:
 *
 *     .global .Lee1ac
 *     .global .Lee1b4
 *
 * and reports exactly two crossing labels, both this function's.  rom_d2d98.s
 * holds SIX functions -- Anim_Nereid, Anim_Froth, Anim_Whirlwind, Anim_Prism,
 * ColorCycleVFXPalette, Anim_Plasma -- and Anim_Nereid is the FIRST, so the cut
 * is two ways with the whole `.rodata` staying with the remainder.  A `.global`
 * emits no bytes: export both, run `make compare` BEFORE the split so the two
 * changes stay separable, then split.  NOTE that Anim_Froth
 * (src/rom_c9000/rom_d2d98_b.c (LANDED; was src/non_matching/rom_c9000/Anim_Froth.c), 4 of 529, THE CLOSEST NON-MATCHING
 * FUNCTION IN THE TREE) is the SECOND function in this same file and will need
 * `.global .Lee1c4` for its own cut -- export all three at once if both are
 * going to land, and coordinate the stem suffixes.
 *
 * SHIMS: `python3 tools/shimcount.py` prints the filename and nothing else --
 * ZERO register pins, zero `.equ` shims, zero `"+r"` barriers.  PIN-FREE.  No
 * fakematch.txt row, no per-file Makefile flag override.
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN ORDER, WITH FIGURES
 * ================================================================
 *
 * First candidate straight off the disassembly: 54.9% aligned, size +4, count
 * +2, relocation sequence already exact.
 *
 *   1. 54.9 -> 55.4 AND THE FRAME FROM WRONG TO EXACT.  THE SIX FRAME-LOOP
 *      SCALARS ARE BLOCK-SCOPED, NOT FUNCTION-LEVEL, AND THEIR ORDER IS
 *      py, px, vy, vx, yo, fo.  Same reading as Anim_Ragnarok in this batch:
 *      reload assigns spill slots in ascending pseudo number and the frame grows
 *      down, so the ROM's 0x30/0x2c/0x28 for ctx/base/gfx followed by the `&fns`
 *      COMPILER TEMP at 0x24 and only then py at 0x20 says the six scalars are
 *      created AFTER the BuildDraw2DFuncs call.  A temp cannot outrank a
 *      function-level declaration.  Opening a block right after that call and
 *      declaring them inside it, in that order, puts all eleven slots on the
 *      ROM's offsets.  Also dropped in this step: the `DrawFn *pf = fns` local,
 *      which is inert (the ROM's store-then-reload of the fns address at
 *      sp+0x24 is reload's, not a source pointer).
 *
 *   2. 55.4 -> 56.6 AND COUNT +2 -> +1.  THE TWO `t = -1` CLEARING LOOPS WALK AN
 *      `int *`, NOT A `Part *`.  The tell is the STORE OFFSET: the ROM writes
 *      `str r2, [r3, #0]` with the base at `base + 0x7098` and at the symbol
 *      `ewram_2010018`, where a `Part *` with `q->t = -1` gives
 *      `str r3, [r5, #0x18]` and a base 0x18 lower.  0x7098 is 0x7080 + 0x18 and
 *      0x2010018 is gBuffer + 0x18, so BOTH addresses are already the `t` field
 *      of element zero.  Written `int *tp = (int *)(base + 0x7098); *tp = -1;
 *      tp += 7;` and `tp = &ewram_2010018;` the offsets and the pointer stride
 *      come out the ROM's.  This is why the tree carries both
 *      `extern Part ewram_2010018[];` and `extern int ewram_2010018;` -- this
 *      function needs the scalar spelling, and the array spelling is a
 *      +0x18-offset program.
 *
 * ALSO APPLIED ON THE FIRST CANDIDATE, transferred from Anim_Ragnarok in the
 * same batch and correct here without measurement of the alternative:
 *   - MUL OPERAND ORDER `sin(a) * r`, not `r * sin(a)`: Thumb `mul` is
 *     destructive and the SECOND source operand is the one that lands in the
 *     destination, so the ROM's `mov r3, r6 / mul r3, r0` (named multiplier in
 *     the destination, call result as the source) needs the call FIRST in the
 *     expression.  Four sites.
 *   - NO int CARRIER FOR REG_BLDALPHA: plain `REG_BLDALPHA = 0x1010;` gives the
 *     ROM's `ldr r2,=REG_BLDALPHA / ldr r3,=0x1010 / strh r3,[r2]` by itself,
 *     because gcc has no PC-relative `ldrh` and pools the constant in SImode
 *     unasked.  See the same finding, and the cross-function contradiction it
 *     raises with BaseAnim_Attack, in PARK_Anim_Ragnarok.c step 6.
 *   - 0x7824 IS A NAMED LOCAL, because the ROM DERIVES the sprite-array offset
 *     from it: `sub r7, #0x4c` after `ldr r7,=0x7824` gives 0x77d8.  gcc-2.96
 *     never chains plain CONST_INTs, so a ROM deriving one constant from another
 *     held in a register identifies a named base.  `off = 0x7824;
 *     *(int *)(base + off) = 1; ... (void **)(base + off - 0x4c)`.
 *   - THE BARE LITERALS STAY BARE: 0x3f across the two particle-seeding loops,
 *     0x22 across the two draw calls in the `.Lee1ac` loop, and 0x20 as both the
 *     width and the height in the five-sprite loop are each HOISTED INTO A
 *     CALLEE-SAVED REGISTER by the ROM.  Naming them would place them; leaving
 *     them bare lets gcc hoist them, which is the Anim_Hail rule.
 *
 * ================================================================
 * WHAT IS STILL OPEN, AND WHAT EACH RESIDUE IS
 * ================================================================
 *
 * FIRST, THE ONE EXTRA INSTRUCTION -- and it is a cse.c artifact, not a shape
 * error.  The ROM materialises the `-1` for the first clearing loop by
 * DECREMENTING A LIVE ZERO: `mov r2, #0` serves `yo = 0` and `i = 0`, and then
 * `sub r2, #1` is the whole constant, one instruction.  This candidate emits
 * `movs r3, #1 / negs r3, r3`, two.  The prologue store order is already the
 * ROM's (py, vx, vy, yo, px, then i) so the zero IS live at that point, and the
 * SECOND clearing loop -- where the ROM itself uses `movs r1,#1 / negs r1,r1` --
 * proves the ROM's compiler does not always take the cheap form.  So this is
 * `cse_insn` choosing between two equal-value expressions for one CONST_INT in
 * one basic block, which is not addressable from source without adding a
 * variable the ROM does not have.  It accounts for the whole +1 count and 2 of
 * the +4 bytes; the other 2 bytes are the pool alignment `.short` that follows.
 *
 * SECOND, ~10 encodings of RELOAD DECIDING WHETHER TO REUSE A LIVE SPILL.  Three
 * places where the ROM reloads a spilled value ONCE and covers two uses while
 * this candidate reloads twice:
 *   - `base` across `*(int *)(base + (0xef << 7)) = 2` and
 *     `*(int *)(base + 0x7784) = 0x4b` (ROM: one `ldr r7,[sp,#0x2c]`, two adds).
 *   - `vx` across `px += vx` and `vx = vx * 0x3a / 0x40` (ROM loads it twice
 *     too, but stores px BEFORE the second load; we defer the store).
 *   - the `&fns` address, stored and reloaded in the ROM, copied here.
 * All three are two instructions either way in isolation; what differs is the
 * ORDER, and the order follows from which pseudo reload found free.  Not a
 * source-level question at this candidate's pressure -- retry after the count
 * closes.
 *
 * THIRD, a whole-function LOW-REGISTER ROTATION.  The residue that dominates the
 * hunk list is r7/r5, r0/r1, r2/r3 swapped in long runs with the instruction
 * sequence otherwise identical -- e.g. the ROM's `ldr r7,=0x7828 / adds r3,r2,r7`
 * against our `ldr r5,=0x7828 / adds r3,r2,r5`.  Per batch 300's rule a
 * whole-function register rotation is ONE MISSING QUANTITY, not an ordering
 * problem, and the quantity to look for is an allocno this candidate does not
 * have or has with the wrong live length.  The frame and all eleven spill slots
 * already agree, so it is not a spill-set difference; it is the order
 * `allocno_compare` put the surviving low registers in, whose only source-level
 * inputs are n_refs, live_length and declaration order.  DECLARATION ORDER OF
 * THE UNSPILLED LOCALS IS NOW MEASURED AND IT IS INERT: moving i, frame and off
 * into the inner block after the six scalars, and moving them ahead of
 * ctx/base/gfx, are both BYTE-IDENTICAL to this file (619 of 707, 56.6%, same
 * 121 hunks).  An inert spelling is UNTESTED as a hypothesis, not disproved --
 * but it does narrow the axis: these three are never candidates in
 * allocno_compare's ranking because they are not spilled and their declaration
 * position does not reach it.  What is genuinely untried is permuting the SIX
 * SPILLED scalars away from the ROM's slot order, which would trade the exact
 * frame map for a different low-register ranking; that is a real experiment and
 * it is the next thing to run.
 *
 * WHAT IS ALREADY RIGHT AND READ OFF THE REFERENCE, so do not re-derive it:
 *   - THE TABLE POINTER IS `iwram_3001ef0`, NOT `iwram_3001eec`.  The ROM loads
 *     `iwram_3001ef0` and reaches base with `sub r2, r3, #4` -- so ctx is
 *     `iwram_3001ef0[0]`, gfx is `iwram_3001ef0[1]` and base is
 *     `*(void **)((char *)&iwram_3001ef0 - 4)`.  Writing `iwram_3001eec` and
 *     indexing forward (the Anim_Froth idiom in the next function of the same
 *     file) pools the wrong symbol.  This is the rom_ccebc.c spelling.
 *   - the file loads go through a `CopyFn copy` local and `_call_via_r3`, with
 *     `d0 = 0xa0; d0 <<= 19;` kept as a SEPARATE STATEMENT -- the Anim_Froth
 *     idiom, and the veneer is r3 here because the ROM does `ldr r3,=Func_8001af8`.
 *   - `vx = vx * 0x3a / 0x40` and `vy = vy * 0x38 / 0x40`: the
 *     `cmp #0 / bge / add #0x3f / asr #6` quartet is gcc's SIGNED divide by 64,
 *     so the divisor must be written as a division and not as a shift.  Same for
 *     the two `* 0x3e / 0x40` in the particle loop.  0x38 comes out as
 *     `lsl #3 / sub / lsl #3` and 0x3e as `lsl #5 / sub / lsl #1`; both are
 *     gcc's own synth_mult and must not be hand-distributed.
 *   - `if ((gKeyRepeat & 3) != 0 && frame > 0x20 && frame <= 0x61) frame = 0x62;`
 *     as a single `&&` chain -- three `cmp`/branch pairs falling through to one
 *     assignment, which is the `while (A && B)` reading applied to an `if`.
 *   - `fo = frame - 0x20` is a REAL LOCAL at sp+0x0c, computed once and tested
 *     TWICE as `(unsigned)fo <= 0x2f` -- the ROM does `sub r1,#0x20 /
 *     str r1,[sp,#0xc] / cmp r1,#0x2f / bhi` and then reloads it later for the
 *     second test.  Recomputing `frame - 0x20` at the second site costs a
 *     subtract the ROM does not have.
 *   - the two particle-seeding loops are DELIBERATELY NOT IDENTICAL: the second
 *     carries an `n` counter and `break`s at 0x10, and the ROM stores `t` BEFORE
 *     `vy` there and AFTER it in the first.  Per the cross-jumping hazard, do
 *     not tidy the two bodies to look alike -- the ROM emits both in full.
 *   - the draw-function selector is a BYTE OFFSET, not an index: the ROM has
 *     `mov r6,#4 / cmp r3,#0 / bgt / mov r6,#0` and then `ldr r4,[r6,r0]` with
 *     r0 = &fns, so there is no `lsl #2`.  Written `k = 4; if (vy <= 0) k = 0;`
 *     and `(*(DrawFn *)((char *)fns + k))(...)`.  `fns[vy > 0]` would compute a
 *     boolean and shift it.
 *   - the five `n` clamps (`frame > 0x44 && n <= 5` ... `frame > 0x4c`) are five
 *     separate `if`s with a `.pool_aligned` dropped between the second and the
 *     third by gcc's minipool; that pool is not a residue.
 *   - `Func_80d6888((*slot)->ids[i], 7, 5, -1, m)` passes the VARIABLE m -- known
 *     to be 0 at that point -- as the stacked argument, because the ROM's
 *     `str r6,[sp]` reuses the register the `m == 0` compare left.  Writing 0
 *     invites a cross-jump with the `m == 6` arm below it.
 *   - the region-4 loop is a real `while` (guard on `(*slot)->f14` plus a bottom
 *     test re-reading it), not a do-while.
 *   - the trailing sprite loop is `_DeleteSprite(*sp++)` -- the ROM's
 *     `ldmia r5!, {r0}` is the post-increment read, and `ldmia rX!` on its own
 *     does NOT prove a pointer walk, but combined with the `add r5, r3, r7`
 *     preheader and the `cmp r1,#0xc` bottom test it does here.
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

extern u8 *iwram_3001ef0;
extern Part gBuffer[];
extern int ewram_2010018;
extern int gKeyRepeat;
extern unsigned short Data_ede48[];
extern unsigned char Lee1ac[] __asm__(".Lee1ac");
extern int Lee1b4[] __asm__(".Lee1b4");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int DecompressLZ(void *src, void *dst);
extern void BuildDraw2DFuncs(int a, void **fns);
extern int *_GetBattleActor(int id);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80d6750(void *st);
extern void CreateSummonSprite(int a, int b, int c);
extern void Func_80e6d3c(int a, int x, int y);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Anim_Unsummon(int a, int x, int y);
extern void _DeleteSprite(void *sprite);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Nereid(void *context)
{
    DrawFn fns[2];
    void *ctx;
    unsigned char *base;
    unsigned char *gfx;
    int i;
    int frame;
    int off;

    ctx = (void *)iwram_3001ef0;
    base = *(unsigned char **)((unsigned char *)&iwram_3001ef0 - 4);
    gfx = *(unsigned char **)((unsigned char *)&iwram_3001ef0 + 4);
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BLDALPHA = 0x1010;
    BuildDraw2DFuncs(0, (void **)fns);
    {
    int py;
    int px;
    int vy;
    int vx;
    int yo;
    int fo;
    {
        CopyFn copy;
        unsigned char *p;
        int d0;
        p = GetFile(FILE_6e);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, p, 0x80);
        p += 0x80;
        DecompressLZ(p, base);
        p = GetFile(FILE_85);
        p += 0x80;
        DecompressLZ(p, base + 0x6e4);
        DecompressLZ(GetFile(FILE_73), gfx);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    py = 0xb0 << 15;
    vx = 0xfff00000;
    vy = 0xfffc0000;
    yo = 0;
    px = 0x80 << 17;
    {
        int *tp = (int *)(base + 0x7098);
        Part *q;
        i = 0;
        do {
            *tp = -1;
            i++;
            tp += 7;
        } while (i != 0x40);
        q = (Part *)(base + 0x7320);
        i = 0;
        do {
            q->x = Random() & 0x7f;
            q->y = (Random() & 7) + 0x38;
            q->t = -(Random() & 0xf);
            i++;
            q++;
        } while (i != 0x10);
        tp = &ewram_2010018;
        i = 0;
        do {
            *tp = -1;
            i++;
            tp += 7;
        } while (i != (0x80 << 3));
    }
    Func_80d6750(*(State **)(base + 0x7828));
    WaitFrames(1);
    CreateSummonSprite(0xc, 0xbe << 1, 2);
    frame = 0;
    do {
        if ((gKeyRepeat & 3) != 0 && frame > 0x20 && frame <= 0x61) {
            frame = 0x62;
        }
        if (frame == 0x78) {
            _Func_80bd7dc(0x86);
        }
        if (frame <= 0xf) {
            yo += 2;
        }
        if (frame <= 0x63) {
            px += vx;
            py += vy;
            vx = vx * 0x3a / 0x40;
            vy = vy * 0x38 / 0x40;
            if (px <= 0x77ffff) {
                vx += 0x80 << 8;
            }
        }
        Func_80e6d3c(1, px, py);
        if (frame == 0x1c) {
            Part *g = gBuffer;
            i = 0;
            do {
                if (g->t == -1) {
                    int r = Random() & 0x3f;
                    int a = Random() & 0xffff;
                    g->x = ((sin(a) * r) >> 3) + (0x80 << 14);
                    g->y = ((cos(a) * r) >> 2) + (0xc0 << 15);
                    g->vx = ((Random() & 0x3f) - 0x20) << 14;
                    g->vy = (-(Random() & 0x3f) - 8) << 13;
                    g->t = 0;
                }
                i++;
                g++;
            } while (i != (0x80 << 1));
        }
        fo = frame - 0x20;
        if ((unsigned int)fo <= 0x2f) {
            Part *g = gBuffer;
            int n = 0;
            i = 0;
            do {
                if (g->t == -1) {
                    int r = Random() & 0x3f;
                    int a = Random() & 0xffff;
                    g->x = ((sin(a) * r) >> 3) + (0x80 << 14);
                    g->y = ((cos(a) * r) >> 2) + (0xc0 << 15);
                    g->vx = ((Random() & 0x3f) - 0x20) << 14;
                    g->t = 0;
                    g->vy = (-(Random() & 0x3f) - 8) << 13;
                    n++;
                    if (n == 0x10) {
                        break;
                    }
                }
                i++;
                g++;
            } while (i != (0x80 << 3));
        }
        if (frame == 0) {
            _PlaySound(0xa4);
        }
        if (frame == 0x20) {
            _PlaySound(0x91);
        }
        if (frame == 0x50) {
            _PlaySound(0x90);
        }
        if ((unsigned int)fo <= 0x2f) {
            unsigned char *src = base + 0x6e4;
            unsigned char *tbl = Lee1ac;
            int u = frame * 16 - 0x100;
            i = 0;
            do {
                int m = u % 0x68;
                fns[0](ctx, src, tbl[0] - 0x11, tbl[1] - m - 0x68, 0x22, 0x68);
                fns[0](ctx, src, tbl[0] - 0x11, tbl[1] - m, 0x22, m);
                i++;
                tbl += 2;
                u += 0x19;
            } while (i != 3);
        }
        if (frame <= 0x5f) {
            i = 0;
            do {
                fns[0](ctx, base, i * 32 + (frame / 4 & 0x1f) - 0x20,
                       0x78 - yo, 0x20, 0x20);
                i++;
            } while (i != 5);
        }
        {
            Part *g = gBuffer;
            i = 0;
            do {
                if (g->t >= 0) {
                    int vyy = g->vy;
                    int n = i % 3 + 2;
                    int k;
                    if (vyy > 0) {
                        n += 2;
                    }
                    if (frame > 0x44 && n <= 5) {
                        n = 6;
                    }
                    if (frame > 0x46 && n <= 6) {
                        n = 7;
                    }
                    if (frame > 0x48 && n <= 7) {
                        n = 8;
                    }
                    if (frame > 0x4a && n <= 8) {
                        n = 9;
                    }
                    if (frame > 0x4c) {
                        n = 0xa;
                    }
                    k = 4;
                    if (vyy <= 0) {
                        k = 0;
                    }
                    (*(DrawFn *)((char *)fns + k))(
                        ctx, gfx + Data_ede48[n - 1],
                        *(short *)((char *)g + 2) - n / 2,
                        *(short *)((char *)g + 6) - n, n, n * 2);
                    g->x = g->x + g->vx;
                    vyy = g->vy;
                    g->y = g->y + vyy;
                    if (frame > 0x50) {
                        g->vy = vyy + 0xffff8000;
                    } else {
                        g->vy = vyy + Lee1b4[i & 3];
                    }
                    g->vx = g->vx * 0x3e / 0x40;
                    g->vy = g->vy * 0x3e / 0x40;
                    g->t = g->t + 1;
                    if (g->vy > 0 && *(short *)((char *)g + 6) > 0x68) {
                        g->t = -1;
                    }
                }
                i++;
                g++;
            } while (i != (0x80 << 3));
        }
        if (frame <= 0x4f) {
            State **slot = (State **)(base + 0x7828);
            i = 0;
            if ((*slot)->f14 != 0) {
                do {
                    if (frame > 0x1d) {
                        int m = frame % 0xc;
                        if (m == 0) {
                            int *ap = (int *)*_GetBattleActor((*slot)->ids[i]);
                            Func_80d6888((*slot)->ids[i], 7, 5, -1, m);
                            ap[0x28 / 4] = 0x90 << 11;
                            ap[0x48 / 4] = 0xab85;
                        }
                        if (m == 6) {
                            Func_80d6888((*slot)->ids[i], 0, 5, -1, 0);
                        }
                    }
                    i++;
                } while (i != (*slot)->f14);
            }
        }
        off = 0x7824;
        *(int *)(base + off) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x7c);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    Anim_Unsummon(1, px, py);
    {
        void **sp = (void **)(base + off - 0x4c);
        i = 0;
        do {
            _DeleteSprite(*sp++);
            i++;
        } while (i != 0xc);
    }
    AnimEnd();
    }
}
