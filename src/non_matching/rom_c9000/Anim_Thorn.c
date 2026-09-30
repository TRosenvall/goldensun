/* Anim_Thorn -- 0x080dd9c0, 446 instructions.  PARKED.
 * NON-MATCHING, 348 of 476 encodings differ.  THAT FIGURE IS SATURATED: ours
 * is 466 encodings / 1036 bytes against the ROM's 476 / 1056, so the count is
 * NOT a distance.  The honest measure is tools/aligncmp.py, which masks
 * nothing but alignment: 296 of 476 encodings aligned-equal (62.2%), 198
 * differing/inserted/deleted in 73 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Thorn.c \
 *     asm/rom_c9000/rom_dd2ac_c_c_c.s --func Anim_Thorn
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_Thorn.c \
 *     asm/rom_c9000/rom_dd2ac_c_c_c.s Anim_Thorn
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT NEEDED.  tools/datacheck.py on
 * asm/rom_c9000/rom_dd2ac_c_c_c.s reports .rodata plus three functions
 * (Anim_Thorn, Anim_Bolt, Anim_Djinni) and twelve labels that are ALREADY
 * global (.Leeb96, .Leeb48, .Leeb4b, .Leeb4e, .Leeb54, .Leeb58, .Leeb5e,
 * .Leeb61, .Leeb71, .Leeb79, .Leeb80, .Leeb88) -- none of which is the set
 * Anim_Thorn needs.  Its split
 *   *** MUST EXPORT: .global .Leeba6 .global .Leebae .global .Leebb6
 *                   .global .Leebb9 .global .Leebc0 .global .Leebc8
 * Suffixes taken for this base are _b, _c_b, _c_c_a, _c_c_b, _c_c_c in asm/
 * and _b, _c_b, _c_c_b in src/, so the next free letters for the pieces of
 * rom_dd2ac_c_c_c are _a and _d onward.  NO SHIMS, NO PINS (shimcount.py is
 * clean): no volatile beyond io.h, no barrier, no flag override.
 *
 * ================================================================
 * THE SHAPE, and what is already right
 * ================================================================
 *
 * The reconstruction is structurally complete and the whole control-flow
 * skeleton aligns: prologue, the three register writes reached by ADDING to
 * one REG_BG2PA pointer (0x4000020 -> +0x30 REG_BLDCNT -> +2 REG_BLDALPHA),
 * the 0x400-entry `*q = -1` sweep over &ewram_2010018 in 0x1c strides, the
 * frame loop over `Leebb6[state->f18] * 8 + 0x38`, the j loop with its four
 * blit arms, the free-slot search over gBuffer, the actor loop and the final
 * 0x40-slot draw pass all line up instruction for instruction in aligncmp.
 *
 * LEVERS THAT PAID, in order:
 *   - `pp = g; base = *pp++; ctx = *pp;` for `ldmia r3!, {r1}`.
 *   - DECLARATION ORDER IS THE SPILL MAP, DESCENDING, and it is already
 *     correct: ctx, frame, f1, f0, nframes land at the ROM's 0x30, 0x2c, 0x28,
 *     0x24, 0x20 (ours sits 8 bytes lower only because two frame slots are
 *     missing -- see the residue).  NOTE `f1` IS DECLARED BEFORE `f0`; the ROM
 *     puts the 0x2f blitter at the HIGHER address.
 *   - a single `int one = 1;` shared by BOTH BuildDraw2DFuncEx stack arguments
 *     AND by `*(int *)(base + (0xef << 7)) = one`, which is the ROM's r5.
 *   - `f1 = (DrawFn)g[8];` loaded AFTER the two base stores, matching the
 *     ROM's `ldr r7,[r7,#0x20]` wedged between `str r5,[r3]` and `str r3,[r2]`.
 *   - `int kind = Leeba6[j];` as an **int**, not `unsigned char`: the ROM has
 *     `ldrb r2,[r3,r4] / mov r3,r2 / cmp r3,#1 / bhi`, and that redundant copy
 *     is the QImode-to-SImode conversion of an int-typed local.  An
 *     `unsigned char kind` with `(unsigned)kind <= 1` compares r2 directly and
 *     loses the `mov`.
 *   - BOTH `w` AND `h` COMPUTED BEFORE EITHER IS CLAMPED in the kind<=1 arm.
 *     The ROM emits `lsl r3,r6,#1 / add r3,r8 / lsl r0,r6,#4 / lsl r1,r3,#1`
 *     and only then the two `cmp/ble/mov` clamps; computing w, clamping w,
 *     computing h, clamping h interleaves them wrongly.
 *   - the `while (frame != nframes)` and `while (j != Leebb6[...])` forms, so
 *     duplicate_loop_exit_test manufactures the ROM's guard-then-preheader
 *     layout and the two invariants `nframes - 0x40` and `nframes - 0x10` are
 *     hoisted into their own frame slots as the ROM has them.
 *   - `*(State **)(base + 0x7828)` written out AT EVERY USE INSIDE the frame
 *     loop (the ROM re-derives it in the j loop and again in the actor loop)
 *     while the three prologue uses share one CSEd register -- no `st` local.
 *   - `int *q = &ewram_2010018; do { ii++; *q = -1; q = (int *)((char *)q +
 *     0x1c); } while (...)`, the d82b0_b.c idiom verbatim, including the
 *     counter bump BEFORE the store.
 *
 * ================================================================
 * THE RESIDUE -- ONE CAUSE, MEASURED FROM FOUR SIDES
 * ================================================================
 *
 * WE ARE 10 ENCODINGS AND 20 BYTES SHORT, AND ALL OF IT IS ONE loop.c
 * DECISION: WHICH `j * 8` GIV combine_givs ELECTS AS THE BASE.
 *
 * The ROM reduces TWO givs of multiplier 8 -- `8 + j*8` (frame slot 0xc) and
 * `0xc + j*8` (frame slot 0x14) -- and reduces NO bare `j*8`.  Its four uses
 * inside `if (frame > j*8+8)` then read the first giv directly (`ldr
 * r2,[sp,#0xc] / cmp r1,r2`) or with a small add (`adds r3,#1`, `adds r3,#3`),
 * and the actor-loop test reads the second giv, which reload keeps in r10 for
 * the whole of that nested loop.
 *
 * We reduce ONE giv, the bare `j*8` (add_val 0), and pay `adds r3,#8`,
 * `adds r3,#9`, `adds r3,#11`, `adds r3,#12` at the five uses.  That is one
 * instruction cheaper at every use, one whole biv update (ldr/add/str) cheaper
 * at the loop bottom, and it leaves no loop-invariant register for the actor
 * loop -- so r10 stays free, `frame` is NOT spilled (the ROM keeps it at
 * sp+0x2c and reloads it eight times, we keep it in r11), `base` lands in r10
 * instead of the ROM's r9, and the frame shrinks from 0x34 to 0x2c.  Every one
 * of the 73 hunks is downstream of that.
 *
 * MEASURED, and none of it moves the decision (all still 466/1036 unless
 * noted, `fold` normalises every rewriting of the index expression):
 *   - `(j + 1) * 8` for the four uses and `j * 8 + 0xc` for the actor test.
 *   - `(j + 1) * 8` for the four and `(j + 1) * 8 + 4` for the actor test.
 *   - `j * 8 + 8` for the four and `(j + 1) * 8 + 4` for the actor test.
 *   - `j * 8 + (4 * sizeof ...)`-style rewritings of the 0x70-equivalent.
 * COUNTER-SHARING WAS SWEPT SEPARATELY, because in Anim_Whirlwind that is what
 * moved the counters into r8/r9.  The ROM shares r11 across the init sweep,
 * the j loop and the final 0x40 pass, and r6 across the slot search and the
 * actor loop.  All eight combinations were measured; the best is with the slot
 * search given its OWN counter (296 aligned of 476) against the ROM-shaped
 * all-shared spelling (286), and giving the init sweep its own counter is the
 * worst (263).  That disagreement is itself the tell that the register map is
 * not being driven by the counters here but by the missing giv.
 * ALSO MEASURED AND INERT: splitting the slot search into two pointers
 * (`p = q;` ... `q = p + 1;`) to reproduce the ROM's r5/r7 pair -- gcc merges
 * them back, 0 encodings either way, even though the ROM plainly stores `x`
 * through one and `y`/`t` through the other.
 *
 * THE NEXT STEP is a `-da` dump of the loop pass on this file to read
 * combine_givs' benefit numbers directly and find out whether the election is
 * reachable at all from C; no dump was taken here.
 *
 * ONE UNRESOLVED READING OF THE ROM, recorded so the next session does not
 * re-derive it: in the final 0x40-slot pass the ROM subtracts `Leebb9[idx]`
 * from `s->x` for the FIRST blit and nothing for the second, while it
 * subtracts `(signed char)Leebc0[idx] / 2` from `s->y` for both.  That
 * asymmetry is unlike e0564_a_b.c's twin (`- Leec5f[k]/2, - Leec63[k]/2`), but
 * `sub r2, r6` with r6 straight out of `ldrsb` is unambiguous: there is no
 * halving there.  `Leebc0` is read with `ldrb` and then `lsl #24 / asr #24`,
 * which is why it is declared `unsigned char[]` and cast, where `Leebb9` gets
 * a plain `ldrsb` and is declared `signed char[]`.
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

extern void *iwram_3001eec[];
extern Part gBuffer[];
extern int ewram_2010018;

extern unsigned char Leeba6[] __asm__(".Leeba6");
extern signed char Leebae[] __asm__(".Leebae");
extern unsigned char Leebb6[] __asm__(".Leebb6");
extern signed char Leebb9[] __asm__(".Leebb9");
extern unsigned char Leebc0[] __asm__(".Leebc0");
extern unsigned short Leebc8[] __asm__(".Leebc8");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int  Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(int n);
extern void gfree(int tag);

void Anim_Thorn(void *context)
{
    void **g;
    void **pp;
    unsigned char *base;
    void *ctx;
    int frame;
    DrawFn f1;
    DrawFn f0;
    int nframes;
    int one;
    int j;
    int m;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    REG_BG2PA = 0x100;
    REG_BLDCNT = 0;
    REG_BLDALPHA = 0x1010;
    LoadVFXFile(FILE_7e, base, 1, 1);
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        REG_BG2X = 0xffff9000;
    }
    one = 1;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, one);
    f0 = (DrawFn)g[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, one);
    *(int *)(base + (0xef << 7)) = one;
    *(int *)(base + 0x7784) = 0;
    f1 = (DrawFn)g[8];
    StartTask(Task_BlitAnim, 0x90 << 3);
    nframes = Leebb6[(*(State **)(base + 0x7828))->f18] * 8 + 0x38;
    {
        int *q = &ewram_2010018;
        j = 0;
        do {
            j++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (j != (0x80 << 3));
    }
    frame = 0;
    while (frame != nframes) {
        if (frame == nframes - 0x40) {
            _Func_80bd7dc(0x84);
        }
        if (frame >= nframes - 0x10) {
            REG_BLDCNT = 0x3f44;
            REG_BLDALPHA = (nframes - frame - 1) | 0x1000;
        }
        j = 0;
        while (j != Leebb6[(*(State **)(base + 0x7828))->f18]) {
            if (frame > j * 8 + 8) {
                int t;
                int kind;
                int w;
                int h;

                t = frame - (j * 8 + 8);
                kind = Leeba6[j];
                if (kind <= 1) {
                    w = t * 16;
                    h = t * 3 * 2;
                    if (w > 0x50) {
                        w = 0x50;
                    }
                    if (h > 0x1e) {
                        h = 0x1e;
                    }
                    if (kind & 1) {
                        f1(ctx, base, Leebae[j] - h, 0x6c - w, 0x30, w);
                    } else {
                        f0(ctx, base, Leebae[j] + h, 0x6c - w, 0x30, w);
                    }
                } else {
                    h = t;
                    w = t * 8;
                    if (w > 0x40) {
                        w = 0x40;
                    }
                    if (h > 8) {
                        h = 8;
                    }
                    if (kind & 1) {
                        f1(ctx, base + (0xf0 << 4), Leebae[j] - h, 0x6c - w, 0x20, w);
                    } else {
                        f0(ctx, base + (0xf0 << 4), Leebae[j] + h, 0x6c - w, 0x20, w);
                    }
                }
                if (frame == j * 8 + 8 + 1) {
                    *(int *)(base + 0x77a8) = 3;
                }
                if (frame < j * 8 + 8 + 3) {
                    int y0;
                    Part *p;
                    Part *q;
                    int sn;
                    y0 = (Random() & 0x1f) + 0x48;
                    q = gBuffer;
                    sn = 0;
                    while (sn != 0x40) {
                        p = q;
                        if (p->t == -1) {
                            int v;
                            v = Leebae[j] + (Random() & 0x1f) + 0x20;
                            p->x = v;
                            if (v > 0x60) {
                                p->x = 0x60;
                            }
                            q->y = y0;
                            q->t = 0;
                            break;
                        }
                        q = p + 1;
                        sn++;
                    }
                }
            }
            m = 0;
            while (m != (*(State **)(base + 0x7828))->f14) {
                if (frame == j * 8 + 0xc) {
                    _PlaySound(0x84);
                    Func_80d6888((*(State **)(base + 0x7828))->ids[m], 7, 5, m, 3);
                }
                m++;
            }
            j++;
        }
        {
            Part *s;
            s = gBuffer;
            j = 0;
            do {
                if (s->t >= 0) {
                    int idx = s->t / 2;
                    f0(ctx, base + Leebc8[idx], s->x - Leebb9[idx],
                       s->y - (signed char)Leebc0[idx] / 2,
                       Leebb9[idx], (signed char)Leebc0[idx]);
                    f1(ctx, base + Leebc8[idx], s->x,
                       s->y - (signed char)Leebc0[idx] / 2,
                       Leebb9[idx], (signed char)Leebc0[idx]);
                    s->t = s->t + 1;
                    if (s->t == 0xe) {
                        s->t = -1;
                    }
                }
                j++;
                s++;
            } while (j != 0x40);
        }
        UpdateScreenShake(8, 8);
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
