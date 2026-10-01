/* Anim_DragonCloud -- PARKED.  0x080e89ec, 745 instructions in the ROM listing.
 * NON-MATCHING, 682 of 782 encodings differ.
 * SIZE  ref 1724 bytes, ours 1708 (-16) -- NOT exact.
 * COUNT ref 782, ours 775 (-7)          -- NOT exact.
 * Both axes are off, so the objcmp figure SATURATES and does not rank.
 * tools/aligncmp.py: aligned-equal 536 of 782 (68.5%), 271 differing in 128 hunks.
 * SHIMS: none.  `python3 tools/shimcount.py` is silent -- no register pins, no
 * `.equ`, no `asm volatile`, no per-file Makefile flag override.  Pin-free.
 *
 * Verify with (the delivered park body, runnable as written; installed path
 * is src/non_matching/rom_c9000/Anim_DragonCloud.c):
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     scratch_elev/b310c/PARK_Anim_DragonCloud.c \
 *     asm/rom_c9000/rom_e7320_c_c.s --func Anim_DragonCloud
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py \
 *     scratch_elev/b310c/PARK_Anim_DragonCloud.c \
 *     asm/rom_c9000/rom_e7320_c_c.s Anim_DragonCloud -v
 *
 *
 * ================================================================
 * BATCH 310C -- RE-MEASURED AS INSTALLED, AND WHY THE NEW LEVER OF THAT BATCH
 * DOES NOT REACH THIS BLOCKER
 * ================================================================
 *
 * RE-MEASURE, as installed, no edits: 682 of 782 encodings, SIZE 1708/1724, COUNT 775/782 -- both axes still
 * inexact, so the 682 still SATURATES.  aligncmp 536 of 782 (68.5%).
 * Every figure in the header above reproduces exactly.  The header does not lie.
 * `python3 tools/shimcount.py` emits three rows in the whole tree and none of
 * them name this file: PIN-FREE confirmed.
 *
 * THE NEW LEVER, AND ITS DIRECTION.  Batch 310c closed the shared blocker of
 * Anim_Djinni and Anim_CriticalHit -- a constant that the ROM rematerialises at
 * each use while we held it in a callee-saved register -- with three tokens:
 *
 *     int clen = 0x80 << 7;     <-- INITIALISED AT ITS DECLARATION
 *
 * > AN INT CARRIER INITIALISED AT ITS DECLARATION MAKES ITS PSEUDO LIVE FROM
 * > FUNCTION ENTRY, SO allocno_compare's log2(n_refs) * freq / LIVE_LENGTH PUTS
 * > IT LAST.  It is allocated last, loses its hard register, and because its
 * > REG_EQUIV is a constant reload REMATERIALISES it instead of spilling it.
 * > A BODY ASSIGNMENT DOES THE OPPOSITE -- it keeps the range short and the
 * > priority high.  Measured both ways on Anim_Djinni: declaration initialiser
 * > 26 -> 16 with the relocations becoming exact, body assignment inert at 26.
 * > Declaration RANK is free; the constant's SPELLING is free.
 *
 * WHY IT DOES NOT APPLY HERE, stated so it is not retried.  The lever LOWERS a
 * quantity's priority.  The quantity that must lose here -- `frame` -- is
 * ALREADY whole-function-lived, which is already the longest range and the
 * lowest priority available, and it still wins a register because there is one
 * free when its turn comes.  There is nothing left to lower.
 *
 * WHAT THIS FUNCTION STILL NEEDS IS THE COMPLEMENT: RAISE A COMPETITOR SO THE
 * LOOP WALKER IS PUSHED OFF ITS LOW CALLEE-SAVED REGISTER.  The recorded
 * causality is that the ROM's walker sits HIGH, every `ldr rX,[walker,#imm]`
 * then needs a LOW base, reload manufactures one copy per iteration, no low
 * callee-saved register is free to be that scratch, and so reload spills the
 * lowest-priority allocno -- the victim -- which is where its reloads and the
 * missing frame word come from.  find_reg walks REG_ALLOC_ORDER low-first, so
 * our walker takes the low register and the cycle never starts.  To reach it
 * from source, a SHORT-RANGE, MANY-REFERENCE quantity must claim that low
 * register before the walker's turn -- which is region-splitting a competitor
 * (one variable per region), NOT reuse, and NOT the carrier.  The recorded
 * caution is live: counter-splitting on Anim_Frost measured 466 -> 388, the
 * complement applied where it does not belong, and partition splits are NOT
 * ADDITIVE, so a whole partition must be applied before anything is concluded.
 *
 * THE OTHER HALF OF THE PAIR IS UNCHANGED AND STILL THE REASON TO BELIEVE THIS
 * IS A CLASS: Anim_Frost, which spills `base` instead has the same one-allocno rotation with a DIFFERENT
 * VICTIM, which is what shows the spilled variable is whoever is left over
 * rather than any particular named local -- so no per-variable spelling reaches
 * it, and the open item remains REG_ALLOC_ORDER.
 *
 * ================================================================
 * SPLIT SHAPE -- THREE-WAY, EIGHT NEW EXPORTS, NONE OF THEM THIS FUNCTION'S
 * ================================================================
 *
 * asm/rom_c9000/rom_e7320_c_c.s holds EIGHT functions -- Func_80e7338,
 * Func_80e73a0, BaseAnim_Meteor, Anim_Ramses, Anim_DragonCloud (FIFTH),
 * Anim_Annihilation, Anim_Ragnarok, Anim_TitanBlade -- and a .rodata tail.
 * `tools/split_s.py --dry-run asm/rom_c9000/rom_e7320_c_c.s Anim_DragonCloud`
 * gives:
 *
 *   asm/rom_c9000/rom_e7320_c_c_a.s   Func_80e7338, Func_80e73a0,
 *                                     BaseAnim_Meteor, Anim_Ramses
 *   src/rom_c9000/rom_e7320_c_c_b.c   THIS FUNCTION
 *   asm/rom_c9000/rom_e7320_c_c_c.s   Anim_Annihilation, Anim_Ragnarok,
 *                                     Anim_TitanBlade + ALL the .rodata
 *
 * tools/datacheck.py says Anim_DragonCloud READS NO DATA LABEL, so it needs no
 * `.global` of its own.  The EIGHT the split needs all belong to the two
 * SIBLINGS the split pushes into `_a.s` while their tables stay in `_c.s`:
 *
 *     .global .Leee76   .global .Leeea0   .global .Leeebc   .global .Leeeca
 *         (BaseAnim_Meteor)
 *     .global .Leeed8   .global .Leeee1   .global .Leeeea   .global .Leeef8
 *         (Anim_Ramses)
 *
 * A `.global` emits no bytes: add all eight, prove `make compare` green with
 * only that change, then split and prove it green again, then write the .c.
 * Everything this file names is already a global -- Data_ede48 / Data_ede9f /
 * Data_edea5 / Data_edeab / Data_edeb2 are `.incdata` in
 * asm/rom_c9000/rom_eda78.s:31-37, and `.incdata` expands to `.global \sym`
 * (include/macros.inc:46).
 *
 * NOTE FOR WHOEVER LANDS Anim_TitanBlade (the other unattempted function in
 * this file): TitanBlade is the LAST of the eight, so its split is two-way and
 * needs `.global .Leef12` and `.global .Leef18` -- its own tables, not a
 * sibling's.  Anim_Ragnarok needs `.Leef06` and `.Leef0c`.  Do the exports once
 * for all four functions and the later splits cost nothing.
 *
 * ================================================================
 * THE PROGRAM
 * ================================================================
 *
 * Read with the file's own park, src/non_matching/rom_c9000/Anim_Annihilation.c,
 * as the type oracle -- same original file, so the `State` / `Part` layouts, the
 * iwram_3001eec opening and the extern set transfer verbatim.
 *
 * A palette/tile ramp builder, then three particle seed loops, then a 0x96-frame
 * main loop.  Two shapes are worth naming:
 *
 *  - THE OPENING IS A NESTED BYTE LOOP, 19 rows of 0x3a8 bytes, that reads a
 *    fixed source run at base+0x1680 and writes `v - row*4 + 0x28` clamped at 0
 *    into base + row*0x3a8 - 0xe10, and ONLY for rows above 10.  The store and
 *    the `dst++` are on opposite sides of that `if`, which is why the copy is
 *    written with `dst++` outside the guard.  gcc holds -0xe10 and 0x1680 in
 *    `lr` and `ip` -- with no call in the nest those are ordinary allocatable
 *    registers, so do not read them as a pin or a shim.
 *  - `int trail[20][2]` at sp+0x44 is a 20-entry position trail shifted down by
 *    one every frame (`trail[j] = trail[j-1]` for j from 0x13 to 1, each entry
 *    blitted as it moves), with trail[0] written from the projected dragon
 *    position.  The array is the FIRST-declared aggregate because it sits at the
 *    TOP of the frame (0x44..0xe3), ahead of `vec3_t v` (0x38) and
 *    `DrawFn fns[2]` (0x30) -- Anim_Fireball's declaration-order-IS-the-frame-map
 *    rule, and getting it wrong moves every stack reference in the function.
 *
 * ================================================================
 * LEVERS THAT PAID, WITH FIGURES
 * ================================================================
 *
 * (1) `dst` DECLARED BEFORE `src` in the byte-copy loop: 684 -> 682, aligned
 *     530 -> 536.  loop.c hoists the two constants into the preheader in the
 *     order their uses appear, and the ROM's preheader materialises -0xe10
 *     (dst's) before 0x1680 (src's); with src declared first the two pointers
 *     swap r1 and r2 for the whole nest and the pool loads swap with them.
 * (2) `void *view = iwram_3001e80;` as the FIRST statement of the main loop
 *     body -- the lever that carried this batch's Anim_Unused_Fizz from 630 to
 *     139.  The ROM loads it before the frame tests and reuses it across
 *     _Func_80bd7dc, _PlaySound and InitMatrixStack, which a global's load can
 *     never do (a call clobbers memory); only a pseudo can.  Present from the
 *     first candidate here.
 * (3) `State **slot` assigned ONCE, immediately before the main loop, while
 *     every use BEFORE the loop re-derives `*(State **)(base + 0x7828)`.  The
 *     ROM is explicit: `ldr r3,=0x7828 / add r3,r9 / ldr r3,[r3]` at each of the
 *     five pre-loop sites and `ldr r2,[sp,#0xc] / ldr r3,[r2]` at every site
 *     inside it.
 * (4) `sin(ang) * 24 >> 16`, written as a multiply.  gcc expands *24 as
 *     `lsl r3,r0,#1 / add r3,r0 / lsl r3,#3`, which is exactly the ROM's three
 *     instructions -- do not hand-distribute it into shifts here (contrast
 *     batch 298's DISTRIBUTE THE SHIFT BY HAND, which applies when the ROM does
 *     NOT fold; this ROM does).
 * (5) `w = 9 - (v.z - 0xa0) / 64` with `Data_ede48[w - 1]` and `w * 2` last,
 *     computed UNCONDITIONALLY before the `i > 0x2f` test even though only the
 *     else-arm uses it: the ROM computes r4 before the branch, so it is a
 *     declared local of the outer block.
 * (6) `p->f0 - ((w2 = Data_ede9f[u]) >> 1)` as an embedded assignment in the
 *     final blit, `unsigned char w2`, which is what orders the pool
 *     Data_edeb2 / Data_ede9f / Data_edeab / Data_edea5 the ROM's way.
 *
 * MEASURED INERT, so the arm order of the seed loops' `f4` test is NOT the
 * residue: `if (f4 != 0) z = 0x380000; else z = -0x380000;` and the ternary
 * `p->f0 = f4 == 0 ? -0x380000 : 0x380000;` are BOTH byte-identical to the
 * if/else in this file (682 all three).  gcc hoists the pool load above the
 * test and corrects it in one arm regardless of spelling, where the ROM keeps
 * two real arms joining on one `str`.  Recorded because the shape LOOKS like
 * fold's ternary inversion (batch 305's Func_80f62b8 lever) and is not.
 *
 * ================================================================
 * THE BLOCKER: `frame` MUST BE SPILLED AND IS NOT -- THE SAME ONE ALLOCNO AS
 * THIS BATCH'S Anim_Frost
 * ================================================================
 *
 * The ROM SPILLS `frame` to sp+0x28 and reloads it about twenty-five times; we
 * keep it in r11 for the whole function.  That one fact produces every figure:
 *
 *   - the frame.  `sub sp,#0xe0` against the ROM's `#0xe4`: exactly one word,
 *     and it is frame's own slot.  The ROM's spilled scalars descend
 *     ctx 0x2c, frame 0x28, tilt 0x24, cur 0x20, px 0x1c, py 0x18, base2 0x14,
 *     fp 0x10, slot 0xc, d 0x8; ours has ctx at 0x28 and everything below it
 *     shifted up by four, with frame in a register instead.
 *   - the SIZE and COUNT deficit: about 25 `ldr rX,[sp,#0x28]` reloads the ROM
 *     pays and we do not, against our `mov rX,fp`, is the -7 count and most of
 *     the -16 bytes.  There is no missing work: both objects make the same 6
 *     Random, 2 sin, 1 cos, 1 __divsi3, 2 Func_80d6888 and 6 _call_via_r4
 *     calls, and the relocation symbol SEQUENCE matches.
 *   - 128 hunks of rotation.  With frame holding r11, the ROM's
 *     (view r11, per-region r10/r8) becomes ours (view r10, frame r11), and the
 *     low file rotates with it -- every seed loop reads
 *     ROM (p r5, i r6, mask r7) against ours (p r6, i r7, mask r5), with the
 *     ROLES already correct and only the NAMES wrong.  That is batch 301's
 *     "a whole-function high-register rotation is ONE EXTRA ALLOCNO".
 *
 * THIS IS THE SAME BLOCKER AS Anim_Frost IN THIS BATCH, with a different
 * variable in the r11 seat (`base` there, `frame` here), and the same paradox:
 * the ROM's allocation is measurably WORSE than gcc's -- it pays a reload at
 * every one of twenty-five sites for a value gcc keeps in a register -- so the
 * allocator was forced into it and ours is not.  Both are clean instances of
 * the REG_ALLOC_ORDER class HANDOFF.md names as the corpus's top open item, and
 * having two of them in one file family with different victims is the useful
 * new datum: the victim is whichever whole-function-lived quantity is left over
 * when the per-loop allocnos have taken the seven call-saved registers, so the
 * fix is not variable-specific and no per-variable spelling will find it.
 *
 * WHAT IS RULED OUT HERE
 *   - moving `frame` down the declaration list: the ROM's own frame map puts
 *     its slot SECOND from the top of the scalars, so frame must be declared
 *     second and cannot be demoted.
 *   - the fifteen-flag sweep run against Anim_Frost's identical residue
 *     (-fno-gcse, -fno-rerun-cse-after-loop, -fno-strength-reduce,
 *     -fno-schedule-insns2, -fno-cse-follow-jumps,
 *     -fno-expensive-optimizations, -fno-force-mem, -fno-caller-saves,
 *     -fno-thread-jumps, -fno-delete-null-pointer-checks,
 *     -fno-optimize-sibling-calls, -fno-peephole, -fno-function-cse,
 *     -fno-cse-skip-blocks): none moved the victim out of r11.  Re-run it here
 *     before believing a flag row exists, but do not expect one.
 *   - sched1, which does not run in this build, and sched2, which cannot change
 *     a register assignment.
 *
 * THE ONE THING WORTH TRYING NEXT that was not tried: this function has a
 * SECOND whole-function quantity in a high register, `base` in r9, which the
 * ROM ALSO keeps in r9.  So DragonCloud is the cleaner experiment of the pair --
 * the ROM proves one such variable can win r9 while another loses r11, which
 * means the discriminator is visible in allocno_compare's three inputs
 * (n_refs, live_length, declaration order) for base against frame.  Dump
 * .18.greg for both and compare those three numbers directly; that is a
 * measurement nobody in the corpus has taken, and it would either name the
 * source-level knob or retire the REG_ALLOC_ORDER hypothesis for this class.
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
    int f0, f4, f8, fc, f10, f14, f18;
} Unit;

extern void *iwram_3001eec[];
extern void *iwram_3001e80;
extern Unit gBuffer[];
extern unsigned short Data_ede48[];
extern unsigned char  Data_ede9f[];
extern unsigned char  Data_edea5[];
extern unsigned char  Data_edeab[];
extern unsigned short Data_edeb2[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int file, void *dst, int a, int b);
extern void BuildDraw2DFuncs(int a, void **fns);
extern int  Random(void);
extern int  sin(int a);
extern int  cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void _Func_80c0cec(int a, int b, int c, int d);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int  Func_80e3944(vec3_t *in, vec3_t *out);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_DragonCloud(void *context)
{
    int trail[20][2];
    vec3_t v;
    DrawFn fns[2];
    void *ctx;
    int frame;
    int tilt;
    signed char *cur;
    int px;
    int py;
    void *base2;
    DrawFn *fp;
    State **slot;
    int d;
    unsigned char *base;
    void **gp;
    void **pp;
    Unit *p;
    int i;
    int j;
    int ang;

    gp = iwram_3001eec;
    pp = gp;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    tilt = 0;
    px = 0;
    py = 0;
    base2 = gp[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BLDALPHA = 0x1010;
    LoadVFXFile(FILE_c2, base, 1, 1);
    i = 1;
    do {
        unsigned char *dst = base + i * 0x3a8 - 0xe10;
        unsigned char *src = base + 0x1680;
        j = 0;
        do {
            int t = *src++;
            if (i > 0xa) {
                t = t - i * 4 + 0x28;
                if (t < 0) {
                    t = 0;
                }
                *dst = t;
            }
            j++;
            dst++;
        } while (j != 0x3a8);
        i++;
    } while (i != 0x14);
    LoadVFXFile(FILE_73, base2, 0, 0);
    LoadVFXFile(FILE_b4, base + 0x3c00, 1, 1);
    LoadVFXFile(FILE_7d, gBuffer, 1, 0);
    fp = fns;
    BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, (void **)fp);
    *(int *)(base + 0x7780) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x480);
    p = (Unit *)(base + 0x7160);
    i = 0;
    do {
        int z;
        if ((*(State **)(base + 0x7828))->f4 == 0) {
            z = -0x380000;
        } else {
            z = 0x380000;
        }
        p->f0 = z;
        p->f4 = 0;
        p->f8 = 0;
        p->fc = ((Random() & 0x3f) - 0x20) << 14;
        p->f10 = (Random() & 0x3f) << 13;
        p->f14 = ((Random() & 0x3f) - 0x20) << 14;
        p->f18 = 1;
        i++;
        p++;
    } while (i != 0x28);
    p = (Unit *)(base + 0x75c0);
    i = 0;
    do {
        int z;
        if ((*(State **)(base + 0x7828))->f4 == 0) {
            z = -0x380000;
        } else {
            z = 0x380000;
        }
        p->f0 = z;
        p->f4 = 0x140000;
        p->f8 = 0;
        p->fc = ((Random() & 0x3f) - 0x20) << 14;
        p->f10 = (Random() & 0x3f) << 12;
        p->f14 = ((Random() & 0x3f) - 0x20) << 14;
        p->f18 = 0;
        i++;
        p++;
    } while (i != 0x10);
    p = (Unit *)(base + 0x7080);
    ang = -0x4000;
    i = 0;
    do {
        if ((*(State **)(base + 0x7828))->f4 == 1) {
            p->f0 = (sin(ang) * 24 >> 16) + 0x58;
        } else {
            p->f0 = (-(sin(ang) * 24) >> 16) + 0x10;
        }
        p->f4 = (cos(ang) * 16 >> 16) + 0x28;
        p->f18 = -(i * 2);
        ang += 0x1000;
        i++;
        p++;
    } while (i != 8);
    cur = (signed char *)GetFile(FILE_d3);
    frame = 0;
    slot = (State **)(base + 0x7828);
    do {
        void *view = iwram_3001e80;
        if (frame == 0x53) {
            _Func_80bd7dc(0x86);
        }
        if (frame == 0) {
            _PlaySound(0x88);
        }
        if (frame == 0x32) {
            _PlaySound(0x88);
        }
        if ((*slot)->f4 == 0) {
            if (frame <= 0x3f) {
                *(unsigned short *)((char *)view + 0x36) += -0x100;
            }
        } else {
            if (frame <= 0x3f) {
                *(unsigned short *)((char *)view + 0x36) += 0x100;
            }
        }
        _Func_80c0cec(0, 0, 0, 0x64);
        if (frame <= 0x11) {
            int k = frame / 3;
            fns[0]((void *)ctx, base + 0x3c00 + Data_edeb2[k], 0x30,
                   Data_edeab[k] + 0x3c, Data_ede9f[k], Data_edea5[k]);
            fp[1]((void *)ctx, base + 0x3c00 + Data_edeb2[k], 0x38,
                  Data_edeab[k] + 0x3c, Data_ede9f[k], Data_edea5[k]);
        }
        d = frame - 0x12;
        if ((unsigned)d <= 0x28) {
            if (frame == 0x12) {
                px = (cur[0] << 8) + (unsigned char)cur[1];
                py = (cur[2] << 8) + (unsigned char)cur[3] + 0x10;
                cur += 4;
            } else {
                px += cur[0];
                py += cur[1];
                cur += 2;
            }
        }
        if ((unsigned)(frame - 0x4e) <= 0x28) {
            if (frame == 0x4e) {
                px = -0x38;
                py = 0x30;
            } else {
                py -= 0x10;
            }
        }
        j = 0x13;
        do {
            if (frame > j + 0x12 && frame <= j + 0x53) {
                int ty;
                trail[j][0] = trail[j - 1][0];
                ty = trail[j - 1][1];
                trail[j][1] = ty;
                if (j > 0xa) {
                    fns[0]((void *)ctx, base + j * 0x3a8 - 0xe10,
                           trail[j][0], ty, 0x18, 0x27);
                } else {
                    fns[0]((void *)ctx, base + 0x1680,
                           trail[j][0], ty, 0x18, 0x27);
                }
            }
            j--;
        } while (j != 0);
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        if ((unsigned)d <= 0x41) {
            int m;
            if ((*slot)->f4 == 1) {
                v.x = 0x40 - px / 2;
            } else {
                v.x = px / 2 + 0x40;
            }
            v.y = 0x3c - py;
            m = (v.y - trail[0][1] - 0x18) / 2;
            if (m > 2) {
                m = 2;
            }
            if (m < -2) {
                m = -2;
            }
            tilt += m;
            if (tilt > 8) {
                tilt = 8;
            }
            if (tilt < -8) {
                tilt = -8;
            }
            m = tilt / 4 + 2;
            trail[0][0] = v.x - 0xc;
            trail[0][1] = v.y - 0x14;
            fns[0]((void *)ctx, base + m * 0x480, v.x - 0x12, v.y - 0x16,
                   0x18, 0x30);
        }
        if (frame == 0x53) {
            *(int *)(base + 0x77a8) = 8;
            Func_80d6888((*slot)->ids[0], 7, 5, 0, 8);
            _SetBattleActorKnockback((*slot)->ids[0], 1);
        }
        if (frame > 0x53) {
            p = (Unit *)(base + 0x7160);
            i = 0;
            do {
                if (p->f4 >= 0) {
                    int w;
                    Func_80e3944((vec3_t *)p, &v);
                    v.x >>= 1;
                    if (v.z <= 0x9f) {
                        v.z = 0xa0;
                    }
                    if (v.z > 0x31f) {
                        v.z = 0x31f;
                    }
                    w = 9 - (v.z - 0xa0) / 64;
                    if (i > 0x2f) {
                        if (p->f18 <= 0xb) {
                            fns[0]((void *)ctx,
                                   (char *)gBuffer + ((p->f18 / 2) << 11),
                                   v.x - 0x10, v.y - 0x20, 0x20, 0x40);
                            p->f18 = p->f18 + 1;
                        }
                    } else {
                        fns[0]((void *)ctx,
                               (char *)base2 + Data_ede48[w - 1],
                               v.x - w / 2, v.y - w, w, w * 2);
                    }
                    p->f0 += p->fc;
                    p->f4 += p->f10;
                    p->f8 += p->f14;
                    p->f10 += -0x2000;
                }
                i++;
                p++;
            } while (i != 0x38);
        }
        if (frame == 0x32) {
            *(int *)(base + 0x77a8) = 0xc;
            Func_80d6888((*slot)->ids[0], 7, 5, 0, 8);
        }
        if (frame > 0x31) {
            p = (Unit *)(base + 0x7080);
            i = 0;
            do {
                int t = p->f18;
                if ((unsigned)t <= 0xb) {
                    int u = t / 2;
                    unsigned char w2;
                    fp[1]((void *)ctx, base + 0x3c00 + Data_edeb2[u],
                          p->f0 - ((w2 = Data_ede9f[u]) >> 1),
                          p->f4 + Data_edeab[u], w2, Data_edea5[u]);
                    t = p->f18;
                }
                p->f18 = t + 1;
                i++;
                p++;
            } while (i != 8);
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x96);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
