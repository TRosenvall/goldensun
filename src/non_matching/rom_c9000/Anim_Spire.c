/* Anim_Spire -- 0x080e2538, the SECOND of the two functions in
 * asm/rom_c9000/rom_e0564_c.s (Anim_Thor is the first and is unattempted).
 *
 * NON-MATCHING, 249 of 432 encodings differ.
 * SIZE is the ROM's (956 bytes = 956) and INSTRUCTION COUNT is the ROM's
 * (432 = 432), so 249 is a TRUE distance and not a saturated figure -- but it
 * is heavily INFLATED by position.  tools/aligncmp.py reads 375 of 432
 * aligned-equal (86.8%) with only 64 differing in 29 hunks, because ONE extra
 * instruction at index 185 shifts every later index by one and objcmp compares
 * position by position.  READ THE ALIGNCMP VIEW: the real distance is 64, and
 * closing the single insert at 185 should collapse objcmp's figure with it.
 * All 35 relocations are in the ROM's SYMBOL SEQUENCE (objcmp's
 * `RELOCATIONS differ` is 15 shifted OFFSETS from the same one-instruction slip).
 * objcmp verbatim:
 *     XX ENCODINGS differ in 249 place(s) (ref 432, ours 432)
 *     XX RELOCATIONS differ          <- offsets only; the SYMBOL SEQUENCE matches
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Spire.c \
 *     asm/rom_c9000/rom_e0564_c.s --func Anim_Spire
 *   (residue view: tools/aligncmp.py with the same two paths plus
 *    `Anim_Spire -v`)
 *
 * SPLIT SHAPE IF IT LANDS -- THE MOST EXPENSIVE OF THE THREE.
 * `python3 tools/datacheck.py asm/rom_c9000/rom_e0564_c.s`:
 *     data sections : .rodata
 *     functions     : Anim_Thor, Anim_Spire
 *     EXPORTS       : .Leec5f .Leec63 .Leec68 .Leec70 .Leec74 .Leec7d .Leec86
 *                     .Leec98 .Leeca1   (already global -- NOT the set a split needs)
 *     Anim_Spire reads .Leecb2 .Leecf2 .Leecf7 .Leecfc .Leecff .Leed0e .Leed1e
 *     *** SPLIT MUST EXPORT: .global for all SEVEN of those
 * so this is a TEXT/DATA split: the .rodata keeps its own object beside
 * Anim_Thor and gains seven `.global` lines, and this file reads all seven as
 * `extern ... __asm__(".LeecXX")`.  Note the TYPES, all read off the ROM:
 * .Leecf2 is SIGNED char (`ldrsb`), .Leecb2 / .Leecf7 / .Leecfc / .Leecff /
 * .Leed0e are unsigned char (`ldrb`), .Leed1e is unsigned short (`ldrh` with a
 * doubled index).
 * THE REFERENCE KEEPS A LITERAL POOL INSIDE THE FUNCTION (the `.align 2, 0 /
 * .Le25a0: .word 0x100 / .Le25a4: .word 0 / .pool` block after the `b .Le25b8`
 * at +0xa0), so `tryc.py --align` is USELESS here -- objcmp only.  gcc puts the
 * same two words in the same mid-function pool, which is why the first 100
 * encodings align exactly.
 * SHIMS: none -- `python3 tools/shimcount.py` is clean.
 *
 * ================================================================
 * THREE LEVERS TOOK IT 317 -> 249 (51.3% -> 86.8% ALIGNED), AND TWO OF THEM
 * ARE ABOUT WHO OWNS THE OUTER LOOP'S INVARIANTS
 * ================================================================
 *
 * 317 of 432, 6 instructions SHORT (51.3% aligned).  Pass 1.
 *
 * 317 -> 315, 2 SHORT  THE PARTICLE-SEED LOOP'S BASE IS `boff + i * 0x8c`, NOT
 *   `&gBuffer[i * 0x15]`.  The ROM computes the 21-Part stride as a giv PLUS a
 *   residual multiply -- r7 starts at 0 and gains 0xe0 << 1 (= 16 Parts) per
 *   outer iteration, and `movs r3,#0x8c / muls r4,r3` adds 5 Parts more:
 *       Part *g = (Part *)((char *)gBuffer + boff + i * 0x8c);
 *       ... boff += 0xe0 << 1;
 *   A single `&gBuffer[i * 0x15]` gives one `muls` by 0x24c and no giv at all,
 *   and the missing outer-latch `adds r7,r7,r4` is two of the six missing
 *   instructions.  The 0x1c0 / 0x8c decomposition is the ROM's, so `boff` is a
 *   source variable here exactly as in Anim_Venus / Anim_Jupiter / Anim_Mars.
 *
 * 315 -> 314  THE BLIT'S THREE TABLES ARE READ IN ARGUMENT ORDER, VIA EMBEDDED
 *   ASSIGNMENTS.  Anim_Flare's pool-ORDER lever, unchanged:
 *       fn(ctx, base + Leed1e[idx] + 0x83c,
 *          *(short *)((char *)g + 2) - ((w = Leecff[idx]) >> 1),
 *          *(short *)((char *)g + 6) - ((h = Leed0e[idx]) >> 1), w, h);
 *   with `u8 w; u8 h;`.  Assigning w and h as statements before the call gives
 *   Leecff the first `ldr` where the ROM gives it to Leed1e.
 *
 * 314 -> 249, AND THE COUNT BECOMES EXACT  THE Leecb2 BASE MUST BE A NAMED
 *   POINTER HOISTED OUT OF THE OUTER LOOP.
 *   > A `ptr = SYMBOL;` written INSIDE a loop body is not the same as one
 *   > written in its preheader: gcc-2.96 will not always promote the address to
 *   > a callee-saved register from inside, and when it does not, every iteration
 *   > pays its own pool `ldr`.
 *   The ROM keeps &Leecb2 in sl for the whole outer loop (`ldr r2,=.Leecb2 /
 *   mov sl,r2` in the preheader, `mov r6,sl` in the body).  Writing
 *   `unsigned char *tp = Leecb2;` inside the body leaves the `ldr` in the body.
 *   Writing
 *       tbase = Leecb2;        (in the preheader, before `boff = 0;`)
 *       do { unsigned char *tp = tbase; ... } while (...);
 *   moves it out, and that single edit closed the last 2 missing instructions
 *   and 11 more encodings.
 *
 * WHAT IS ALREADY RIGHT AND MUST NOT BE DISTURBED
 *   - `pp = tbl; base = *pp++; slot = (State **)(base + 0x7828); ctx = *pp;`
 *     -- `base` is SPILLED (sp+0x1c) and `slot` lives in r8; the ROM computes
 *     the 0x7828 address ONCE here and then re-derives it from `base` later, so
 *     the knockback loop must NOT reuse `slot` (it reads
 *     `(*(State **)(base + 0x7828))->f14`).
 *   - `GetBattleActorPos2((*slot)->ids[(*slot)->f14 - 1], &pos2)` for the ROM's
 *     `ldr r3,[r2,#0x14] / lsl r3,#1 / add r3,#0x22 / ldrsh r0,[r2,r3]`.
 *   - `mid = pos1.x + (pos2.x - pos1.x) / 2; pos1.x = mid;
 *      REG_BG2X = (0x40 - mid) << 8;`
 *   - `REG_BG2PA = 0x100; REG_BLDCNT = 0;` -- BOTH become pool words on Thumb
 *     (gcc emits `ldrh r3, .LC` even for the zero), which is what puts two
 *     `.word`s in the mid-function pool.
 *   - `while (frame != Leecf7[last] + 0x50)` as a `while`, so
 *     duplicate_loop_exit_test manufactures the entry guard the ROM has
 *     (`movs r4,#0x50 / negs r4,r4 / cmp r3,r4` -- the +0x50 folded through the
 *     comparison with frame == 0).
 *   - TWO SEPARATE `if`s, not an if/else, around the k loop:
 *         if (frame >= Leecf7[i] + 0x12) { ...0x15-particle loop... }
 *         if (frame <  Leecf7[i] + 0x12) { ...the single-sprite work... }
 *     The ROM RE-TESTS after the k loop (`cmp r11,r3 / bge / b`) instead of
 *     jumping, which only happens when the source states the condition twice.
 *   - the k loop INDEXES, it does not walk: `Part *g = &gBuffer[i * 0x15 + k];`
 *     recomputed every iteration, with `i * 4` CSE'd to sp+8 -- the ROM's
 *     `ldr r3,[sp,#8] / add r3,r9 / lsl r3,#2 / add r3,r9 / add r3,r8` then
 *     `lsl r2,r3,#3 / sub r2,r3 / lsl r2,#2`.
 *   - `Leecf2[i]` and `Leecf7[i]` stay INLINE inside the inner loops.  They look
 *     invariant but the loops call Random / __modsi3 / the blit, so LICM cannot
 *     hoist a load across them -- and the ROM does not.
 *   - `((u32)Random() % 0x60 - 0x30)` for `bl __umodsi3` (UNSIGNED) against
 *     `k % 5` and `g->t / 0x60 % 3` for `bl __modsi3` / `bl __divsi3` (signed).
 *   - `q->y += q->vy;` read BEFORE the `if (frame > Leecf7[i])` that adds
 *     0x80 << 9 -- the ROM reuses the OLD q->vy in r2 for the second store.
 *   - THE DECLARATION LIST IS THE FRAME MAP, DESCENDING: pos1 0x2c, pos2 0x20,
 *     base 0x1c, ctx 0x18, fn 0x14, cnt 0x10, last 0xc, with gcc's own `i * 4`
 *     temporary at 0x8 and `sub sp, #0x38`.
 *
 * MEASURED WORSE / INERT:
 *   - `frame = 0;` before `last = cnt - 1;`  -- 434 instructions, TWO LONG.
 *   - `last` declared before `cnt` -- 252, and the frame map is unchanged.
 *   - `tp = tbase;` before the `g` initialiser instead of after -- 250.
 *
 * ================================================================
 * THE BLOCKER: ONE EXTRA `ldr rX,[sp,#0xc]` AT THE TOP OF THE FRAME LOOP,
 * WHICH IS RELOAD INHERITANCE, AND THE 4-BYTE POOL SHIFT IT CAUSES
 * ================================================================
 *
 * The duplicated guard at .Le26b2 ends `ldr r1,=.Leecf7 / movs r4,#0x50 /
 * ldrb r3,[r1,r2] / negs r4,r4 / cmp r3,r4 / bne .Le26ca`, and the ROM's loop
 * top at .Le26ca then does `ldrb r3,[r1,r2]` REUSING BOTH r1 (&Leecf7) and r2
 * (`last`) from the guard -- even though `last` is a spilled pseudo that the
 * guard has just written to sp+0xc.  Ours inherits r2 (the symbol) but not the
 * index, so it emits an extra `ldr r0,[sp,#0xc]`.  That single insertion is the
 * whole reason objcmp reads 249 instead of ~64: every `ldr [pc,#N]` after it is
 * two bytes out of step, which also costs the `.short 0x0000` the ROM's second
 * pool needs for alignment.
 *
 * This is reload's inheritance decision (`reload_reg_reaches_end` /
 * `reload_inheritance_insn`), not a source construct, so the lever -- if there
 * is one -- is whatever changes `last`'s reload class or its death point.  It is
 * NOT the REG_ALLOC_ORDER class.
 *
 * NEXT IDEAS, NONE TRIED:
 *   1. Spell the guard's comparison so `last`'s reload register survives, e.g.
 *      hoisting `Leecf7[last]` (not `last`) into a local read by both the guard
 *      and the loop top -- the ROM re-does the `ldrb` but not the index load,
 *      which is exactly the shape a `u8 tail = Leecf7[last];` would NOT give,
 *      so this needs measuring rather than reasoning.
 *   2. The rest of the 64 is low-register rotation in five blocks plus three
 *      one-position sched2 ties; they are downstream of (1).
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
extern Part gBuffer[];
extern unsigned char Leecb2[] __asm__(".Leecb2");
extern signed char Leecf2[] __asm__(".Leecf2");
extern unsigned char Leecf7[] __asm__(".Leecf7");
extern unsigned char Leecfc[] __asm__(".Leecfc");
extern unsigned char Leecff[] __asm__(".Leecff");
extern unsigned char Leed0e[] __asm__(".Leed0e");
extern unsigned short Leed1e[] __asm__(".Leed1e");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern int  Random(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80e3908(Part *p, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Spire(void *context)
{
    vec3_t pos1;
    vec3_t pos2;
    u8 *base;
    void *ctx;
    DrawFn fn;
    int cnt;
    int last;
    int **tbl;
    int **pp;
    State **slot;
    Part *p;
    Part *q;
    int i;
    int k;
    int frame;
    int mid;
    int boff;
    unsigned char *tbase;
    int arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    slot = (State **)(base + 0x7828);
    ctx = (void *)*pp;
    *slot = (State *)context;
    AnimStart(1);
    REG_BG2PA = 0x100;
    REG_BLDCNT = 0;
    LoadVFXFile(FILE_8a, base, 1, 1);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 1);
    fn = (DrawFn)tbl[7];
    GetBattleActorPos2((*slot)->ids[0], &pos1);
    GetBattleActorPos2((*slot)->ids[(*slot)->f14 - 1], &pos2);
    mid = pos1.x + (pos2.x - pos1.x) / 2;
    pos1.x = mid;
    REG_BG2X = (0x40 - mid) << 8;
    *(int *)(base + (0xef << 7)) = 1;
    *(int *)(base + 0x7784) = 0;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    cnt = Leecfc[(*slot)->f18];
    i = 0;
    if (cnt != 0) {
        p = (Part *)(base + (0xe1 << 7));
        do {
            p->y = 0xffc00000;
            p->vy = 0;
            i++;
            p++;
        } while (i != cnt);
        i = 0;
        if (cnt != 0) {
            tbase = Leecb2;
            boff = 0;
            do {
                Part *g = (Part *)((char *)gBuffer + boff + i * 0x8c);
                unsigned char *tp = tbase;
                k = 0;
                do {
                    g->x = (tp[0] + Leecf2[i]) << 16;
                    g->y = tp[1] << 16;
                    g->vx = ((u32)Random() % 0x60 - 0x30) << 10;
                    g->vy = -((Random() & 0x7f) + 0x20) << 11;
                    g->z = 0x20;
                    k++;
                    g->t = 0;
                    tp += 2;
                    g++;
                } while (k != 0x15);
                boff += 0xe0 << 1;
                i++;
            } while (i != cnt);
        }
    }
    last = cnt - 1;
    frame = 0;
    while (frame != Leecf7[last] + 0x50) {
        if (frame == Leecf7[last] + 0x30) {
            _Func_80bd7dc(0x84);
        }
        i = 0;
        if (cnt != 0) {
            q = (Part *)(base + (0xe1 << 7));
            do {
                if (frame == Leecf7[i] + 0x12) {
                    _PlaySound(0x86);
                    *(int *)(base + 0x77a8) = 4;
                }
                if (frame >= Leecf7[i] + 0x12) {
                    k = 0;
                    do {
                        Part *g = &gBuffer[i * 0x15 + k];
                        int idx = k % 5 * 3 + g->t / 0x60 % 3;
                        u8 w;
                        u8 h;
                        fn(ctx, base + Leed1e[idx] + 0x83c,
                           *(short *)((char *)g + 2) - ((w = Leecff[idx]) >> 1),
                           *(short *)((char *)g + 6) - ((h = Leed0e[idx]) >> 1),
                           w, h);
                        Func_80e3908(g, 0x40, 0x80 << 7);
                        g->t += g->z;
                        if (g->z > 1 && (frame & 1)) {
                            g->z = g->z - 1;
                        }
                        k++;
                    } while (k != 0x15);
                }
                if (frame < Leecf7[i] + 0x12) {
                    if (frame >= Leecf7[i]) {
                        fn(ctx, base, Leecf2[i] + 0x2f,
                           *(short *)((char *)q + 6), 0x22, 0x3e);
                    }
                    q->y += q->vy;
                    if (frame > Leecf7[i]) {
                        q->vy = q->vy + (0x80 << 9);
                    }
                    if (q->y > (0xc8 << 14)) {
                        q->y = 0xc8 << 14;
                    }
                }
                if (frame == Leecf7[i] + 0x12) {
                    k = 0;
                    while (k != (*(State **)(base + 0x7828))->f14) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[k], 7, 5, k, 8);
                        k++;
                    }
                }
                i++;
                q++;
            } while (i != cnt);
        }
        UpdateScreenShake(2, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
