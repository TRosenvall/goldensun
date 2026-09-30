/* Anim_Ray -- PARKED.  0x080db264, 464 instructions in the ROM listing.
 * NON-MATCHING, 294 of 495 encodings differ.
 * SIZE  ref 1124 bytes, ours 1128 (+4).  COUNT ref 495, ours 497 (+2).
 * tools/aligncmp.py: aligned-equal 455 of 495 (91.9%), 47 differing in 31 hunks.
 * SHIMS: none.  `python3 tools/shimcount.py` is silent -- no register pins, no
 * `.equ`, no `asm volatile`, no per-file Makefile flag override.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/Anim_Ray.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c.s --func Anim_Ray
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_c9000/Anim_Ray.c \
 *     asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c.s Anim_Ray -v
 *
 * SPLIT SHAPE.  asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c.s holds FOUR functions --
 * Anim_Quake, Anim_Fireball, Anim_Frost, Anim_Ray -- and Anim_Ray is the FOURTH
 * and last, so landing it is a two-way split of the stem:
 *
 *   asm/rom_c9000/rom_d9ab8_c_c_c_c_c_a.s   Quake + Fireball + Frost + ALL .rodata
 *   src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c   THIS FILE
 *
 * EXPORTS: ONE NEW.  `python3 tools/datacheck.py asm/rom_c9000/rom_d9ab8_c_c_c_c_c_c.s`
 * reports a .rodata section whose already-global labels are `.Leea08`, `.Leea20`
 * and `.Leea2c` -- none of them Anim_Ray's.  Anim_Ray reads `.Leeadc`
 * (`.incrom 0xeeadc, 0xeeae2`, six bytes, indexed `[f18 * 2 + 1]`), so the asm
 * piece must gain `.global .Leeadc`; this file declares it
 * `extern unsigned char Leeadc[] __asm__(".Leeadc")`.  The data stays in the asm
 * object, so objcmp's SIZE line carries no .rodata-from-C false positive.
 *
 * ================================================================
 * WHAT CLOSED IT, BY PASS -- 274 -> 263 -> 263(alignment) -> 296 -> 294
 * ================================================================
 *
 * Pass 1 (ray1.c) already read SIZE and COUNT both EXACT (495/495, 274 of 495)
 * and a byte-identical RELOCATION SEQUENCE, from four oracles taken off the
 * landed siblings before a line was written:
 *   - src/rom_c9000/rom_d9ab8_c_c_c_c_a_b.c (Anim_Flare), the other half of this
 *     same rom_d9ab8 family, for the iwram_3001eec `ldmia r3!, {r1}` idiom, the
 *     declaration-list-IS-the-frame-map rule, and the `while` form of the
 *     Func_80d6888 lane loop;
 *   - src/rom_c9000/rom_d82b0_b.c (Anim_Break) for the gBuffer/Data_ede48 blit
 *     (`n = t/8 + 1; w2 = n*2; src = gfx + Data_ede48[n-1]`) and for
 *     "a counter reused across several loops is ONE variable";
 *   - src/rom_c9000/rom_e0564_a_b.c (Anim_Hail) for the bare-literal rule and
 *     the preheader-transposition tell;
 *   - src/non_matching/rom_c9000/80ecef4.c for `ClearFn fill = Func_80008d8;`
 *     as a block-local, which is what makes the ROM's `bl _call_via_r3`.
 *
 * Pass 2 (ray2.c), 274 -> 263.  THREE edits, all read off the frame map:
 *   - `frame << 11` must NOT be a declared local.  The ROM spills it at sp+0x8,
 *     BELOW the compiler temp holding `&fns[0]` at sp+0xc; a declared local took
 *     the higher slot and swapped the pair.  Written inline inside the lane loop,
 *     loop.c hoists it and the pseudo lands in the lowest slot, as Anim_Break's
 *     "read a spill slot below a compiler temp as evidence that value is a giv"
 *     predicts.  The `+ 0x4000` is NOT hoisted with it -- the ROM rebuilds
 *     `mov r1,#0x80 / lsl r1,#7` inside the loop, which is loop.c refusing to
 *     move a two-insn constant.
 *   - `(a0 + 0x4000) * lane`, not `lane * (a0 + 0x4000)`.  THE THUMB `mul` SEEDS
 *     ITS DESTINATION FROM THE RIGHT OPERAND.  Same rule as
 *     `sin(ang) * mag` and `sin(ang) * (0x20 - frame)` two lines later; all three
 *     sites in this function agree, so the rule is "right operand becomes the
 *     destination", stated without reference to which side holds a call.
 *   - The blit source must be NAMED before the call, not written inline in
 *     argument 2.  The ROM computes `base + 0x60e + r*0xb40` COMPLETELY, then
 *     calls Random again for the x jitter (24 bytes between the two
 *     R_ARM_THM_CALL Random relocations; inline it is 10) and finishes with
 *     `mov r1, r5`.  An inline argument computes straight into r1 and there is no
 *     `mov`.
 *
 * Pass 3 (ray3.c), aligned-equal 395 -> 409 with the count unchanged.  THE THREE
 * gBuffer WALKERS ARE THREE VARIABLES.  The ROM keeps the init loop's and the
 * spark loop's walker in r7 and the frame-draw loop's in r5; one `Part *g` for
 * all three gave the draw loop r7 and shifted every low register in it, plus the
 * `movs r6,#0x24 / movs r5,#4` givs of the Func_80d6888 loop.  This is the
 * family's "pointers split, counters unify" pair -- the single `i` across all
 * four loops is correct and splitting it is not.
 *
 * Pass 4 (ray4.c), the big one: aligned-equal 409 -> 435 and THE WHOLE
 * r9/r10/r11 ROTATION CAME OUT.  The Func_80d6888 lane loop must be
 * `i = 0; while (i != (*(State **)(base + 0x7828))->f14) { ... }` and NOT
 * `i = 0; if (f14 != 0) { do ... while (...); }`.
 *
 * > IN ONE FUNCTION THE TWO GUARD FORMS ARE BOTH PRESENT AND THE ROM SAYS WHICH
 * > IS WHICH.  `duplicate_loop_exit_test` runs after gcse, so a `while` guard
 * > does not exist at gcse time and the loop-invariant address `base + 0x7828`
 * > CANNOT be hoisted; an explicit `if` guard does exist and gcse hoists it.
 * > Anim_Ray's Func_80d6888 loop re-derives the address three times with
 * > `ldr r2,=0x7828 / mov r1,r10 / ldr r3,[r1,r2]` -> `while`.  Anim_Prism's
 * > identical-looking loop CACHES it in r5 across the loop -> `if`.  Read the
 * > loop, not the family.
 *
 * And the cost of getting it wrong is not local: with the `if` form gcse's
 * hoisted address became a fourth high-register allocno, `base` fell from r10 to
 * r11, `X` from r9 to r10 and `yy` from r11 to r9, and 47 encodings in unrelated
 * blocks moved with them.  A WHOLE-FUNCTION HIGH-REGISTER ROTATION IS ONE
 * MISSING OR EXTRA ALLOCNO, NOT A REGISTER-ORDER PROBLEM.
 *
 * Pass 5 (rayI.c), aligned-equal 435 -> 451.  `(unsigned char)Random() & 7`
 * rather than `Random() & 7` at the two blit sites.  The cast is value-neutral
 * (the mask keeps three bits) but it changes the pseudo numbering enough to clear
 * the ENTIRE spark-loop residue -- the `cnt`/`Leeadc` tail at ref[310..333], 16
 * encodings, none of them the site edited.  Recorded as a MEASUREMENT, not a
 * mechanism: it was reached by probe, and three other value-neutral rewritings of
 * the same expression (`(Random() & 7) + X - 0x10`, `X - 0x10 + (Random() & 7)`,
 * a named `int j = Random() & 7`) were all BYTE-IDENTICAL to each other.
 *
 * Pass 6 (rayM1.c, this file), aligned-equal 451 -> 455.  The blit source's
 * strength-reduced chain must be tied into the source pointer's own quantity:
 *
 *     src = (u8 *)((Random() & 3) * 0xb40);
 *     src += (int)base;
 *     src += 0x60e;
 *
 * The ROM has `lsl r5,r3,#4 / sub r5,r5,r3 / lsl r5,r5,#6 / add r5,sl /
 * add r5,r5,r2` -- the *15 step is ALREADY in r5, the callee-saved register `src`
 * needs because it is live across the second Random call.  Written as one
 * expression the *15 lands in the caller-saved r3 and only the final add reaches
 * r5, which also pushes the *3 step from r3 to r2.  Three statements to ONE
 * pointer variable make the whole chain one local-alloc quantity.
 *
 * ================================================================
 * THE BLOCKER: +2 INSTRUCTIONS, ONE MECHANISM, TWO SITES
 * ================================================================
 *
 * Everything that is left is ONE residue repeated in the two arms of the
 * `f18 == 0` test, and it is the whole +2:
 *
 *     ROM   movs r2, #7 / ands r2, r0                 (2)
 *     ours  movs r3, #7 / adds r2, r0, #0 / ands r2, r3   (3)
 *
 * Thumb's `andsi3` is two-address with `%0` on operand 1, so the result can be
 * seeded either from the value or from the mask.  The ROM seeds from the MASK,
 * which requires the constant pseudo and the result pseudo to be ONE local-alloc
 * quantity in r2 (r2 is argument 3 of the blit, so the result quantity carries a
 * copy suggestion to it).  Ours allocates the constant r3 -- first in
 * REG_ALLOC_ORDER -- and reload then has neither operand in r2 and inserts the
 * copy.
 *
 * The same ROM does BOTH spellings within twenty instructions: `mov r3,#3 /
 * and r0,r3` (value-seeded) for `Random() & 3` four insns earlier, and
 * `mov r1,#1 / and r1,r3` (mask-seeded) for `lane & 1` in the second arm, which
 * WE ALREADY REPRODUCE.  The discriminator is that `lane` arrives from a stack
 * slot as a fresh pseudo while `Random()`'s result arrives in hard r0 as a
 * suggested quantity; ours therefore combines the result with r0's quantity and
 * the ROM did not.
 *
 * NINE SPELLINGS MEASURED, NONE CLOSES IT (all still 497 encodings):
 *   X + (Random() & 7) - 0x10                   455   <- this file
 *   X + ((unsigned char)Random() & 7) - 0x10    451 with the one-expression src
 *   (Random() & 7) + X - 0x10                   435  (byte-identical to the above)
 *   X - 0x10 + (Random() & 7)                   435  (byte-identical)
 *   X + ((unsigned)Random() & 7u) - 0x10        435  (byte-identical)
 *   int j = Random() & 7;                       436
 *   int j = 7; j &= Random();                   432  -- DOES produce the ROM's
 *       two-instruction form (`mov r5,#7 / and r5,r5,r0`), because reusing a
 *       variable as its own accumulator picks the tied side, BUT `j = 7` is then
 *       born before the call, the quantity becomes callee-saved r5, and the copy
 *       reappears as `mov r2,r5` at the argument.  Net zero, and it costs `src`
 *       and `Y` their registers.
 *   int rnd = Random(); int j = 7; j &= rnd;    367
 *   the same as a four-step accumulator (j &= rnd; j += X; j -= 0x10)  441
 *   unsigned char rb = Random(); (rb & 7)       414
 *
 * So the accumulator lever REACHES the tie and cannot reach it without also
 * lengthening the constant's live range across the call.  docs/elevation.md
 * ~3463 lists "which operand of a commutative and/orr becomes the destination"
 * under NOT EVIDENCE, and ~22388 records that no flag disables the
 * canonicalisation.  This park is that class, with the added datum that the
 * accumulator spelling does flip the tie and is still not enough because the
 * operand is a CALL RETURN.
 *
 * TWO SMALLER, COUNT-NEUTRAL RESIDUES, both transpositions:
 *   - `lsls r3,r3,#16` and `mov fp,r3` are one position apart from the ROM's in
 *     the spark loop's preheader (`yy = (Y + 0x70) << 16; i = 0; g = gBuffer;`).
 *   - `ldr r0,=Task_BlitAnim` is one insn early before StartTask.
 * Both are birth-order, not role: every register in them is already right.
 *
 * NOT A RESIDUE, recorded so it is not chased: the ROM pools 0x80 and 0x100 as
 * WORDS for the two `REG_BG2PA` writes (`ldr r3, .Ldb2a0 @ 0x80`).  gcc emits
 * `ldrh r3, .L3` for a HImode volatile store of a constant and gas assembles
 * Thumb `ldrh <pool-label>` to the same halfword as `ldr` -- the third time this
 * has been recorded (docs/elevation.md, batch 278).  Both arms load the address
 * separately and cross-jump only the `strh`, which is what two separate
 * `REG_BG2PA = k;` statements give.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*ClearFn)(void *dst, int len, int val);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern int *iwram_3001eec[];
extern void *gPtrs[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern unsigned char Leeadc[] __asm__(".Leeadc");

extern void Func_80cdb24(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void Func_80008d8(void *dst, int len, int val);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
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

void Anim_Ray(void *context)
{
    DrawFn fns[2];
    void *ctx;
    int frame;
    u8 *gfx;
    int lane;
    int cnt;
    char **tbl;
    char **pp;
    u8 *base;
    State **slot;
    Part *p;
    Part *q;
    Part *d;
    int i;


    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    gfx = (u8 *)tbl[2];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    Func_80cdb24(1);
    if ((*slot)->f18 == 2) {
        REG_BG2PA = 0x80;
    } else {
        REG_BG2PA = 0x100;
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    fns[0] = (DrawFn)gPtrs[0xb8 / 4];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, 3);
    fns[1] = (DrawFn)gPtrs[0xbc / 4];
    LoadVFXFile(FILE_c4, base + 0x60e, 1, 1);
    LoadVFXFile(FILE_73, gfx, 0, 0);
    if ((*(State **)(base + 0x7828))->f18 == 2) {
        if ((*(State **)(base + 0x7828))->f4 == 1) {
            REG_BG2X = -0x1000;
        } else {
            REG_BG2X = 0x80 << 5;
        }
    } else if ((*(State **)(base + 0x7828))->f4 == 1) {
        REG_BG2X = -0x8000;
    }
    i = 0;
    p = gBuffer;
    do {
        int mag = (Random() & 0x3ff) + (0x80 << 1);
        int ang = (Random() & 0x7fff) - 0x4000;
        p->x = 0x80 << 7;
        p->y = 0xe0 << 7;
        p->z = (sin(ang) * mag) >> 16;
        p->vz = -(cos(ang) * mag * 2) >> 16;
        p->t = 0;
        i++;
        p++;
    } while (i != (0x80 << 3));
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    StartTask(Task_BlitAnim, 0x90 << 3);
    _PlaySound(0x8a);
    frame = 0;
    do {
        if (frame == 0x14) {
            _Func_80bd7dc(0x85);
        }
        if (frame <= 0xf) {
            if (frame % 5 == 2) {
                ClearFn fill = Func_80008d8;
                fill(ctx, 0x80 << 7, 0x10101010);
            }
            lane = 0;
            do {
                int ang;
                int X;
                int Y;
                int yy;
                u8 *src;

                cnt = 0;
                ang = ((frame << 11) + (0x80 << 7)) * lane;
                X = ((sin(ang) * (0x20 - frame)) >> 16) + 0x40;
                Y = -((cos(ang) * 8) >> 16) - 8;
                if ((*(State **)(base + 0x7828))->f18 == 0) {
                    src = (u8 *)((Random() & 3) * 0xb40);
                    src += (int)base;
                    src += 0x60e;
                    fns[0](ctx, src, X + ((unsigned char)Random() & 7) - 0x10, Y, 0x18, 0x78);
                } else {
                    src = (u8 *)((Random() & 3) * 0xb40);
                    src += (int)base;
                    src += 0x60e;
                    fns[lane & 1](ctx, src, X + ((unsigned char)Random() & 7) - 0x10, Y, 0x18, 0x78);
                }
                yy = (Y + 0x70) << 16;
                i = 0;
                q = gBuffer;
                do {
                    if (q->t == 0) {
                        int m = (Random() & 0x1ff) + 0x80;
                        int a2 = (Random() & 0x7fff) - 0x4000;
                        q->x = X << 16;
                        q->y = yy;
                        q->vx = (sin(a2) * m) >> 9;
                        q->vy = -(cos(a2) * m * 2) >> 7;
                        q->t = (Random() & 7) + 0x20;
                        cnt++;
                        if (cnt == Leeadc[(*(State **)(base + 0x7828))->f18 * 2 + 1]) {
                            break;
                        }
                    }
                    i++;
                    q++;
                } while (i != (0x80 << 3));
                lane++;
            } while (lane != 4);
            *(int *)(base + 0x77a8) = 1;
        }
        i = 0;
        d = gBuffer;
        do {
            if (d->t > 0) {
                d->t -= 1;
                Func_80e3908(d, 0x3c, -0x800);
                if (d->y > (0xf0 << 15)) {
                    d->vy = -d->vy / 2;
                } else if ((unsigned)d->x <= 0x7effff && d->y >= 0) {
                    int px = d->x >> 16;
                    int py = d->y >> 16;
                    int n = d->t / 8 + 1;
                    int w2 = n * 2;
                    fns[0](ctx, gfx + Data_ede48[n - 1], px - n / 2, py - n, n, w2);
                }
            }
            i++;
            d++;
        } while (i != (0x80 << 3));
        if ((unsigned)(frame - 4) <= 0x5b) {
            i = 0;
            while (i != (*(State **)(base + 0x7828))->f14) {
                if (frame == i * 4 + 4) {
                    Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 0xa);
                }
                i++;
            }
        }
        UpdateScreenShake(2, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x40);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
