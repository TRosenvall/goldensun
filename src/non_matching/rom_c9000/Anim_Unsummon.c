/* Anim_Unsummon (asm/rom_c9000/rom_e6638_a_c_c.s, 411 instructions) --
 * NON-MATCHING, 415 of 432 encodings differ.  SIZE 968 against the ROM's 976
 * (-8) and COUNT 428 against 432 (-4), so THAT FIGURE IS SATURATED, NOT A
 * DISTANCE -- read the four-instruction deficit below, not the 415.
 * tools/aligncmp.py puts it at 271 of 432 aligned-equal (62.7%, 80 hunks) and
 * that number is NOT an objcmp number and must not be quoted as one.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/e6638_Unsummon.c \
 *     asm/rom_c9000/rom_e6638_a_c_c.s --func Anim_Unsummon
 *
 * THE RELOCATION SEQUENCE IS ALREADY THE ROM'S -- 46 relocations, same symbols
 * in the same order, every offset shifted by the missing 8 bytes.  Per batch
 * 295, read the SEQUENCE: there is no wrong symbol and no wrong argument order
 * here, so the literal-pool ordering levers are all already satisfied.
 *
 * SPLIT SHAPE: TEXT-ONLY, NO NEW EXPORTS.  tools/datacheck.py is SILENT for
 * rom_e6638_a_c_c.s -- it has no data section.  The file holds TWO functions,
 * Anim_Unsummon (first) and Func_80e727c (44 instructions, second), so landing
 * this is a two-way split of the stem:
 *
 *   src/rom_c9000/rom_e6638_a_c_c_a.c   THIS FILE      (Anim_Unsummon)
 *   asm/rom_c9000/rom_e6638_a_c_c_b.s   Func_80e727c   (unchanged asm)
 *
 * and stage1.ld line 1925 `asm/rom_c9000/rom_e6638_a_c_c.o(.text)` becomes those
 * two, in that order.  There is no `.rodata` row for this stem to duplicate.
 * Both pieces need the stem's `.include "macros.inc"` + `.include "gba.inc"`.
 * `.Leee56`, `.Leee5e` and `.Leee66` are read as externs and are ALREADY
 * `.global` in asm/rom_c9000/rom_e6638_c.s, and `Data_ede48` is `.incdata` in
 * asm/rom_c9000/rom_eda78.s -- so NO asm edit is required.
 *
 * SHIMS: none.  `python3 tools/shimcount.py` is clean -- zero pins, no
 * fakematch.txt row needed.  No per-file Makefile flag override applies.
 *
 * ================================================================
 * THE WHOLE RESIDUE IS ONE ALLOCATION DECISION, WORTH EXACTLY 4 INSTRUCTIONS
 * ================================================================
 *
 * The ROM spills `ctx` and keeps `cx` in fp; this candidate keeps `ctx` in fp
 * and `cx` in r9.  Everything else follows from that, and the deficit is in two
 * places and nowhere else (verified by an aligncmp delta tally -- every other
 * hunk is a +1/-1 scheduling pair that cancels):
 *
 *   PROLOGUE, 2 instructions.  The ROM's param pseudo lives in fp from entry
 *     (`mov fp, r1`, then `mov r2, fp` to take the copy), so cx inherits fp.
 *     Ours computes into r2 and moves the result to r9 at the end, which needs
 *     no parm copy at all.
 *   LOOP-3 PREHEADER, 2 instructions.  `ldr r3, =Data_ede48 / mov r9, r3`.
 *
 * WITH ctx SPILLED, r9 IS FREE AND THE loop-3 TABLE INVARIANT TAKES IT -- and
 * the two-instruction form is a Thumb constraint, not a lever: a pc-relative
 * `ldr` cannot target r8-r11, so a symbol allocated to r9 is ALWAYS `ldr` into
 * a low register plus a `mov`.  Do not read that pair as a source-level copy.
 *
 * WHY IT IS A NEAR-TIE, measured.  Weighting each reference by flow.c's
 * loop_depth, `ctx` and `cx` both come to 12:
 *   ctx  1 def (depth 1) + 1 use in the frame-loop blit (2) + 3 uses in the
 *        inner-loop blits (3+3+3)                                       = 12
 *   cx   param def + `x += K` + `x /= 2` + `px = x` (1+1+1+1) + one use in each
 *        of the three init loops (2+2+2) + `cx >> 16` (2)               = 12
 * allocno_compare then divides by live_length, and cx's range starts one insn
 * earlier as the parameter, so cx loses by a hair.  r11 is LAST in
 * REG_ALLOC_ORDER, so whoever holds fp is the lowest-priority winner: the ROM's
 * fp is cx, ours is ctx.  This is an allocation-INPUT question exactly as batch
 * 300 settled -- do NOT reach for REG_ALLOC_ORDER.
 *
 * MEASURED NEGATIVE, seven spellings, none of which frees r9 (all leave `ctx`
 * in fp; the generated prologue is byte-identical across the placement set, so
 * placement of the table pointer is INERT here):
 *   `unsigned short *tab = Data_ede48;` in loop 3's preheader, indexed
 *     `tab[n-1]`                                            972 / 430, 413 diff
 *   the same with the byte-offset form
 *     `*(unsigned short *)((char *)tab + (n2 - 2))`          972 / 430, 413 diff
 *   the same assigned at the TOP OF THE FRAME-LOOP BODY      972 / 430, 413 diff
 *   the same assigned BEFORE THE FRAME LOOP                  972 / 430, 413 diff
 *   `register unsigned char *base __asm__("r10")`            972 / 430, 409 diff
 *   `register int i __asm__("r8")`                           968 / 428 (inert)
 *   `tab` used in BOTH loop 2 and loop 3                     980 / 434, 416 diff
 *
 * AND THE THREE THAT READ "CLOSER" ARE THE TRAP THE BRIEF NAMES.  Every 430 above
 * is closer to 432 by COUNT and WORSE by shape: the `tab` local is rematerialised
 * inside the loop anyway (`ldr r6, =Data_ede48` in the body) AND the pointer
 * arithmetic degrades from the ROM's `sub r1, r5, #2 / ldrh r1, [r0, r1]` to
 * `add r3, r6, r4 / sub r3, #2 / ldrh r1, [r3]`.  A COUNT THAT MOVES TOWARD THE
 * ROM WHILE THE SHAPE MOVES AWAY IS NOT PROGRESS; this file keeps the 428 form.
 *
 * WHY THE `tab` LOCAL CANNOT WIN A REGISTER, from the dumps (one `-da`
 * invocation).  With the table written as the global array, the pool load and
 * its use sit in ONE basic block, so the pseudo never reaches global_alloc:
 * `u4.c.18.greg` lists "21 regs to allocate" and neither REG_EQUIV
 * `Data_ede48` pseudo (183, 211 in `.17.lreg`) is among them -- loop.c does NOT
 * hoist it.  Written as a source local it DOES reach the preheader, but
 * local-alloc's `update_equiv_regs` substitutes it straight back down (one def
 * + one use), so there is still no allocno.  Given a third reference it stays
 * (the `tab` in both loops row above) -- and then global_alloc hands it r6, a
 * LOW callee-saved register, which displaces the loop pointer instead of ctx and
 * costs 2.  So the recorded docs/elevation.md rule "the assignment has to be in
 * a block that dominates the call, and in a loop every such block is also
 * reachable across the BACK EDGE -- so gcc keeps it in a callee-saved register"
 * IS BOUNDED: it does not hold for a pool-loaded SYMBOL with a single use, at
 * any of the four placements measured above.
 *
 * ================================================================
 * WHAT CLOSED THE OTHER 428, BY PASS
 * ================================================================
 *
 * Pass 1 read 426 of 432 with the relocation sequence already exact, from three
 * oracles taken off the landed siblings before writing a line:
 * src/rom_c9000/rom_cd508_c_b.c (Anim_Confuse) for the iwram_3001eec
 * pointer-table prologue and the shared `two`; src/rom_c9000/rom_e0564_a_b.c
 * (Anim_Hail) for the bare-literal rule and the reused counter; and
 * src/rom_c9000/rom_d82b0_b.c (Anim_Break) for `Data_ede48[n - 1]` beside
 * `n2 = n * 2` and for `*(short *)((char *)g + 2)`.
 *
 * Pass 2, THE ONE THAT MATTERS: THE ROM REASSIGNS THE PARAMETER, and the tell is
 * the SPILL-SLOT ORDER.  Frame objects are allocated in declaration order and,
 * with FRAME_GROWS_DOWNWARD, that is DESCENDING sp offsets, so the value at the
 * LOWEST slot has the HIGHEST regno -- it is the LAST thing created, not a
 * parameter.  Here sp+8 holds the raw `x`, below `gfx` at sp+0xc, so it cannot be
 * the incoming parameter; and fp is written TWICE (`mov fp,r3` for x+K and again
 * for the halved value), so the arithmetic is two statements on one pseudo:
 *
 *     px = x;                   -- px DECLARED LAST, hence sp+8
 *     x += 0xa0 << 14;          -- two statements: fp is written twice
 *     x /= 2;
 *
 * `px = x` ALONE IS INERT -- copy propagation folds it and the figures do not
 * move (measured: byte-identical to the one-expression form).  It only survives
 * because `x` is REDEFINED after it.  Worth 964/426 -> 968/428 and, more
 * importantly, it is what puts px at sp+8 and cx in a register at all.
 *
 * > A VALUE AT A LOWER SPILL SLOT THAN A DECLARED LOCAL CANNOT BE A PARAMETER.
 * > If the ROM keeps a parameter's original value past a point where the
 * > parameter is reused, the source REASSIGNED THE PARAMETER and copied it to a
 * > local declared LAST.  This is the declaration-order rule read backwards, and
 * > it is the same corollary batch 283 reached for givs.
 *
 * ALSO LOAD-BEARING, and all confirmed by the measurement:
 *   - THE SPILL MAP IS THE DECLARATION LIST.  ctx(0x18) f1(0x14) f0(0x10)
 *     gfx(0xc) in that order; the two blit pointers are TWO SCALARS, NOT an
 *     array.  An array would be an expand-time object and expand-time objects
 *     take the HIGHEST addresses (Anim_Confuse's `fns[2]` sits at sp+0x20 under
 *     its two vec3_t), whereas here the top two slots are the spilled parameters
 *     `mode`(0x20) and `y`(0x1c) -- so this function has NO addressed locals.
 *   - `REG_BG2PA = 0x80` and `REG_BLDCNT = 0x3f46` as plain assignments.  Both
 *     pool, and that is CORRECT: a CONST_INT stored in HImode has no Thumb
 *     movhi-immediate, so it goes through force_const_mem; the adjacent SImode
 *     `REG_BG2X = 0` gets `mov r3, #0`.  Anim_Confuse's byte-exact `ldrh r3, .L33`
 *     for `REG_BG2PA = 0x100` is the confirmation.
 *   - ONE counter `i` across ALL SIX loops -- r8 in every one of them, the
 *     counters-unify half of the family lever.
 *   - THREE `Part *` walkers on the pointers-split half: `p` (init loops 1 and 3,
 *     r7), `q` (the 3-iteration init loop and the 3-iteration frame loop, r5),
 *     `g` (the 0x1e and 0x3c frame loops, r6).
 *   - `i = 0;` BEFORE `p = gBuffer;`: the ROM's preheader is
 *     `mov r3,#0 / ldr r7,=gBuffer / mov r8,r3`, so the zero is born first.
 *   - The Anim_Flare embedded-assignment idiom on BOTH size arguments of the
 *     first blit: `(cx >> 16) - ((w = Leee56[k]) >> 1)` with `unsigned char w`
 *     for the ROM's `lsr` (a signed int gives `asr`), and the same for `h`.
 *   - `(unsigned)(frame - 0x24) <= 0x1b` -- an unsigned guard round a signed
 *     `/ 7`, the recorded no-value-range-propagation lever.
 *   - `n = g->t / 16 + 3` RE-READ AFTER `Func_80e3908` in both frame loops: the
 *     ROM re-derives the slot (`ldr r4,[r6,#0x18]`) and caching it is wrong.
 *   - `mag = (Random() & 0xff) + (0x80 << 1)` with the shift left UNFOLDED, and
 *     `fill = Func_80008d4; fill((void *)0x6004000, 0x80 << 7);` through a
 *     function-pointer local for the ROM's `bl _call_via_r3`.
 *
 * NEXT STEP FOR WHOEVER PICKS THIS UP: it is ONE quantity.  Find a source form
 * that lowers `ctx`'s priority below `cx`'s (or raises cx's) without adding an
 * instruction, and all four fall together with the frame going 0x20 -> 0x24.
 * Seven spellings are on file above; none of them is it.  A pin on ctx is not
 * available (you cannot pin a value INTO memory) and pinning base or the counter
 * is measured flat.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*FillFn)(void *dst, int len);

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern void *iwram_3001eec[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern unsigned char Leee56[] __asm__(".Leee56");
extern unsigned char Leee5e[] __asm__(".Leee5e");
extern unsigned short Leee66[] __asm__(".Leee66");

extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void _PlaySound(int id);
extern void Func_80e3908(Part *g, int a, int b);
extern void Func_80e6d3c(int mode, int x, int y);
extern void Func_80008d4(void *dst, int len);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Unsummon(int mode, int x, int y)
{
    void *ctx;
    DrawFn f1;
    DrawFn f0;
    unsigned char *gfx;
    unsigned char *base;
    void **tbl;
    void **pp;
    Part *p;
    Part *q;
    Part *g;
    FillFn fill;
    int ang;
    int i;
    int frame;
    int two;
    int arg;
    int px;

    tbl = iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    px = x;
    x += 0xa0 << 14;
    x /= 2;
    gfx = (unsigned char *)tbl[2];
    REG_BG2PA = 0x80;
    REG_BG2X = 0;
    REG_BLDCNT = 0x3f46;
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    f0 = (DrawFn)tbl[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
    f1 = (DrawFn)tbl[8];
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_5e, base, 1, 0);
    LoadVFXFile(FILE_5f, base + 0x59d8, 0, 0);
    *(int *)(base + (0xef << 7)) = two;
    *(int *)(base + 0x7784) = 0x32;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    p = (Part *)(base + (0xe1 << 7));
    i = 0;
    do {
        int mag = (Random() & 0xff) + (0x80 << 1);
        int a = Random() & 0xffff;
        p->x = x;
        p->y = y;
        p->vx = (sin(a) * mag) >> 7;
        p->vy = -((cos(a) * mag) >> 6);
        p->t = (Random() & 0xf) + 0x10;
        i++;
        p++;
    } while (i != 0x40);
    q = (Part *)(base + 0x772c);
    i = 0;
    ang = 0;
    do {
        q->x = x;
        q->y = y;
        q->vx = (sin(ang) << 5) >> 6;
        q->vy = -((cos(ang) << 5) >> 5);
        i++;
        ang += 0x5555;
        q++;
    } while (i != 3);
    i = 0;
    p = gBuffer;
    do {
        int mag = (Random() & 0xff) + 0x20;
        int a = Random() & 0xffff;
        p->x = x;
        p->y = y;
        p->vx = (sin(a) * mag) >> 6;
        p->vy = -((cos(a) * mag) >> 5);
        p->t = (Random() & 0xf) + 0x14;
        i++;
        p++;
    } while (i != 0x40);
    frame = 0;
    do {
        if (frame == 4) {
            _PlaySound(0x9a);
        }
        if (frame == 0x20) {
            _PlaySound(0xd4);
        }
        if (frame <= 0x2f) {
            int k = (frame - 8) / 5;
            unsigned char w;
            unsigned char h;
            if (k < 0) {
                k = 0;
            }
            f0(ctx, base + Leee66[k],
               (x >> 16) - ((w = Leee56[k]) >> 1),
               (y >> 16) - ((h = Leee5e[k]) >> 1),
               w, h);
        }
        g = (Part *)(base + (0xe1 << 7));
        i = 0;
        do {
            if (frame > i / 2 && g->t > 0) {
                int n;
                int n2;
                g->t = g->t - 1;
                Func_80e3908(g, 0x3c, 0);
                n = g->t / 16 + 3;
                n2 = n * 2;
                f1(ctx, gfx + Data_ede48[n - 1],
                   *(short *)((char *)g + 2) - n / 2,
                   *(short *)((char *)g + 6) - n,
                   n, n2);
            }
            i++;
            g++;
        } while (i != 0x1e);
        i = 0;
        g = gBuffer;
        do {
            if (frame > 0x23 && g->t > 0) {
                int n;
                int n2;
                g->t = g->t - 1;
                Func_80e3908(g, 0x3c, 0);
                n = g->t / 16 + 1;
                n2 = n * 2;
                f1(ctx, gfx + Data_ede48[n - 1],
                   *(short *)((char *)g + 2) - n / 2,
                   *(short *)((char *)g + 6) - n,
                   n, n2);
            }
            i++;
            g++;
        } while (i != 0x3c);
        q = (Part *)(base + 0x772c);
        i = 0;
        do {
            if ((unsigned)(frame - 0x24) <= 0x1b) {
                Func_80e3908(q, 0x40, 0);
                f0(ctx, base + (frame - 0x24) / 7 * 0x120 + 0x59d8,
                   *(short *)((char *)q + 2) - 6,
                   *(short *)((char *)q + 6) - 0xc,
                   0xc, 0x18);
            }
            i++;
            q++;
        } while (i != 3);
        if (frame <= 0x23) {
            Func_80e6d3c(mode, px, y);
        }
        *(int *)(base + 0x7824) = 1;
        frame++;
        WaitFrames(1);
    } while (frame != 0x48);
    StopTask(Task_BlitAnim);
    fill = Func_80008d4;
    fill((void *)0x6004000, 0x80 << 7);
    gfree(0x2f);
    gfree(0x2e);
}
