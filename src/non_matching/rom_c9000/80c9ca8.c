/* BaseAnim_SonicWave -- 0x080c9ca8, 580 instructions.  PARKED, FIRST OPENING.
 *
 * NON-MATCHING: 565 encodings of 607 differ (objcmp).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/80c9ca8.c asm/rom_c9000/rom_c91dc_c_c_c_c_c_c.s --whole
 *
 * READ THE COUNT FIRST, NOT THE 565.  ours 550 instructions against the ROM's 580, so
 * the streams are NOT aligned and 565 IS NOT A DISTANCE -- 30 missing instructions
 * desynchronise everything after the first gap.  The honest number is the aligned one:
 * a difflib alignment over normalised instruction text puts 437 of 580 in a differing
 * group.  objcmp's SIZE line (ref 1386, ours 1280) likewise mixes the 30-instruction
 * shortfall with the 40 bytes of .rodata the reference carries and this candidate
 * declares extern.
 *
 * This is the least converged of the batch's three and the reason is structural, not
 * allocational: the ROM hoists FIVE strength-reduced givs into the actor loop's
 * preheader and this draft produces three of them.  See "THE BLOCKER" below.
 *
 * ================================================================
 * SPLIT SHAPE (asked for explicitly), AND WHY datacheck's EXPORTS LINE IS WRONG HERE
 * ================================================================
 *
 * rom_c91dc_c_c_c_c_c_c.s holds BaseAnim_SonicWave alone (`grep -ci func_start` = 1)
 * plus a `.section .rodata` tail of FOUR blobs, of which the first two are already
 * `.global` and the last two are not:
 *
 *     .global .Leded6   <- already exported; NOT read by this function
 *     .global .Lededc   <- already exported; NOT read by this function
 *     .Ledee8  .incrom 0xedee8, 0xedefc   20 bytes -- READ, needs `.global`
 *     .Ledefc  .incrom 0xedefc, 0xedf04    8 bytes -- READ, needs `.global`
 *
 * So the landing needs exactly two new lines, placed immediately before their labels:
 *
 *     .global .Ledee8
 *     .global .Ledefc
 *
 * tools/datacheck.py's EXPORTS line for this file names `.Leded6, .Lededc` -- BOTH
 * WRONG.  Those two are already global and this function never touches them; the two it
 * does touch are the two the tool does not name.  The EXPORTS line reports which labels
 * are already exported, not which ones a split would require, and this file is the
 * clearest example of the difference in the bank.  The check that works is a grep of the
 * function body for every `.L` referenced, followed by `grep -rn 'global \.<label>' asm/`.
 *
 * Widths, read off the ROM's own addressing:
 *   .Ledee8  `ldrsb r3, [r3, r0]`  -> SIGNED char, indexed variant*4 + {0,1,2,3}
 *            (5 variants x 4 flags = the 20 bytes exactly)
 *   .Ledefc  `ldrsb r3, [r1, r3]`  -> SIGNED char[8], indexed by a signed % 8
 *
 * ================================================================
 * THE BLOCKER: THE ACTOR LOOP'S giv SET (loop.c strength_reduce)
 * ================================================================
 *
 * The ROM's per-actor loop carries FIVE strength-reduced induction variables, all
 * initialised together in the preheader at .Lc9f20 and all living in stack slots -- the
 * SR-pseudo signature (lowest slots):
 *
 *     sp+0x14  io      = 0x24 + b*2      step +2     (the ids[] member-array offset)
 *     sp+0x10  w       = frame - 0x1e - b*0x20   step -0x20
 *     sp+0xc   b*0x20                    step +0x20
 *     sp+8     pi      = b*3             step +3
 *     sp+0x38  b                         step +1
 *
 * and the loop bottom at .Lca152 updates all five in one block (`add r3,#2 /
 * sub r4,#0x20 / add r0,#0x20 / add r1,#3 / add r2,#1` then five stores).  This draft
 * gets io, b*0x20 and b, but writes `frame - b*0x20 - 0x1e` and `b*3 + c` as ordinary
 * expressions, so gcc recomputes them instead of carrying them -- and because
 * `b * 0x20` then has only two uses instead of four, the whole preheader collapses and
 * the 30 instructions go missing there.  THE FIX IS A SOURCE SHAPE, NOT A LEVER: each
 * of the five has to appear as a plain `b`-linear expression that strength_reduce can
 * recognise, and `w` in particular has to be written so that `w` and `w % 8` are the
 * SAME giv (the ROM computes `w % 8` from sp+0x10 with the signed `asr #3 / lsl #3 /
 * sub` idiom, off the carried value, not off a fresh subtraction).
 *
 * ================================================================
 * FOUR THINGS ALREADY ESTABLISHED, KEEP THEM
 * ================================================================
 *
 * (1) THE FILE-ID SWITCH IS A JUMP TABLE AND IT NEEDS AN EXPLICIT `case 4`.  The ROM has
 *     `cmp r0,#4 / bhi default / ldr r3,=table / lsl r1,r0,#2 / ldr r3,[r1,r3] /
 *     mov pc,r3` over a FIVE-entry table whose last entry is the default's own block.  A
 *     switch with cases 0..3 plus a default compiles to a comparison TREE here (11
 *     instructions, and `mov pc` appears zero times); adding `case 4: file = FILE_a3;`
 *     beside the identical `default:` tips CASE_VALUES_THRESHOLD and produces the ROM's
 *     table -- gcc then merges case 4 into the default block by itself, which is why the
 *     table's fifth entry points at the default.  Confirmed: `grep -c 'mov.*pc' ` on the
 *     output goes 0 -> 1.
 *
 * (2) `nframes` NEEDS ITS MULTIPLY SPLIT ACROSS TWO STATEMENTS.  The ROM computes
 *     `mov r4,#3 / mov r2,r1 / mul r2,r4` -- a REAL `mul` by 3 -- and only then
 *     `lsl r3,r2,#1 / add r3,r2 / lsl r3,#1 / add r3,#0x30` for the *6.  Written as one
 *     expression (`f14 * 3 * 6 + 0x30`) fold collapses it to *18 and synth_mult emits
 *     `lsl #3` shifts, no `mul` at all.  Written as
 *         np = f14 * 3;
 *         nframes = np * 6 + 0x30;
 *     the `mul` comes back (confirmed: `grep mul` on the output goes empty -> `mul r5,
 *     r5, r3`).  MECHANISM: expand_mult's synth_mult picks `mul` for 3 because Thumb's
 *     `mul rd,rs` is one instruction and 3 needs two (`lsl`+`add`) -- but it never gets
 *     the chance while fold has already turned the product into 18, for which the shift
 *     chain wins.  THE READOUT IS GENERAL: a `mul` by a small constant in the ROM where
 *     gcc gives you shifts means the source had TWO multiplies, not one.
 *     Also note `mul r2, r4` with r2 = f14: operand 0 is tied to operand 1 in
 *     `*thumb_mulsi3`, so the destination register names the LEFT operand -- `f14 * 3`,
 *     not `3 * f14`.
 *
 * (3) THE TWO BuildDraw2DFuncEx ARMS ARE WRITTEN OUT IN FULL and gcc cross-jumps their
 *     common tail (`str r5,[sp] / bl BuildDraw2DFuncEx`) back into one block by itself,
 *     exactly as recorded on BuildDraw2DFuncs.  Only the fourth argument differs (0xb/3
 *     against 0xf/7); `fn0 = tbl[7]` sits inside both arms and `fn1 = tbl[8]` after.
 *
 * (4) THE PARTICLE ADDRESS FORM IS INERT, MEASURED THREE WAYS.  All of
 *         (Part *)(base + (0xe1 << 7) + a * 3 * 0x1c)
 *         (Part *)(base + (0xe1 << 7)) + a * 3
 *         (Part *)((int *)(base + (0xe1 << 7)) + a * 21)
 *         (Part *)((int *)(base + (0xe1 << 7)) + a * 3 * 7)
 *     give BYTE-IDENTICAL output -- fold normalises all four to `a * 84`, and gcc then
 *     loads 84 into a register and uses `mul`, which kills the giv (a `(mult reg reg)` is
 *     not a giv, only `(mult reg const)` is) and spills the particle pointer to a stack
 *     slot.  The ROM instead carries a giv stepping 21 with the use shifted `<<2`, and
 *     materialises the step as `mov r2,#3 / lsl r3,r2,#3 / sub r3,r2` -- i.e. 3*7
 *     computed at RUNTIME from a register holding 3, which is loop.c expanding a
 *     TWO-LEVEL giv's increment (inner step 3, outer multiplier 7).  So the ROM's inner
 *     `a * 3` is a giv in its own right, which is the same finding as (2) from the other
 *     end and is the concrete form the blocker above needs.  MEASURED: naming it
 *     (`pi = a * 3;` then `... + pi`) is not enough on its own -- 546 instructions and a
 *     0x5c frame, worse -- because `pi` then has a single use and loop.c folds it back.
 *
 * ALSO ESTABLISHED AND LOAD-BEARING: `view = *(void **)((char *)tbl - 0x6c)` (the
 * one-symbol-plus-offset derive off iwram_3001eec, `mov r3,r6 / sub r3,#0x6c /
 * ldr r3,[r3]`); `tbl` kept as a named local so `tbl[7]` / `tbl[8]` are `ldr r3,[r6,#0x1c]`
 * / `ldr r6,[r6,#0x20]` off one base; `*q++ = ...` in the sine loop for the ROM's
 * `stmia r6!,{r0}`; `((0x80 << 12) - sin(ang) * 2) >> 10` with the multiply on the RIGHT
 * of the subtraction; `_SetBattleActorKnockback` declared with ONE argument here (the ROM
 * sets only r0 before the `bl`, unlike BaseAnim_Blast in the same batch, which sets r1
 * too -- the two files are separate TUs so the two declarations do not conflict);
 * `Ledefc[w % 8]` as a SIGNED modulo (`cmp #0 / bge / sub #0x17`, then
 * `asr #3 / lsl #3 / sub`); and the two draw calls with `v.x` RE-READ from the vec3_t
 * between them, which is automatic because the vec is memory and the first call
 * invalidates it.
 *
 * FRAME: ours `sub sp, #0x58` against the ROM's `sub sp, #0x54` -- one slot too many,
 * which is the spilled particle pointer from (4).  The ROM's map is
 *   8=pi 0xc=b*0x20 0x10=w 0x14=io 0x18=nframes-0x10 0x1c=view+0xc 0x20=variant*4
 *   0x24=&state 0x28=nframes 0x2c=view 0x30=fn0 0x34=fn1 0x38=b 0x3c=ctx 0x40=base
 *   0x44=variant 0x48..0x50=vec3_t
 * and reproducing it is the same job as reproducing the giv set.
 *
 * NO SHIMS IN THIS DRAFT: no register pins, no `asm volatile`, no `volatile` beyond the
 * palette pointer, no flag override, no .sym entry (every constant is a literal or an
 * existing FILE_* id).
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

extern int *iwram_3001eec[];

extern signed char Ledee8[] __asm__(".Ledee8");
extern signed char Ledefc[] __asm__(".Ledefc");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern int Func_80e3944(Part *in, vec3_t *out);
extern void Func_80e38b8(Part *g, int a, int b);
extern int **_GetBattleActor(int id);
extern int sin(int a);
extern int __divsi3(int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void BaseAnim_SonicWave(void *context, int variant)
{
    vec3_t v;
    int **tbl;
    int **pp;
    unsigned char *base;
    void *ctx;
    void *view;
    State **slot;
    DrawFn fn0;
    DrawFn fn1;
    int *src;
    int *dst;
    Part *p;
    int a, b, c, i, k;
    int frame, nframes, io, ang, arg, file, np, pi;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    view = *(void **)((char *)tbl - 0x6c);
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(1);
    if ((*slot)->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 0xb, 2);
        fn0 = (DrawFn)tbl[7];
        BuildDraw2DFuncEx(0x2f, 7, 7, 3, 2);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 0xf, 2);
        fn0 = (DrawFn)tbl[7];
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    }
    fn1 = (DrawFn)tbl[8];
    LoadVFXFile(FILE_58, base, 0, 0);
    switch (variant) {
    case 0:
        file = FILE_b4;
        break;
    case 1:
        file = FILE_a0;
        break;
    case 2:
        file = FILE_cb;
        break;
    case 3:
        file = FILE_86;
        break;
    case 4:
        file = FILE_a3;
        break;
    default:
        file = FILE_a3;
        break;
    }
    {
        CopyFn copy;
        void *data = GetFile(file);
        copy = Func_8001af8;
        copy((volatile u16 *)(0xa0 << 19), data, 0x80);
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    arg = 0x90;
    StartTask(Task_BlitAnim, arg << 3);
    src = *_GetBattleActor((*(State **)(base + 0x7828))->f8);
    np = (*(State **)(base + 0x7828))->f14 * 3;
    nframes = np * 6 + 0x30;
    a = 0;
    if ((*(State **)(base + 0x7828))->f14 != 0) {
        do {
            dst = *_GetBattleActor(
                (*(State **)(base + 0x7828))->ids[a]);
            k = 0;
            p = (Part *)(base + (0xe1 << 7) + a * 3 * 0x1c);
            do {
                int x = src[2];
                int y = src[3] + (0xa0 << 13);
                int z = src[4];
                p->x = x;
                p->y = y;
                p->z = z;
                p->vx = (dst[2] - x) / 0x18;
                p->vy = (dst[3] + (0xa0 << 13) - y) / 0x18;
                p->vz = (dst[4] - z) / 0x18;
                k++;
                p->t = 0;
                p = (Part *)((char *)p + 0x1c);
            } while (k != 3);
            a++;
        } while (a != (*(State **)(base + 0x7828))->f14);
    }
    frame = 0;
    if (nframes != 0) {
      do {
        {
            int *q = (int *)(base + (0xd3 << 7));
            i = 0;
            ang = frame << 12;
            do {
                *q++ = ((0x80 << 12) - sin(ang) * 2) >> 10;
                i++;
                ang += 0x80 << 5;
            } while (i != 0xa0);
        }
        if (frame > nframes - 0x10) {
            REG_BLDALPHA = (nframes - frame) | 0x1000;
        }
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        b = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            do {
                io = 0x24 + b * 2;
                if (frame >= b * 0x20) {
                    int vi = variant * 4;
                    c = 0;
                    do {
                        if (frame >= b * 0x20 + c * 6) {
                            int s;
                            p = (Part *)(base + (0xe1 << 7)
                                         + (b * 3 + c) * 0x1c);
                            Func_80e3944(p, &v);
                            v.x >>= 1;
                            s = p->t / 8;
                            if (s > 5) {
                                s = 5;
                            }
                            if (Ledee8[vi] != 0) {
                                unsigned char *sp2;
                                int m = frame / 2 % 3;
                                sp2 = base + (s + m * 6) * 0x320;
                                fn0(ctx, sp2, v.x - 0xa, v.y - 0x28,
                                    0x14, 0x28);
                                fn1(ctx, sp2, v.x - 0xa, v.y, 0x14, 0x28);
                            } else {
                                unsigned char *sp2;
                                sp2 = base + (s + 0xc) * 0x320;
                                fn0(ctx, sp2, v.x - 0xa, v.y - 0x28,
                                    0x14, 0x28);
                                fn1(ctx, sp2, v.x - 0xa, v.y, 0x14, 0x28);
                            }
                            Func_80e38b8(p, 0x40, 0);
                            p->t += 1;
                        }
                        c++;
                    } while (c != 3);
                }
                if (Ledee8[variant * 4 + 3] != 0
                    && frame >= b * 0x20 + 0x1e
                    && frame < b * 0x20 + 0x3e) {
                    int *ap = *_GetBattleActor(
                        (*(State **)(base + 0x7828))->ids[b]);
                    int w = frame - b * 0x20 - 0x1e;
                    ap[2] += Ledefc[w % 8] << 16;
                    if (ap[2] > 0) {
                        ap[2] += 0x8000;
                    } else {
                        ap[2] += 0xffff8000;
                    }
                    Func_80d6888((*(State **)(base + 0x7828))->ids[b],
                                 -1, 5, -1, 0);
                }
                if (Ledee8[variant * 4 + 1] != 0) {
                    if (frame == b * 0x20 + 0x18) {
                        _PlaySound(0x85);
                        if (b == 0) {
                            _Func_80bd7dc(-1);
                        }
                        Func_80d6888((*(State **)(base + 0x7828))->ids[b],
                                     7, 5, b, 8);
                    }
                    if (frame == b * 0x20 + 0x28) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[b],
                                     7, 5, b, 8);
                    }
                }
                if (Ledee8[variant * 4 + 2] != -1
                    && frame == b * 0x20 + 0x18) {
                    *(int *)(base + 0x77a8) = 4;
                    _SetBattleActorKnockback(
                        (*(State **)(base + 0x7828))->ids[b]);
                }
                b++;
            } while (b != (*(State **)(base + 0x7828))->f14);
        }
        UpdateScreenShake(8, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
      } while (frame != nframes);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
