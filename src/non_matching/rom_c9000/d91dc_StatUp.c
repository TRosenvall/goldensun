/* BaseAnim_StatUp -- 0x080d91dc, asm/rom_c9000/rom_d9194_c_c_c_c_c_c.s line 12,
 * 962 ROM instructions.
 * NON-MATCHING, 844 of 1007 encodings differ.
 *
 * SIZE AND COUNT ARE BOTH INEXACT, so 844 is NOT a distance: 2260 bytes against
 * the ROM's 2268 (-8) and 1003 encodings against 1007 (-4) -- we are SHORT by
 * four instructions.  tools/aligncmp.py reads 650 aligned-equal of 1007 (64.5%),
 * 439 differing/ins/del in 174 hunks, and 64.5% is the figure to beat.
 *
 * *** THE FRAME IS EXACT.  `sub sp, #0x84` on both sides, 17 spilled scalars on
 * both sides, two out-params and four vec3_t aggregates accounted for
 * word-for-word: 8 bytes outgoing args + 17*4 scalars + 2*4 out-params +
 * 4*12 aggregates = 132 = 0x84.  AND THE CAVEAT THE BRIEF DEMANDS: this is NOT
 * rung 3 of the ladder passing, because size and count are still inexact.  Two
 * quantities land differently (blocker A), and the frame total agrees anyway --
 * so read the frame here as evidence about the QUANTITY SET, which it settles,
 * and not about the allocation, which it does not. ***
 *
 * *** THE RELOCATION SEQUENCE IS EXACT IN LENGTH AND ALMOST EXACT IN ORDER:
 * 109 relocations on both sides, every call and every symbol in the ROM's
 * order, with exactly two classes of difference -- `_call_via_r7`/`_call_via_r9`
 * where the ROM has `_call_via_r8`/`_call_via_r4`/`_call_via_r10` (blocker A),
 * and ONE transposed pool pair (iwram_3001f08 before iwram_3001f0c, blocker B).
 * That was true of the FIRST candidate, before any lever. ***
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/d91dc_StatUp.c \
 *     asm/rom_c9000/rom_d9194_c_c_c_c_c_c.s --func BaseAnim_StatUp
 * FINAL INSTALLED PATH: src/non_matching/rom_c9000/d91dc_StatUp.c
 *
 * SHIMS: ZERO.  tools/shimcount.py prints the filename and nothing else.
 * PIN-FREE: no pin was installed, so none is owed a measurement.
 *
 * ================================================================
 * THE SPLIT SHAPE -- THERE IS NONE
 * ================================================================
 * `grep -c thumb_func_start asm/rom_c9000/rom_d9194_c_c_c_c_c_c.s` prints 1:
 * BaseAnim_StatUp is ALONE in its .s.  tools/datacheck.py prints NOTHING for
 * the file -- no data section.  NO SPLIT, NO new `.global`, NO linker script
 * edit.  Data_ede5c is already an `.incdata` export; everything else is a call
 * or a symbol decls.h already declares.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID, WITH FIGURES
 * ================================================================
 * Baseline v1 -- the Heal skeleton re-read against THIS function's asm, first
 * candidate: size 2256 (-12), count 1001 (-6), objcmp 854, 638 aligned (63.4%),
 * 178 hunks, FRAME ALREADY EXACT AT 0x84, 109/109 relocations in order.  The
 * frame landing exact on candidate v1 is what makes this the better understood
 * of the two functions in this brief even though its aligned figure is lower.
 *
 * (1) THE `ldmia` WALKING-POINTER PROLOGUE.  `g = iwram_3001eec; pp = g;
 *     base = *pp++; ctx = *pp;` -- note only TWO words here, no `g[2]`, where
 *     BaseAnim_Heal takes three.  Byte-exact.  Confirms the prologue as the one
 *     genuinely family-wide idiom.
 *
 * (2) THE SPILL-SLOT MAP READ AS THE DECLARATION LIST, DESCENDING -- and it
 *     landed the frame on the first try.  The ROM's scalars run
 *         0x48 variant (a PARAMETER -- gcc puts parameter spills at the top,
 *              and the ROM spills it in the very FIRST instruction after
 *              `sub sp`, so unlike Heal this function does NOT keep the variant
 *              in a high register)
 *         0x44 base, 0x40 ctx, 0x3c fb, 0x38 fa, 0x34 b, 0x30 dx, 0x2c flags,
 *         0x28 look, 0x24 (gcc's OWN hoist -- see below), 0x20 look2,
 *         0x1c tvp, 0x18 pvp, 0x14 zvp, 0x10 lim, 0x0c io, 0x08 (a spilled
 *         constant)
 *     Declaring in that order gives `sub sp, #0x84` exactly.
 *     *** ONE SLOT IN THAT LIST IS NOT A SOURCE VARIABLE AND MISREADING IT
 *     WOULD HAVE COST THE FRAME: sp+0x24 holds the ADDRESS of iwram_3001e80,
 *     written once before the outer loop (`ldr r5,=iwram_3001e80 /
 *     str r5,[sp,#0x24]`) and dereferenced inside it.  That is loop.c moving a
 *     loop-invariant SYMBOL_REF, not a declared local -- and the proof is that
 *     the SAME slot is then used to reach a DIFFERENT global:
 *     `ldr r3,[sp,#0x24] / add r3,#0x88 / ldr r3,[r3]` is iwram_3001f08,
 *     because 0x3001e80 + 0x88 == 0x3001f08 and cse commoned the address.  A
 *     plain `look = iwram_3001e80;` plus a plain `iwram_3001f08` reproduces
 *     both.  So the brief's rule that a slot says a quantity EXISTED needs the
 *     rider that gcc's own loop-invariant hoists OCCUPY SLOTS TOO -- count
 *     them in the frame, do not declare them in the source. ***
 *
 * (3) grep 3 OF THE FRAME TRIAD EARNED ITS KEEP TWICE HERE.  `str r4, [sp, #8]`
 *     appears at two draw sites and reads exactly like a SEVENTH argument, which
 *     would have made the outgoing-args area 12 bytes and the frame wrong.  Both
 *     are LOADED BACK (`ldr r4,[sp,#8]` before the paired second call), so sp+8
 *     is the lowest SPILL slot and every call here takes six arguments.  Had
 *     either been read as staging the frame would have come out 0x88 and the
 *     whole slot map would have shifted.
 *
 * (4) UNIFY THE SEED COUNTER AND THE PARTICLE COUNTER INTO ONE VARIABLE.
 *     The ROM uses r10 for the gBuffer seed loop and r10 again for the inner
 *     particle loop -- disjoint ranges, one register.  Measured both ways:
 *         split (`i` and `j`) : size 2256 (-12), count 1001 (-6), objcmp 854, 638 aligned (63.4%)
 *         unified (one `i`)   : size 2260  (-8), count 1003 (-4), objcmp 844, 650 aligned (64.5%)
 *     *** ALL FOUR AXES IMPROVE AT ONCE and the frame stays exact, which is the
 *     cleanest possible result for a counter-partition probe.  Paired with the
 *     same change on BaseAnim_Heal (+5.5 aligned there) this is now TWO
 *     independent confirmations in one batch that the seed counter and the
 *     particle counter are ONE source variable in this family -- which is the
 *     first thing in this family to transfer other than the prologue. ***
 *
 * ================================================================
 * MEASURED INERT (untested, not disproved)
 * ================================================================
 *   - Swapping the two Anim_Djinni out-params (`int p50; int p4c;` ->
 *     `int p4c; int p50;`): BYTE-IDENTICAL (2256/1001/854/63.4%).
 *   - Lifting the table-derived source pointer into its own declaration above
 *     the draw call (the lever that was worth +7.1 aligned and an exact
 *     relocation sequence on BaseAnim_Heal): BYTE-IDENTICAL here.
 *     *** AND THAT INERTNESS NARROWS THE LEVER USEFULLY: Heal's draw site reads
 *     TWO tables (Data_ede84 for the offset, Data_ede96 for the width) and the
 *     declaration order is what orders their pool words.  This site reads ONE
 *     (Data_ede5c), so there is no pair to order and nothing for the lever to
 *     purchase.  The precondition is TWO TABLES AT ONE SITE. ***
 * MEASURED WORSE:
 *   - Reversing the four vec3_t declarations (the brief's aggregate-order
 *     lever): 636 aligned (63.2%) against 638, same size and count, frame still
 *     exact.  So on this function aggregate declaration order is already right
 *     in the natural order and reversing costs two encodings -- a third
 *     data point, after Attack (inert) and ParticleCloud (decisive), that the
 *     direction is genuinely per-function and must be measured both ways.
 *
 * ================================================================
 * THE BLOCKER, BY PASS
 * ================================================================
 *
 * (A) *** THE TWO DRAW-FUNCTION POINTERS KEEP HARD REGISTERS WHERE THE ROM
 *     SPILLS THEM -- global-alloc, and it is the whole residue. ***  The ROM
 *     puts `fa` at sp+0x38 and `fb` at sp+0x3c and RELOADS each one at its call
 *     site, so the calls go through whatever register the reload picked
 *     (`_call_via_r8`, `_call_via_r4`, `_call_via_r10` at different sites).  We
 *     keep both live in r7 and r9 across the whole loop body and call
 *     `_call_via_r7` / `_call_via_r9` everywhere.  The arithmetic: the ROM has
 *     SIX quantities above `dx` in the slot map (variant, base, ctx, fb, fa, b)
 *     and we have FOUR, because fa and fb took registers instead -- and the
 *     frame total still matches because two other quantities spill in their
 *     place.  That is exactly the cancellation the brief's rung 3 warns about,
 *     which is why the frame is reported above with its caveat.
 *     What rules out the alternatives, measured rather than assumed: it is not
 *     declaration order (reversing the aggregates measured worse, and the
 *     scalar list is already variable-for-variable on the ROM's above the two
 *     differences); it is not the quantity SET (the frame total and the scalar
 *     count both agree, so no quantity is missing or extra); and it is not
 *     scheduling, because sched1 does not run in this build.  The lever that
 *     would reach it is "lower a competitor's priority" against fa/fb, and the
 *     obstacle is that they are already function-level declarations assigned
 *     inside the loop -- the one shape that RAISES priority.  Making them
 *     block-scoped per draw arm would raise it further; the untried direction is
 *     to find a disjoint donor to share a pseudo with so that `live_length`
 *     becomes the SUM of the ranges, which is the brief's one-variable-per-region
 *     lever used deliberately to LOSE a register.  That is the next probe and it
 *     was not reached in this batch.
 *
 * (B) ONE TRANSPOSED POOL PAIR: ours dumps iwram_3001f08 before iwram_3001f0c
 *     in the first draw block, the ROM the other way.  The mechanism is already
 *     identified and it is downstream of (2): in the ROM, iwram_3001f08 is
 *     reached off the HOISTED address of iwram_3001e80 (+0x88) and so costs NO
 *     pool word of its own, which leaves iwram_3001f0c holding the first word.
 *     We emit a pool word for iwram_3001f08 because our `look = iwram_3001e80;`
 *     did not leave the address in a place cse could reuse.  This is a cse /
 *     loop-invariant-motion interaction, not a spelling of the global, and it is
 *     the likely home of the missing four instructions.
 *
 * (C) NOT A RESIDUE, recorded so it is not chased: `ldr r3, <pool> @ 0xcc` for
 *     `REG_BG2PA = 0xcc`.  REG_BG2PA is `vu16`, gcc emits `ldrh rX,<pool>` for a
 *     HImode volatile store, and gas assembles Thumb `ldrh <pool-label>` to the
 *     same halfword as `ldr`.  Reproduces byte-exactly from the plain
 *     `REG_BG2PA = 0xcc;` -- the sixth recorded instance, and the sibling
 *     BaseAnim_Heal has the same shape with 0x100.
 */
#include "decls.h"

extern void *iwram_3001e80;
extern void *iwram_3001f08;
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void MatrixTranslatev(vec3_t *v);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);
extern int  Func_8000948(int v);
extern unsigned short Data_ede5c[];

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef int (*MagFn)(int v);

void BaseAnim_StatUp(void *context, int variant)
{
    vec3_t vec;
    vec3_t zv;
    vec3_t pv;
    vec3_t tv;
    int p50;
    int p4c;
    unsigned char *base;
    void *ctx;
    DrawFn fb;
    DrawFn fa;
    int b;
    int dx;
    int flags;
    void *look;
    void *look2;
    vec3_t *tvp;
    vec3_t *pvp;
    vec3_t *zvp;
    int lim;
    int io;
    int **g;
    int **pp;
    State **slot;
    int i;
    int frame;
    int m;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    if ((*slot)->f1c == 1) {
        if (variant == 3)
            Anim_Djinni(context, 0, (*slot)->f4 ^ 1, 1, &p50, &p4c);
        else if (variant == 2 || variant == 4)
            Anim_Djinni(context, 3, (*slot)->f4 ^ 1, 1, &p50, &p4c);
        else
            Anim_Djinni(context, 2, (*slot)->f4 ^ 1, 1, &p50, &p4c);
        p50 = p50 * 4 / 5;
    }
    REG_BG2PA = 0xcc;
    LoadVFXFile(FILE_76, base, 0, 0);
    LoadVFXFile(FILE_b7, base + 0x60e, 1, 1);
    if (variant == 3 || variant == 5) {
        int id;
        void *s;
        int d0;
        CopyFn copy;

        LoadVFXFile(FILE_b0, base + 0x2b8e, 1, 1);
        if (variant == 3)
            id = FILE_93;
        else
            id = FILE_8d;
        s = GetFile(id);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    } else if (variant == 4) {
        LoadVFXFile(FILE_a5, base + 0x2b8e, 1, 1);
    } else {
        int id;
        void *s;
        int d0;
        CopyFn copy;

        if (variant == 0)
            LoadVFXFile(FILE_9c, base + 0x2b8e, 1, 0);
        else
            LoadVFXFile(FILE_9b, base + 0x2b8e, 1, 0);
        if (variant == 0)
            id = FILE_8d;
        else if (variant == 2 || variant == 4)
            id = FILE_8f;
        else if (variant == 1)
            id = FILE_8d;
        else
            id = FILE_bb;
        s = GetFile(id);
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, s, 0x80);
    }
    if (variant == 3)
        LoadVFXFile(FILE_93, base + 0x65c0, 1, 0);
    else if (variant == 2 || variant == 4)
        LoadVFXFile(FILE_8f, base + 0x65c0, 1, 0);
    else
        LoadVFXFile(FILE_8d, base + 0x65c0, 1, 0);
    {
        Part *p = gBuffer;
        i = 0;
        do {
            p->x = ((unsigned int)Random() % 0xc8 - 0x64) << 14;
            p->y = ((unsigned int)Random() % 0xc8 - 0x64) << 15;
            p->z = ((unsigned int)Random() % 0xc8 - 0x64) << 14;
            p->t = 0;
            i++;
            p++;
        } while (i != 0x200);
    }
    if ((*(State **)(base + 0x7828))->f14 == 1) {
        GetBattleActorPos2((*(State **)(base + 0x7828))->ids[0], &vec);
        dx = -vec.x * 4 / 5 + 0x40;
    } else {
        dx = -0x40;
        if ((*(State **)(base + 0x7828))->f4 != 1)
            dx = 0;
    }
    REG_BG2X = dx << 8;
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    {
        int arg;
        arg = 0x90;
        arg <<= 3;
        StartTask(Task_BlitAnim, arg);
    }
    flags = 7;
    if ((*(State **)(base + 0x7828))->f4 != 1)
        flags = 3;
    _PlaySound(0x8e);
    frame = 0;
    if (((*(State **)(base + 0x7828))->f14 << 3) + 0x6c != 0) {
        do {
            look = iwram_3001e80;
            if (frame == 0x50)
                _Func_80bd7dc(0);
            if ((*(State **)(base + 0x7828))->f1c == 1) {
                int a;
                int f;
                int x;
                int y;
                void *src;

                a = frame << 11;
                x = ((sin(a) * 20) >> 16) + p50 + dx - 0x14;
                f = flags ^ 4;
                y = ((cos(a) * 4) >> 16) + p4c;
                BuildDraw2DFuncEx(0x2e, 7, 7, f, 2);
                fa = (DrawFn)iwram_3001f08;
                BuildDraw2DFuncEx(0x2f, 7, 7, f, 3);
                fb = (DrawFn)iwram_3001f0c;
                y = y - 0x18;
                if (frame > 0x20)
                    y = y - frame * 2 + 0x40;
                src = base + 0x65c0;
                fa(ctx, src, x, y, 0x28, 0x28);
                if (frame <= 3)
                    fb(ctx, src, x, y, 0x28, 0x28);
                gfree(0x2f);
                gfree(0x2e);
            }
            b = 0;
            if ((*(State **)(base + 0x7828))->f14 != 0) {
                look2 = (char *)look + 0xc;
                tvp = &tv;
                pvp = &pv;
                zvp = &zv;
                lim = 0;
                io = 0x24;
                m = frame;
                do {
                    State **sl = (State **)(base + 0x7828);
                    int *act;
                    int h;

                    act = (int *)*(int **)_GetBattleActor(
                            *(short *)((char *)*sl + io));
                    h = _Func_80b8530(*(short *)((char *)*sl + io)) * 2 / 3;
                    if (frame == lim + 0x50)
                        _PlaySound(0xd4);
                    InitMatrixStack();
                    MatrixSetLook(look, look2);
                    tvp->x = act[2];
                    tvp->y = h;
                    tvp->z = act[4];
                    MatrixTranslatev(tvp);
                    if (frame == lim + 0x30)
                        Func_80d6888(*(short *)((char *)*sl + io), 7, -1, b, 0x10);
                    if (frame > lim) {
                        Part *p;
                        BuildDraw2DFuncEx(0x2e, 7, 7, flags, 2);
                        fa = (DrawFn)iwram_3001f08;
                        BuildDraw2DFuncEx(0x2f, 7, 7, flags, 3);
                        fb = (DrawFn)iwram_3001f0c;
                        if (variant == 0) {
                            MatrixPitch(-frame << 10);
                        } else if (variant == 1) {
                            ;
                        } else if (variant == 2) {
                            MatrixYaw(frame << 10);
                        } else {
                            int r = frame << 10;
                            MatrixYaw(r);
                            MatrixRoll(r);
                        }
                        p = (Part *)((int *)gBuffer + b * 448);
                        i = 0;
                        do {
                            if (frame > lim + i) {
                                int d;
                                MagFn mag;
                                mag = Func_8000948;
                                d = mag((p->x >> 8) * (p->x >> 8)
                                        + (p->y >> 8) * (p->y >> 8)
                                        + (p->z >> 8) * (p->z >> 8)) >> 9;
                                if (d != 0) {
                                    int u;
                                    int t;

                                    Func_80e3944((vec3_t *)p, pvp);
                                    pvp->x = pvp->x * 4 / 5 + dx;
                                    if (pvp->z <= 0x139)
                                        pvp->z = 0x13a;
                                    if (pvp->z > 0x27a)
                                        pvp->z = 0x27a;
                                    t = pvp->z - 0x13a;
                                    if (t < 0)
                                        t = pvp->z - 0xfb;
                                    u = 6 - (t >> 6);
                                    fb(ctx, base + Data_ede5c[u - 1],
                                       pvp->x - u, pvp->y - u, u * 2, u * 2);
                                    p->x -= p->x / d;
                                    p->y -= p->y / d;
                                    p->z -= p->z / d;
                                }
                            }
                            i++;
                            p++;
                        } while (i != 0x20);
                        gfree(0x2f);
                        gfree(0x2e);
                    }
                    zvp->x = 0;
                    zvp->y = 0;
                    zvp->z = 0;
                    Func_80e3944(zvp, pvp);
                    pvp->x = pvp->x * 4 / 5 + dx;
                    if (frame >= lim + 0x34 && frame < lim + 0x4c) {
                        int u = (m - 0x34) / 4 % 6;
                        BuildDraw2DFuncEx(0x2e, 7, 7, flags, 2);
                        fa = (DrawFn)iwram_3001f08;
                        fa(ctx, base + u * 1600 + 0x60e,
                           pvp->x - 0x14, pvp->y - 0x14, 0x28, 0x28);
                        gfree(0x2e);
                    }
                    if (variant == 0) {
                        if (frame >= lim + 0x50 && frame < lim + 0x6c) {
                            int u = (m - 0x50) / 4 % 7;
                            BuildDraw2DFuncEx(0x2e, 7, 7, flags, 2);
                            fa = (DrawFn)iwram_3001f08;
                            fa(ctx, base + (u * 16 - u) * 64 + 0x2b8e,
                               pvp->x - 0xc, pvp->y - 0x14, 0x18, 0x28);
                            gfree(0x2e);
                        }
                    } else if (variant == 3 || variant == 5) {
                        if (frame >= lim + 0x50 && frame < lim + 0x68) {
                            int u = (m - 0x50) / 4 % 6;
                            BuildDraw2DFuncEx(0x2e, 7, 7, flags, 2);
                            fa = (DrawFn)iwram_3001f08;
                            fa(ctx, base + (u << 11) + 0x2b8e,
                               pvp->x - 0x10, pvp->y - 0x20, 0x20, 0x40);
                            gfree(0x2e);
                        }
                    } else if (variant == 4) {
                        if (frame >= lim + 0x50 && frame < lim + 0x68) {
                            int u = (m - 0x50) / 2 % 6;
                            unsigned char *src;
                            BuildDraw2DFuncEx(0x2e, 7, 7, flags, 2);
                            fa = (DrawFn)iwram_3001f08;
                            BuildDraw2DFuncEx(0x2f, 7, 7, flags | 8, 2);
                            fb = (DrawFn)iwram_3001f0c;
                            src = base + (u << 11) + 0x2b8e;
                            fa(ctx, src, pvp->x - 0x20, pvp->y - 0x18, 0x40, 0x20);
                            fb(ctx, src, pvp->x - 0x20, pvp->y + 8, 0x40, 0x20);
                            gfree(0x2f);
                            gfree(0x2e);
                        }
                    } else {
                        if (frame >= lim + 0x50 && frame < lim + 0x68) {
                            int u = (m - 0x50) / 4 % 6;
                            BuildDraw2DFuncEx(0x2e, 7, 7, flags, 3);
                            fa = (DrawFn)iwram_3001f08;
                            fa(ctx, base + u * 1600 + 0x2b8e,
                               pvp->x - 0x14, pvp->y - 0x14, 0x28, 0x28);
                            gfree(0x2e);
                        }
                    }
                    lim += 8;
                    io += 2;
                    b++;
                    m -= 8;
                } while (b != (*(State **)(base + 0x7828))->f14);
            }
            Func_80cd52c();
            *(int *)(base + 0x7824) = 1;
            WaitFrames(1);
            frame++;
        } while (frame != ((*(State **)(base + 0x7828))->f14 << 3) + 0x6c);
    }
    StopTask(Task_BlitAnim);
    AnimEnd();
}
