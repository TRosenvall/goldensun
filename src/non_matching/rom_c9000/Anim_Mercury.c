/* Anim_Mercury -- 0x080dfe2c, the FIRST of the two functions in
 * asm/rom_c9000/rom_dfa18_c_c_c_c_c.s (Anim_Jupiter is the second and is parked
 * as src/non_matching/rom_c9000/Anim_Jupiter.c at 39 of 367).
 *
 * NON-MATCHING, 295 of 429 encodings differ.
 * SIZE is the ROM's (952 bytes = 952) and INSTRUCTION COUNT is the ROM's
 * (429 = 429), so 295 is a TRUE distance and not a saturated figure.
 * tools/aligncmp.py reads 259 of 429 aligned-equal (60.4%), 207 differing in
 * 60 hunks -- so unlike Anim_Mars this one still has real work in it, not just
 * ties.  All 40 relocations are in the ROM's SYMBOL SEQUENCE (objcmp's
 * `RELOCATIONS differ` here is 18 shifted OFFSETS, nothing more).
 * objcmp verbatim:
 *     XX ENCODINGS differ in 295 place(s) (ref 429, ours 429)
 *     XX RELOCATIONS differ          <- offsets only; the SYMBOL SEQUENCE matches
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Mercury.c \
 *     asm/rom_c9000/rom_dfa18_c_c_c_c_c.s --func Anim_Mercury
 *   (residue view: tools/aligncmp.py with the same two paths plus
 *    `Anim_Mercury -v`)
 *
 * SPLIT SHAPE IF IT LANDS.  `python3 tools/datacheck.py
 * asm/rom_c9000/rom_dfa18_c_c_c_c_c.s` says the file holds a .rodata section
 * and that `.Leec5a` is read by Anim_Mercury ONLY:
 *     *** SPLIT MUST EXPORT: .global .Leec5a
 * so landing Anim_Mercury needs a TEXT/DATA split -- the blob stays in the
 * remaining .s beside Anim_Jupiter and gets a `.global .Leec5a` there, this
 * file reads it as `extern unsigned char Leec5a[] __asm__(".Leec5a")`.  That
 * also settles the data question Anim_Jupiter's park raises: Anim_Jupiter needs
 * NO new export, so whichever of the two lands first should carry the
 * `.global`.  gBuffer, ewram_2010018 and Data_ede48 are already spelled as
 * externs elsewhere in the bank.
 * SHIMS: none -- `python3 tools/shimcount.py` is clean.
 *
 * ================================================================
 * THE ONE THAT MATTERS: THE Leec5a SEARCH IS A `while` WHOSE EXIT TEST GETS
 * DUPLICATED, AND THAT IS WORTH 24 INSTRUCTIONS OF LENGTH
 * ================================================================
 *
 * The ROM emits the `frame == Leec5a[i] && parts[i].t == -1` test AND the four
 * stores that follow it TWICE -- once at the entry (`movs r1,#0 / ldrb r3,[r0,r1]
 * / mov sl,r1`, i.e. indexed by a pseudo that happens to hold 0, NOT by the
 * constant 0) and once in the latch after `i++ / cmp #5`.  Written as the
 * natural
 *     do { if (frame == Leec5a[i]) { if (q->t == -1) { stores; break; } }
 *          if (frame == Leec5a[i] + 6) { ... } i++; } while (i != 5);
 * gcc emits the block ONCE, the candidate is 405 instructions against 429, and
 * every missing instruction is a `mov rX, r8` staging copy that the ROM needs
 * and we do not.  Written as a `while` whose condition IS the A test, with the
 * stores after the loop and a `goto` for the i == 5 exit:
 *     i = 0;
 *     while (frame != Leec5a[i] || ((Part *)(base + (0xe1 << 7)))[i].t != -1) {
 *         if (frame == Leec5a[i] + 6) { ...knockback... }
 *         i++;
 *         if (i == 5) goto skip;
 *     }
 *     { ...stores... }
 *   skip: ;
 * loop.c's duplicate_loop_exit_test copies the test AND the exit path to the
 * top, and the instruction count becomes the ROM's exactly.  314 -> 297.
 *
 * > A SOURCE `break` OUT OF THE MIDDLE OF A do-while IS NOT THE SAME PROGRAM AS
 * > A `while` WHOSE CONDITION IS THAT TEST.  When the ROM shows a test AND its
 * > consequent block emitted twice -- with the entry copy still using a
 * > REGISTER-OFFSET load for what is textually index 0 -- that is
 * > duplicate_loop_exit_test, and only the `while` form reaches it.
 *
 * ALSO STRUCTURAL, worth 367 -> 314 on the first pass:
 *   THE 8-SLOT `-1` LOOP IS AN `int *`, NOT A `Part *`.  The ROM loads
 *   `ldr r3,=0x7098` (a pool word) and stores at offset 0 with stride 0x1c;
 *   `p->t = -1; p++` over `base + (0xe1 << 7)` gives `str r3,[r5,#24]` and
 *   0xe1 << 7 built inline.  Anim_Venus's `int *q = &ewram_2010018;` form is
 *   the right one, with `q = (int *)(base + 0x7098)`.
 *
 * WHAT IS ALREADY RIGHT AND MUST NOT BE DISTURBED
 *   - `pp = tbl; base = *pp++; ctx = *pp; vfx = tbl[2];` -> `ldmia r3!, {r1}`.
 *   - `fp = fns;` before `BuildDraw2DFuncs(0, (void **)fp)`, then `fns[0](...)`
 *     through the array and `fp[1](...)` through the pointer, exactly as
 *     Anim_Jupiter records.
 *   - `buf = (unsigned char *)gBuffer; ... buf += 0xdd << 3;` -- ONE pointer
 *     reused for both Func_80dfddc calls (the ROM's `add r5,r3`), and gBuffer
 *     really is the scratch buffer for the two blits.
 *   - ONE `i` across all five loops (both `-1` loops, the 5-slot part loop, the
 *     Leec5a search and the 0x100-particle draw loop) -- the counters-unify
 *     rule, here reaching five.
 *   - `j` shared between the 0x20-particle inner loop and the knockback loop:
 *     the ROM spills BOTH to sp+8, which is only one slot.
 *   - `int a2 = Random() & 0xffff; int rv = Random(); g->x = p->x << 16;
 *      g->y = p->y << 16; mag = (rv & 0x1ff) + (0x80 << 1);` -- Anim_Venus's
 *     "name the second Random() result" lever, needed so the two stores can be
 *     scheduled INTO the mag computation.
 *   - `(sin(a2) * mag) >> 8` / `(cos(a2) * mag) >> 7` with the CALL SECOND, for
 *     the ROM's `mov r3,r5 / mul r3,r0` seeding (Anim_Jupiter's multiplicand
 *     rule).
 *   - the draw loop is Anim_Venus's verbatim, down to `int n = g->t / 16 + 2`
 *     and `while (i != (0x80 << 1))`.
 *   - `if (frame <= 0x5f)` -- a guard that is ALWAYS true given `frame != 0x60`,
 *     and the ROM keeps it (`ble / b`), so it is in the source.
 *   - THE DECLARATION LIST IS THE FRAME MAP, DESCENDING: fns 0x1c, ctx 0x18,
 *     frame 0x14, vfx 0x10, fp 0xc, j 0x8, `sub sp, #0x24`.
 *
 * MEASURED INERT (all 295-297, no better):
 *   - inlining the scale factor as `sin(ang) * (0x40 - frame * 2)` in both
 *     multiplications instead of a named `s` -- this is Anim_Jupiter's measured
 *     lever and it does NOT pay here, because the blocker below is upstream
 *     of it;
 *   - declaring `boff` first in the list, before `frame`, or after `j` (295 /
 *     297 / 297);
 *   - `X` and `Y` as block locals inside the `if (frame <= 0x5f)`.
 *
 * ================================================================
 * THE BLOCKER: A REGISTER-PRESSURE INVERSION -- fp HOLDS `frame` WHERE THE ROM
 * SPILLS IT AND GIVES fp TO `boff` AND THEN `Y`
 * ================================================================
 *
 * The ROM's assignment is base r9, i sl, p r8 then X r8, boff fp then Y fp,
 * frame AT sp+0x14, j at sp+8, and r5/r6/r7 for buf / a2 / mag / ang / s / sx /
 * g as their ranges allow.  Ours is identical EXCEPT that `frame` wins fp and
 * `boff` is pushed to a stack slot of its own -- which is why our frame is
 * `sub sp, #0x28`, one slot more than the ROM's 0x24, and why `ang` spills in
 * the sin/cos block where the ROM keeps it in r6.
 *
 * This is allocno_compare arithmetic, not a spelling.  priority is
 * `floor_log2(n_refs) * n_refs * size / live_length`; `frame` has ~11 references
 * over the whole loop body and `boff` has 3 over the 5-slot loop, which puts
 * frame ahead (~0.087 against ~0.033) and the ROM behind.  For the ROM's answer
 * `boff` must outrank `frame`, so either the ROM's `frame` has FEWER references
 * than ours (a different spelling of one of its nine tests) or its `boff` has
 * more.  Note this is NOT the REG_ALLOC_ORDER class: the order is correct and
 * proven; the inputs to allocation are what differ.
 *
 * NEXT IDEAS, NONE TRIED:
 *   1. Find a spelling that removes one reference to `frame` from the loop body
 *      -- e.g. whether the two `Leec5a[i]` / `Leec5a[i] + 6` tests in the ROM
 *      read frame once or twice, which the duplicated block makes hard to count
 *      from the disassembly alone.
 *   2. The whole residue past the inversion is one-position sched2 ties and
 *      low-register rotations in five blocks; they are almost certainly
 *      downstream of it, so do not chase them first.
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
extern unsigned short Data_ede48[];
extern unsigned char Leec5a[] __asm__(".Leec5a");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void BuildDraw2DFuncs(int a, void **fns);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void Func_80dfddc(unsigned char *src, unsigned char *dst, int n, int m);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80e3908(Part *p, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Mercury(void *context)
{
    int boff;
    DrawFn fns[2];
    void *ctx;
    int frame;
    unsigned char *vfx;
    DrawFn *fp;
    int j;
    void **tbl;
    void **pp;
    unsigned char *base;
    unsigned char *buf;
    Part *p;
    Part *g;
    int i;
    int X;
    int Y;
    int arg;

    tbl = iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    vfx = (unsigned char *)tbl[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    fp = fns;
    BuildDraw2DFuncs(0, (void **)fp);
    buf = (unsigned char *)gBuffer;
    LoadVFXFile(FILE_73, vfx, 0, 0);
    LoadVFXFile(FILE_92, base, 1, 0);
    LoadVFXFile(FILE_6f, buf, 1, 1);
    Func_80dfddc(buf, base + (0xaa << 2), 0x11, 0x68);
    buf += 0xdd << 3;
    Func_80dfddc(buf, base + (0x99 << 4), 0x22, 0x41);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    {
        int *q = (int *)(base + 0x7098);
        i = 0;
        do {
            i++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (i != 8);
    }
    {
        int *q = &ewram_2010018;
        i = 0;
        do {
            i++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (i != (0x80 << 2));
    }
    _PlaySound(0xa2);
    frame = 0;
    do {
        if (frame == 0x38) {
            _Func_80bd7dc(0x85);
        }
        p = (Part *)(base + (0xe1 << 7));
        i = 0;
        boff = 0;
        do {
            if (p->t != -1) {
                fns[0](ctx, base + (0x99 << 4), p->x - 0x10, p->y - 0x11, 0x22, 0x41);
                p->x -= 0xc;
                p->t = p->t + 1;
                if (p->t == 5) {
                    _PlaySound(0x85);
                    *(int *)(base + 0x77a8) = 4;
                    g = (Part *)((char *)gBuffer + boff);
                    j = 0;
                    do {
                        int a2 = Random() & 0xffff;
                        int rv = Random();
                        int mag;
                        g->x = p->x << 16;
                        g->y = p->y << 16;
                        mag = (rv & 0x1ff) + (0x80 << 1);
                        g->vx = (sin(a2) * mag) >> 8;
                        g->vy = (cos(a2) * mag) >> 7;
                        g->t = (Random() & 0xf) + 0x20;
                        j++;
                        g++;
                    } while (j != 0x20);
                }
            }
            i++;
            p++;
            boff += 0xe0 << 2;
        } while (i != 5);
        if (frame <= 0x5f) {
            int ang = frame << 11;
            int s = 0x40 - frame * 2;
            int sx = (sin(ang) * s) >> 17;
            int sy;
            X = sx + 0x60;
            sy = (cos(ang) * s) >> 16;
            Y = sy + 0x3c;
            fp[1](ctx, base, sx + 0x56, sy + 0x2b, 0x14, 0x22);
            i = 0;
            while (frame != Leec5a[i]
                   || ((Part *)(base + (0xe1 << 7)))[i].t != -1) {
                if (frame == Leec5a[i] + 6) {
                    j = 0;
                    while (j != (*(State **)(base + 0x7828))->f14) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[j], 7, 5, j, 6);
                        _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[j], 6);
                        j++;
                    }
                }
                i++;
                if (i == 5) {
                    goto skip;
                }
            }
            {
                Part *q = (Part *)(base + (0xe1 << 7)) + i;
                q->x = X - 8;
                q->vx = X;
                q->y = Y;
                q->t = 0;
            }
          skip:
            ;
        }
        g = gBuffer;
        i = 0;
        do {
            if (g->t != -1) {
                int n = g->t / 16 + 2;
                unsigned char *src = vfx + Data_ede48[n - 1];
                int w2 = n * 2;
                int x = *(short *)((char *)g + 2) - n / 2;
                int y = *(short *)((char *)g + 6) - n;
                fp[1](ctx, src, x, y, n, w2);
                Func_80e3908(g, 0x3e, 0x80 << 6);
                g->t -= 1;
            }
            i++;
            g++;
        } while (i != (0x80 << 1));
        UpdateScreenShake(4, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x60);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
