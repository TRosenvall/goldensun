/* Anim_Quake -- 0x080da2ac, 462 instructions.  PARKED.
 * NON-MATCHING, 448 of 484 encodings differ.  THAT FIGURE IS SATURATED: ours
 * is 486 encodings / 1060 bytes against the ROM's 484 / 1056, so the count is
 * NOT a distance -- we are 2 encodings and 4 bytes LONG.  The honest measure is
 * tools/aligncmp.py: 295 of 484 aligned-equal (61.0%), 227 differing in 96
 * hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Quake.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_c_a.s --func Anim_Quake
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     src/non_matching/rom_c9000/Anim_Quake.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_c_a.s Anim_Quake
 *
 * SPLIT SHAPE: NONE NEEDED ANY MORE, AND THE REFERENCE PATH MOVED MID-SESSION.
 * Anim_Quake was the first of FOUR functions in
 * asm/rom_c9000/rom_d9ab8_c_c_c_c_c.s (with Anim_Fireball, Anim_Frost,
 * Anim_Ray) and its split would have had to export `.global .Leea38`.  While
 * this reconstruction was being measured, ANOTHER AGENT WORKING THE SAME TREE
 * split that file to land Anim_Fireball: rom_d9ab8_c_c_c_c_c.s is now deleted
 * and replaced by _a (Anim_Quake alone, no data section at all), _b (the
 * elevated Anim_Fireball, src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c) and _c
 * (Anim_Frost, Anim_Ray and ALL the .rodata, with .Leea38 already `.global`).
 * So Anim_Quake is now a WHOLE-FILE conversion of
 * asm/rom_c9000/rom_d9ab8_c_c_c_c_c_a.s -- tools/datacheck.py prints nothing
 * for it -- with NO split and NO new exports.  Re-confirm that before
 * committing, because the _a/_b/_c suffixes were chosen by that other session.
 * NO SHIMS, NO PINS.
 *
 * ================================================================
 * WHAT THE FUNCTION DOES, and what is already aligned
 * ================================================================
 *
 * Prologue, the two register writes, `LoadVFXFile(FILE_8b, base + (0xf0 << 5),
 * 1, 1)`, the 0x8000-byte Func_8001af8 copy, the 16x40 tile-swizzle blit out of
 * gBuffer, the REG_BG2X / xoff branch, BuildDraw2DFuncEx, the `gPtrs[0xb8/4]`
 * blitter read, the base+0x7098 clear loop, StartTask, the 16-byte
 * `arr = Data_eda88` block copy, the frame loop with its 3-step k loop, the
 * two sin-driven blits, the per-actor Func_80e3944 hit test, the second actor
 * loop and the epilogue all appear in the right order and mostly align.
 *
 * LEVERS ALREADY PAID:
 *   - `view = g[-27];`  The ROM does NOT load `iwram_3001e80`; it loads
 *     `iwram_3001eec`, does `sub r2, #0x6c` and dereferences.  gcc cannot
 *     combine two external SYMBOL_REFs, so the source must reach the second
 *     global THROUGH the first: 0x6c / 4 = 27 elements back.  Every other file
 *     in this directory declares `extern void *iwram_3001e80;` separately and
 *     that spelling cannot produce the ROM's `sub`.
 *   - THREE COUNTER VARIABLES, NOT TWO, worth 488 -> 486 encodings and, far
 *     more importantly, 271 -> 295 aligned.  The ROM shares r4 across the
 *     base+0x7098 clear loop, the actor loop inside the k loop and the second
 *     actor loop, and uses r5/r0 for the swizzle's outer/inner counters.  Three
 *     source variables, partitioned exactly that way.  (This is the
 *     Anim_Whirlwind finding again: a shared name is what decides which
 *     register class the counters land in.)
 *   - `int arr[4]`-shaped `Vec4 arr; arr = Data_eda88;` for the ROM's
 *     `ldmia r3!, {r0,r1,r4} / stmia r2!, {r0,r1,r4} / ldr / str` -- a struct
 *     assignment, not four element copies, and the array's address is taken
 *     (it is indexed by k) so it lands at the top of the frame at sp+0x64.
 *   - `if ((*st)->f14 != 0) do { ... } while (i != (*st)->f14);` for all three
 *     actor loops, matching the ROM's guard-plus-do-while.
 *   - `*(State **)(base + 0x7828)` written out at EVERY use; the ROM re-derives
 *     it in five separate places and holds the 0x7828 in a register as an INDEX
 *     (`mov r6,r9 / ldr r3,[r6,r2]`), which is the recorded named-base tell.
 *
 * ================================================================
 * THE RESIDUE -- ONE ALLOCATION FACT DRIVES MOST OF IT
 * ================================================================
 *
 * `base` MUST LIVE IN r9 AND OURS SPILLS IT TO sp+0x48.  The ROM's frame is
 * 0x74 with ctx at the top spill slot (0x48), then frame 0x44, k 0x40, blit
 * 0x3c, view 0x38, xoff 0x34 -- and NO slot for base, because base is r9 for
 * the whole function.  Ours gives base the 0x48 slot and pushes ctx to 0x44,
 * view to 0x34, so EVERY later slot offset is 4 low and every one of the ~14
 * base uses carries a reload.  Since we are only 2 encodings long overall,
 * those reloads are being paid for by work we are missing elsewhere; the two
 * effects are tangled and the aligned figure is the only honest reading.
 * Declaration order is already ROM-shaped for the five scalars that DO spill,
 * so the fix is not a reordering -- it is whatever makes `base` outrank
 * whatever currently holds r9.  NOT YET INVESTIGATED: which allocno that is.
 *
 * THE 0x28 ROW STRIDE IN THE SWIZZLE IS THE SECOND ITEM, and it is the
 * batch-298 "distribute the multiply by hand" lever pointing the OTHER way.
 * The ROM does NOT strength-reduce `j * 0x28` to a single 0x28 walker: it keeps
 * a giv for `j * 4` in r7 and emits `adds r3, r7, r5 / lsls r3, #3`, i.e. it
 * evaluates (j*4 + j) * 8.  MEASURED, on top of the three-counter version:
 *   - `base + j * 0x28`             486 encodings / 1060 bytes, 295 aligned
 *   - `t = j * 4; base + (t+j)*8`   482 encodings / 1052 bytes, 275 aligned
 * The hand-decomposed form is 2 SHORT where the plain form is 2 LONG, and it
 * reproduces the ROM's r7 giv -- but it aligns 20 encodings WORSE, because the
 * named `t` takes a register the ROM spends elsewhere.  Both readings are
 * recorded because neither is a distance and the pair brackets the answer.
 * This file keeps the better-aligned spelling.
 *
 * ALSO MEASURED AND WRONG: `*(unsigned char *)(idx + (int)gBuffer)` instead of
 * `gBuffer[idx]`, to reproduce the ROM's `ldrb r3,[r3,r2]` (index register
 * FIRST, gBuffer second, where ours has them the other way round): 492
 * encodings / 1072 bytes, 249 aligned -- decisively worse, and combined with
 * the stride decomposition 490 / 1068 / 251.  The operand order in that
 * `ldrb` is therefore NOT coming from the source's operand order, and the real
 * cause is still open.
 *
 * ONE MORE UNRESOLVED POOL FACT: the ROM pools Func_8001af8 BEFORE gBuffer
 * (0x80 then 0x84) and loads `ldr r3,=Func_8001af8` before `ldr r0,=gBuffer`;
 * ours pools gBuffer first and keeps it in a callee-saved register across the
 * call instead of rematerialising the symbol afterwards for `lr`.  objcmp
 * reports it as a RELOCATIONS difference, and it is the ONLY relocation
 * difference in the whole function -- every one of the other 30 relocations is
 * in the same order with the same symbol.  Per the batch-295 note this is a
 * relocation FORM difference and may well vanish at `make compare`, but the
 * two pool words are genuinely transposed, so treat it as real until measured.
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

typedef struct { int v[4]; } Vec4;

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern unsigned char gBuffer[];
extern Vec4 Data_eda88;

extern unsigned char Leea38[] __asm__(".Leea38");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int  sin(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int *_GetBattleActor(int id);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(int n);
extern void gfree(int tag);

void Anim_Quake(void *context)
{
    void **g;
    void **pp;
    unsigned char *base;
    void *ctx;
    int frame;
    int k;
    DrawFn blit;
    void *view;
    int xoff;
    CopyFn copy;
    Vec4 arr;
    vec3_t in;
    vec3_t out;
    int i;
    int j;
    int m;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    view = g[-27];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    REG_BG2PA = 0x100;
    REG_BLDCNT = 0;
    LoadVFXFile(FILE_8b, base + (0xf0 << 5), 1, 1);
    copy = Func_8001af8;
    copy((volatile u16 *)gBuffer, (void *)0x6008000, 0x80 << 8);
    j = 0;
    do {
        int y = j + 0x60;
        unsigned char *d = base + j * 0x28;
        i = 0;
        do {
            int x = i + 0x20;
            *d = gBuffer[(x & 7) + x / 8 * 0x40 + ((y & 7) << 3) + y / 8 * 0x800];
            i++;
            d++;
        } while (i != 0x28);
        j++;
    } while (j != 0x10);
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        REG_BG2X = 0xffff9000;
        xoff = -0x70;
    } else {
        xoff = 0;
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    blit = (DrawFn)gPtrs[0xb8 / 4];
    m = 0;
    if ((*(State **)(base + 0x7828))->f14 != 0) {
        int *q = (int *)(base + 0x7098);
        do {
            *q = 0;
            m++;
            q = (int *)((char *)q + 0x1c);
        } while (m != (*(State **)(base + 0x7828))->f14);
    }
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    StartTask(Task_BlitAnim, 0x90 << 3);
    arr = Data_eda88;
    *(int *)(base + 0x77a8) = 0x80;
    _PlaySound(0x8d);
    frame = 0;
    while (frame != Leea38[(*(State **)(base + 0x7828))->f18 * 3]) {
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        if (frame == Leea38[(*(State **)(base + 0x7828))->f18 * 3] - 0x10) {
            _Func_80bd7dc(0x85);
        }
        k = 0;
        do {
            if ((frame & 0x1f) + 0x20 == k * 4 + 0x10) {
                arr.v[k] += 0x20;
            }
            if (frame >= k * 4 + 0x10) {
                if (frame < k * 4 + 0x10
                        + Leea38[(*(State **)(base + 0x7828))->f18 * 3 + 1]) {
                    int amp;
                    int ytop;
                    amp = (arr.v[k] * sin((frame - (k * 4 + 0x10)) << 10)) >> 16;
                    if (amp < 0) {
                        amp = -amp;
                    }
                    ytop = 0x70 - amp;
                    blit(ctx, base + (0xf0 << 5), k * 0x28 + 8, ytop, 0x28, amp);
                    blit(ctx, base, k * 0x28 + 8, 0x60 - amp, 0x28, 0x10);
                    m = 0;
                    if ((*(State **)(base + 0x7828))->f14 != 0) {
                        do {
                            int *actor;
                            actor = _GetBattleActor(
                                (*(State **)(base + 0x7828))->ids[m]);
                            actor = (int *)*actor;
                            in.x = actor[2];
                            in.y = actor[3];
                            in.z = actor[4];
                            Func_80e3944(&in, &out);
                            out.x = out.x + xoff;
                            if (out.x >= k * 0x28 + 8
                                    && out.x <= k * 0x28 + 0x28 + 8
                                    && out.y >= ytop) {
                                actor[10] = 0xc0 << 12;
                                actor[18] = 0xab85;
                            }
                            if (actor[3] < 0) {
                                Func_80d6888(
                                    (*(State **)(base + 0x7828))->ids[m],
                                    0, 5, -1, 0);
                            }
                            m++;
                        } while (m != (*(State **)(base + 0x7828))->f14);
                    }
                }
            }
            k++;
        } while (k != 3);
        m = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            int *p = (int *)(base + (0xe1 << 7));
            do {
                int *actor;
                actor = _GetBattleActor((*(State **)(base + 0x7828))->ids[m]);
                actor = (int *)*actor;
                if (p[6] == 0 && actor[3] <= 0 && actor[10] < 0) {
                    p[6] = 1;
                    Func_80d6888((*(State **)(base + 0x7828))->ids[m], 7, 5, m, 5);
                }
                m++;
                p = (int *)((char *)p + 0x1c);
            } while (m != (*(State **)(base + 0x7828))->f14);
        }
        UpdateScreenShake(Leea38[(*(State **)(base + 0x7828))->f18 * 3 + 2],
                          Leea38[(*(State **)(base + 0x7828))->f18 * 3 + 2]);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
