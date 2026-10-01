/* Anim_Prism -- PARKED.  0x080d3c80, 488 instructions in the ROM listing.
 * NON-MATCHING, 475 of 514 encodings differ.
 * SIZE  ref 1132 bytes, ours 1068 (-64).  COUNT ref 514, ours 485 (-29).
 * tools/aligncmp.py: aligned-equal 289 of 514 (56.2%), 262 differing in 102 hunks.
 * SHIMS: none.  `python3 tools/shimcount.py` is silent.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/Anim_Prism.c \
 *     asm/rom_c9000/rom_d2d98_c.s --func Anim_Prism
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_c9000/Anim_Prism.c \
 *     asm/rom_c9000/rom_d2d98_c.s Anim_Prism -v
 *
 * SIZE AND COUNT BOTH DIFFER, so objcmp's 475 is NOT a distance -- it saturates.
 * Rank revisions of this function with tools/aligncmp.py's aligned-equal figure.
 *
 * SPLIT SHAPE.  See park_Anim_Plasma.c beside this file: Anim_Prism is the FOURTH
 * of the SIX functions in asm/rom_c9000/rom_d2d98_c.s and Anim_Plasma the SIXTH, so
 * the two are one FOUR-WAY split of the stem and must land together.  This piece
 * needs `.global .Lee1d3 .Lee1f5 .Lee1fb .Lee207 .Lee214` added to the asm data
 * object; the data itself stays there.  `.Lee1f5` is six bytes indexed
 * `[f18*2]` (the lane count) and `[f18*2+1]` (the frame count); `.Lee1d3` is 34
 * bytes walked two at a time; `.Lee214` is a u32[12] and the other two are u8.
 *
 * ================================================================
 * WHAT IS ALREADY RIGHT
 * ================================================================
 *
 * The prologue through StartTask, the four-way blit-index ladder, both particle
 * loops, the Func_80e3908 calls, the Func_80d6888 tail and the epilogue are all
 * in place, and the ROM's structure was read off the listing rather than guessed:
 *   - the frame loop is the `if (c) { do ... while (c); }` form, because the ROM
 *     hoists `base + 0x7828` into a spill slot in its PREHEADER (contrast
 *     Anim_Ray's Func_80d6888 loop, which re-derives it and therefore wants
 *     `while`; both forms are present in this batch's three functions);
 *   - `i` is ONE counter across the init loop, both 16-particle loops and the
 *     Func_80d6888 loop, in r10 -- counters unify;
 *   - `Lee1d3` is WALKED (`ldrb r3,[r7] / ldrb r3,[r7,#1] / add r7,#2`), not
 *     indexed, and `q`/`g` are separate pointers -- pointers split;
 *   - `d = 0xc0; if (frame > 0x5f) d = 0x9c0 - frame * 24;` for the
 *     iwram_3001e80 screen shift, the constant set before the test and
 *     overwritten in the arm;
 *   - `UpdateScreenShake(f18 * 2 + 4, f18 * 4 + 8)`.
 *
 * ================================================================
 * FOUR BLOCKERS, AND THE FIRST IS THE 29 INSTRUCTIONS
 * ================================================================
 *
 * 1. THE `.Lee1f5` TABLE ADDRESS IS A SPILLED PSEUDO WITH A REG_EQUIV, AND IT IS
 *    RE-MATERIALISED SEVEN TIMES.  The ROM does `ldr r4,=.Lee1f5 / str r4,[sp,#0xc]`
 *    once immediately after StartTask, reads it back at most sites
 *    (`ldr r5,[sp,#0xc] / lsl r3,#1 / add r3,#1 / ldrb r3,[r5,r3]`), and RE-STORES
 *    it (`ldr r0,=.Lee1f5 / str r0,[sp,#0xc]`) in BOTH arms of the screen-shift
 *    `if` and again at the bottom of the frame loop.  Those re-stores are reload
 *    writing back a pseudo whose equivalent is a constant address, and they are a
 *    large part of the missing 29.
 *      - as a NAMED `unsigned char *tp = Lee1f5;` (pri2.c): 283 aligned, 489 count
 *      - as DIRECT `Lee1f5[...]` reads (pri3.c, this file): 289 aligned, 485 count
 *      - direct reads plus a declared `slot` in the frame loop (pri4.c): 277, 498
 *    So the named pointer buys the spill slot and loses alignment; the direct read
 *    keeps alignment and loses the slot.  Neither reproduces the re-stores.
 *
 *    Note also the INDEXING FORM: the ROM computes the whole index and uses the
 *    register-offset load (`lsl r3,#1 / add r3,#1 / ldrb r3,[r5,r3]`), while
 *    `tp[f18 * 2 + 1]` folds the `+1` into the ldrb displacement
 *    (`adds r3,r3,r4 / ldrb r3,[r3,#1]`).  Same instruction count, different
 *    encodings at eight sites.
 *
 * 2. `fns[k]` IS INDEXED BY A BYTE OFFSET, NOT AN ELEMENT INDEX.  The ROM holds
 *    4 or 0 -- in r12, via `mov r12,r2` / `mov r5,r12`, Thumb's only way to touch
 *    ip -- and loads with `ldr r4,[r5,r0]` and NO `lsl #2`.  Written `k = 1;
 *    if (i > 2) k = 0; fns[k](...)` gcc emits the shift, because the two constant
 *    assignments are in different arms and nothing folds `k << 2` after the join.
 *    Either the source spells the byte offset (`*(DrawFn *)((char *)fns + k)`) or
 *    something else is going on; UNTESTED, and the first thing to try next.
 *
 * 3. `q->vx` IS RE-READ FROM MEMORY ONE INSTRUCTION AFTER BEING STORED.
 *        ROM   lsl r3,#12 / str r3,[r5,#0xc] / ldr r2,[r5,#0xc] /
 *              lsl r3,r2,#3 / add r3,r2 / lsl r3,#1
 *        ours  lsl r2,r0,#12 / str r2,[..,#0xc] / lsl r3,r0,#15 / ...
 *    `q->vx = v << 12; q->x = x0 - q->vx * 18;` gives ours: cse knows the stored
 *    value and multiplies it directly, and the synthesis of *18 then hangs off
 *    `v` rather than off `vx`.  Same shape as blocker 1 -- a pseudo with a
 *    REG_EQUIV to the memory it was just stored to, rematerialised.  Pressure,
 *    and one instruction per iteration of the init loop.
 *
 * 4. WHICH LOOP-INVARIANT CONSTANT loop.c HOISTS.  The ROM hoists `0x1f` into fp
 *    and rebuilds `0x3f` at both uses; ours hoists `0x3f` and rebuilds `0x1f`.
 *    Both are used exactly twice, once in each arm of the same `if`.
 *
 *
 * THE RELOCATION LINE IS TWO FINDINGS HERE, AND BOTH CONFIRM BLOCKER 1.  Read the
 * symbol SEQUENCE, not the offsets: ref carries 41 relocations and ours 39, and
 * the two missing ones are BOTH `.Lee1f5` -- exactly the reload re-stores
 * described above.  Separately, `.Lee1f5 / iwram_3001e80 / gBuffer / .Lee214 /
 * .Lee1fb / .Lee207` sit in the ROM's FIRST pool block (before the mid-function
 * `.pool_aligned` break) and in ours they fall into a later one, so every
 * `ldr [pc,#N]` that reaches them is wrong as well.  For contrast,
 * park_Anim_Ray.c and park_Anim_Plasma.c both have a BYTE-IDENTICAL relocation
 * SEQUENCE (46 and 44 symbols in the ROM's own order) and differ only in offsets,
 * which is what a function that is two instructions from exact looks like.
 * This is the least advanced of the three functions in this batch and it is the
 * one to come back to: its residue is dominated by ONE mechanism (a spilled
 * pseudo whose REG_EQUIV is a constant address, re-stored by reload), the same
 * mechanism that costs Anim_Plasma its last two instructions, and blocker 2 is a
 * plain untested source question.
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

extern int *iwram_3001eec[];
extern void *iwram_3001e80;
extern Part gBuffer[];
extern unsigned char Lee1d3[] __asm__(".Lee1d3");
extern unsigned char Lee1f5[] __asm__(".Lee1f5");
extern unsigned char Lee1fb[] __asm__(".Lee1fb");
extern unsigned char Lee207[] __asm__(".Lee207");
extern unsigned int Lee214[] __asm__(".Lee214");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void BuildDraw2DFuncs(int a, void **fns);
extern int Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80e3908(Part *g, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Prism(void *context)
{
    DrawFn fns[2];
    void *ctx;
    int frame;
    char **tbl;
    char **pp;
    u8 *base;
    State **slot;
    Part *q;
    Part *g;
    int lane;
    int i;
    int arg;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    REG_BLDALPHA = 0x1010;
    LoadVFXFile(FILE_cf, base, 1, 1);
    BuildDraw2DFuncs((*slot)->f4, (void **)fns);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    i = 0;
    if (Lee1f5[(*(State **)(base + 0x7828))->f18 * 2] != 0) {
        int z = 0;
        q = (Part *)(base + (0xe1 << 7));
        do {
            int x0;
            int v;
            Random();
            q->y = -0x400000;
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                x0 = ((Random() & 0x1f) + 0x50) << 16;
                v = Random() & 0x3f;
            } else {
                x0 = ((Random() & 0x1f) + 8) << 16;
                v = -(Random() & 0x3f);
            }
            q->vx = v << 12;
            q->x = x0 - q->vx * 18;
            q->vy = 0;
            q->z = 0;
            q->t = z;
            i++;
            q++;
            z += 8;
        } while (i != Lee1f5[(*(State **)(base + 0x7828))->f18 * 2]);
    }
    frame = 0;
    if (Lee1f5[(*(State **)(base + 0x7828))->f18 * 2 + 1] != 0) {
      do {
        if ((*(State **)(base + 0x7828))->f18 == 2 && frame <= 0x67) {
            char *view = (char *)iwram_3001e80;
            int d = 0xc0;
            if (frame > 0x5f) {
                d = 0x9c0 - frame * 24;
            }
            if ((*(State **)(base + 0x7828))->f4 == 0) {
                *(u16 *)(view + 0x36) = *(u16 *)(view + 0x36) - d;
            } else {
                *(u16 *)(view + 0x36) = *(u16 *)(view + 0x36) + d;
            }
        }
        if (frame == Lee1f5[(*(State **)(base + 0x7828))->f18 * 2 + 1] - 0x50) {
            _Func_80bd7dc(0x86);
        }
        if (frame == Lee1f5[(*(State **)(base + 0x7828))->f18 * 2 + 1] - 8) {
            *(int *)(base + (0xef << 7)) = 3;
            *(int *)(base + 0x7784) = 0x6060606;
        }
        if (frame <= Lee1f5[(*(State **)(base + 0x7828))->f18 * 2 + 1] - 8) {
            lane = 0;
            if (Lee1f5[(*(State **)(base + 0x7828))->f18 * 2] != 0) {
                q = (Part *)(base + (0xe1 << 7));
                do {
                    if (q->z == 1) {
                        i = 0;
                        g = &gBuffer[lane * 16];
                        do {
                            int idx = i % 5 * 3 + g->t / 0x60 % 3;
                            int k = 1;
                            if (i > 2) {
                                k = 0;
                            }
                            fns[k](ctx, base + (0x80 << 4) + Lee214[idx],
                                   *(short *)((char *)g + 2) - Lee1fb[idx] / 2,
                                   *(short *)((char *)g + 6) - Lee207[idx] / 2,
                                   Lee1fb[idx], Lee207[idx]);
                            Func_80e3908(g, 0x40, 0x80 << 6);
                            g->t = g->t + g->z;
                            if (g->z > 1 && (frame & 1) != 0) {
                                g->z = g->z - 1;
                            }
                            i++;
                            g++;
                        } while (i != 0x10);
                    } else if (frame >= q->t) {
                        int b1 = lane & 1;
                        fns[b1](ctx, base, *(short *)((char *)q + 2) - 0x10,
                                *(short *)((char *)q + 6), 0x20, 0x40);
                        Func_80e3908(q, 0x40, 0x80 << 9);
                        if (q->y > (0xe0 << 14)) {
                            unsigned char *d2;
                            q->z = 1;
                            q->y = 0xe0 << 14;
                            i = 0;
                            d2 = Lee1d3;
                            g = &gBuffer[lane * 16];
                            do {
                                int vy;
                                g->x = q->x + ((d2[0] - 0x28) << 16);
                                g->y = d2[1] << 16;
                                g->vx = ((Random() & 0x7f) - 0x40) << 11;
                                vy = -(Random() & 0x7f);
                                g->vy = vy << 11;
                                if (b1 != 0) {
                                    g->vx = g->vx * 2;
                                    g->vy = vy << 12;
                                }
                                g->z = 0x20;
                                g->t = 0;
                                i++;
                                d2 += 2;
                                g++;
                            } while (i != 0x10);
                            *(int *)(base + 0x77a8) = 8;
                            _PlaySound(0x90);
                            i = 0;
                            if ((*(State **)(base + 0x7828))->f14 != 0) {
                                do {
                                    Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 4);
                                    i++;
                                } while (i != (*(State **)(base + 0x7828))->f14);
                            }
                        }
                    }
                    lane++;
                    q++;
                } while (lane != Lee1f5[(*(State **)(base + 0x7828))->f18 * 2]);
            }
        }
        UpdateScreenShake((*(State **)(base + 0x7828))->f18 * 2 + 4, (*(State **)(base + 0x7828))->f18 * 4 + 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
      } while (frame != Lee1f5[(*(State **)(base + 0x7828))->f18 * 2 + 1]);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
