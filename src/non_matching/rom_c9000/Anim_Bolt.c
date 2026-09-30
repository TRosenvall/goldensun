/* Anim_Bolt -- 0x080ddde0, 555 instructions.  PARKED, FOUR INSTRUCTIONS SHORT.
 *
 * NON-MATCHING, 510 of 587 encodings differ.
 *
 * MEASUREMENT -- THE OBJCMP FIGURE IS SATURATED AND MUST NOT BE USED TO RANK.
 * SIZE does NOT match (ref 1304 bytes, ours 1296) and the instruction COUNT
 * does NOT match (ref 587, ours 583), so 510 is the "every index after the
 * first shift differs" number and carries no information about distance.  The
 * ranking view for this function is aligncmp, which reads
 *
 *     aligned-equal 293 of 587 = 49.9%,  344 differing/ins/del in 115 hunks
 *
 * and that is the figure to move.  Both numbers are given because parkcheck
 * re-measures the first line with objcmp and a mismatch there is a defect.
 *
 * RELOCATIONS: 53 rows both sides, no symbol missing and none extra.  The
 * SYMBOL SEQUENCE differs in exactly TWO rows and both are `_call_via_rN`
 * veneer REGISTERS (sequence index 8: ref r5 / ours r6; index 12: ref r4 /
 * ours r5).  45 of 53 rows differ in OFFSET, which is the expected consequence
 * of being four instructions short, not a separate finding.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Bolt.c \
 *     asm/rom_c9000/rom_dd2ac_c_c_c.s --func Anim_Bolt
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT, THREE WAYS, AND NINE NEW EXPORTS.
 * `python3 tools/datacheck.py asm/rom_c9000/rom_dd2ac_c_c_c.s` reports a
 * `.rodata` section and three functions, so converting one needs a TEXT/DATA
 * SPLIT -- the data must keep its own object:
 *
 *   asm/rom_c9000/rom_dd2ac_c_c_c_a.s  Anim_Thorn                 (asm as-is)
 *   src/rom_c9000/rom_dd2ac_c_c_c_b.c  THIS FILE
 *   asm/rom_c9000/rom_dd2ac_c_c_c_c.s  Anim_Djinni + the .rodata tail
 *
 * NINE `.global` lines must be added to the _c piece, beside its
 * `.section .rodata`, and they fall into two groups -- datacheck names both:
 *
 *   for THIS function:   .global .Leebd6  .Leebe2  .Leebe6
 *   for Anim_Thorn, which stays in asm but is now a DIFFERENT object from the
 *   data it reads:      .global .Leeba6  .Leebae  .Leebb6  .Leebb9
 *                               .Leebc0  .Leebc8
 *
 * THE SECOND GROUP IS THE ONE THAT IS EASY TO MISS: Anim_Thorn is byte-neutral
 * across this split and needs no source work, but it loses its file-local
 * access to six blobs the moment the .rodata leaves with the _c piece.  Twelve
 * further labels in that tail (.Leeb48 ... .Leeb96) are ALREADY `.global` from
 * the earlier split that landed Anim_Vine, so they need nothing.  DO NOT emit
 * any of these blobs from C: they sit in the middle of a 21-blob run
 * (.Leeb48 ... .Leebe6) and moving one out of the middle moves its address --
 * the rule Anim_Vine's landed note states for .Leeb96 in this very file.
 *
 * stage1.ld names this object TWICE -- line 1876 `(.text)` and line 1976
 * `(.rodata)`.  The .rodata line must stay on the piece that keeps the data,
 * i.e. both the _c `.text` and the _c `.rodata` entry; the batch-300 hazard is
 * live here and was checked by grepping the script for the stem.
 *
 * `tools/split_s.py` was deliberately NOT run: it rewrites the .s files and the
 * linker script in place, and this was measured from a read-only workspace.
 *
 * EXPORTS ALREADY AVAILABLE, no asm edit: `Data_ede48`, `Data_edebe`,
 * `Data_edeca`, `Data_eded0` (all `.incdata` in asm/rom_c9000/rom_eda78.s) and
 * `ewram_2010018`, all already declared as externs by landed siblings.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE, no
 * barriers, no per-file flag override, no fakematch.txt row.
 *
 * ================================================================
 * THE DATA, AS READ OFF THE ROM
 * ================================================================
 *
 *   .Leebd6  0xeebd6..0xeebe2  12 bytes  THREE groups of FOUR, indexed
 *                                        [state->f18 * 4 + n]:
 *                                        n=0 particle count, n=1 spark count,
 *                                        n=2 a modulus, n=3 bolt-segment count
 *   .Leebe2  0xeebe2..0xeebe6   4 bytes  palette nibble, indexed Random() & 3
 *   .Leebe6  0xeebe6..0xeebe9   3 bytes  a BuildDraw2DFuncEx argument, indexed
 *                                        by state->f18 directly
 * All read with `ldrb`, never sign-extended -- UNSIGNED.
 *
 * `ewram_2010018` IS gBuffer's +0x18 field (the lifetime), and the ROM pools
 * THAT symbol.  It must be named as such; writing `gBuffer[i].t = 0` would
 * relocate against gBuffer with an addend and show as a relocation difference.
 * gBuffer holds 0x400 entries of 0x1c; `base + 0x7098` is the +0x18 field of a
 * second 0x40-entry array at `base + 0x7080`, and gcc folds `0x7080 + 0x18`
 * into the ROM's single pooled 0x7098 on its own.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * (1) THE ORACLE FIRST: src/rom_c9000/rom_dd2ac_c_c_b.c (Anim_Vine) is the SAME
 *     STEM and matching, and src/rom_c9000/rom_e0564_a_b.c (Anim_Hail) supplies
 *     the family prologue.  First candidate: 560 of 587, 27 instructions short.
 *
 * (2) READING THE SECOND SPARK BLOCK'S ARITHMETIC CORRECTLY: 560 -> 583,
 *     i.e. 23 of the 27 missing instructions in one edit.  The block is
 *
 *         m  = Leebd6[f18 * 4 + 2];
 *         r  = Random() % m;                   @ bl __umodsi3, UNSIGNED
 *         py = v.y - r - Data_eded0[u] / 2 + 8;
 *         m  = m - r + 1;
 *         px = v.x + Random() % m - m / 2 - Data_edeca[u] / 2;
 *
 *     The tell is `sub r5, r0` landing BEFORE `add r5, #1`: the second modulus
 *     is taken against `m - r + 1`, and the same value is then halved for px.
 *     `extern unsigned int Random(void)` is required for `__umodsi3` rather
 *     than `__modsi3`, the recorded Anim_Spire rule.
 *
 * (3) THE ANIM_VINE GUARD-FORM LEVER, APPLIED PER LOOP: aligncmp 49.2% ->
 *     49.9%, 347 -> 344 diffs.  This function has FOUR loops whose bound is
 *     `(*(State **)(base + 0x7828))->f14` or a `.Leebd6` entry, and the two
 *     forms are NOT interchangeable.  The deciding question is the recorded
 *     one -- does the ROM RE-COMPUTE the guard's address or SHARE it:
 *
 *       - the Func_80008d8 clear loop: its guard reads the CACHED `ldr r0,
 *         [sp,#0x14]`, the same slot the body uses, so gcse unified guard and
 *         body => it wants `i = 0; if (f14 != 0) { do { } while (...); }`.
 *       - the per-actor loop: its guard reaches the state with the
 *         REGISTER-OFFSET form `mov r4,r11 / ldr r3,[r4,r2]` while the body
 *         uses sp+0x14, so the address existed only inside the loop at gcse
 *         time => it wants `while (i != f14)`.
 *       - both `.Leebd6`-bounded inner loops re-compute (`ldr r3,[r5]` off a
 *         fresh `add r5,r0,r2`) => `while` form.
 *
 *     > ONE FUNCTION CAN NEED BOTH GUARD FORMS, AND THE `ldr` THE GUARD USES
 *     > TELLS YOU WHICH: a cached spill slot shared with the body means the
 *     > `if` + do-while, a register-offset recomputation means `while`.
 *
 * OTHER FACTS READ OFF THE REFERENCE:
 *   - the frame loop is `while (frame != f14 * 8 + 0x28)`, and its entry guard
 *     `cmp r3, #-0x28` with `frame = 0` is exactly what that spells.
 *   - `ClearFn clear = Func_80008d8;` -- `bl _call_via_r5` off a pooled symbol
 *     address means the source called THROUGH A POINTER, the 80ecef4 /
 *     Anim_Whirlwind idiom.
 *   - `int t8 = i * 8;` as a named local: the ROM computes it once into r8.
 *   - `(i + frame + j) / 2 & 3` then `* 2880`, which gcc decomposes as the
 *     ROM's `*3`, `*15`, `<< 6`.
 *   - `mag = (Random() & 0x1ff) + 0x40` as ONE expression -- the ROM's
 *     `add r6, #0x40` is merely scheduled past the sin call, the Anim_Hail rule.
 *   - `sin(ang) * mag` and `-(cos(ang) * mag) >> 6`: the ROM copies mag into the
 *     destination (`mov r3,r6 / mul r3,r0`), so mag is named SECOND.
 *   - `ang = (Random() & 0x7fff) - 0x4000`, which pools 0xffffc000 and ADDS.
 *   - the final sweep tests `(u32)g->x <= 0x7effff` (the ROM's `bhi` is
 *     UNSIGNED) AND `g->y >= 0`, with `g->y` still live in the register from the
 *     preceding `> (0xd0 << 15)` test.
 *   - `w = g->t / 16 + 1` with `Data_ede48[w - 1]` and `w * 2` last.
 *
 * MEASURED WORSE (evidence against this base only, not disproved in general):
 *   - `int lim = t8 + 2;` as a named local for the two tests that use it
 *     (b4): 579 instructions, aligncmp 41.4%.  The ROM materialises `t8 + 2`
 *     into r9 at THREE separate points, which reads like a named local but is
 *     rematerialisation of a cheap value; naming it costs four instructions.
 *   - `int mask = 3;` before the spark loop for its three `& 3` / `| 3` uses
 *     (b5): 591 instructions -- FOUR LONG where the base is four SHORT -- but
 *     aligncmp 50.3%, the best figure measured on this function.  The ROM does
 *     hold 3 in r9 across that loop (`mov r3,#3 / mov r9,r3` in the preheader,
 *     then `mov r0,r9 / and r4,r0` and `mov r3,r9 / orr r3,r2`), so the local is
 *     probably RIGHT and something else is absorbing the eight instructions.
 *     RECORDED AS THE MOST PROMISING RE-ATTEMPT: b5 is kept in the batch's
 *     scratch, and `mask` combined with a fix for the blocker below is the first
 *     thing to try.
 *   - both together (b3): 592, aligncmp 41.4%.
 *
 * ================================================================
 * THE BLOCKER, BY PASS: local_alloc/global_alloc -- FOUR HIGH-REGISTER COPIES
 * THE ROM PAYS FOR AND WE DO NOT
 * ================================================================
 *
 * We are FOUR INSTRUCTIONS SHORT and a uniform ONE-POSITION-LOW register
 * rotation says why.  The ROM spends fp/r11 on `base`, sl/r10 on the loop
 * counters, r8 on `t8` and `py`, r9 on `lim` and the constant 3; we spend r9 on
 * `base` and fp/r11 on the counters and never reach sl.  Per the recorded
 * reading of rotations -- "low = one MISSING short-lived quantity" -- we are
 * carrying one fewer live quantity than the ROM.
 *
 * The four missing instructions are exactly the copies a HIGH register forces,
 * and aligncmp names them as deletes: `mov r3, r8`, `mov r0, r8`, `mov r2, r9`
 * and the `movs r0,#1`/`str r3,[sp]` pair around the first sprite call.  Thumb-1
 * cannot use r8-r11 as an ALU or `cmp` operand, so every use of a value the ROM
 * keeps there costs a `mov` into a low register.  Keeping the same values in low
 * registers, as we do, is CHEAPER AND WRONG -- the length deficit IS the
 * symptom, not an independent fault.  That is the recorded "OUR STREAM BEING
 * SHORTER IS A SIGNATURE, NOT A CURIOSITY", and here it is quantified: one
 * missing high-register allocno, four `mov` instructions.
 *
 * The obvious source handles were tried and BOTH overshoot rather than land:
 * naming `lim` costs four (b4, 579) and naming `mask` costs eight (b5, 591).
 * Neither reproduces the ROM's split, which strongly suggests the ROM's r9
 * holds BOTH `lim` and the constant 3 in disjoint live ranges -- two source
 * quantities sharing one hard register, which is an allocation outcome and not
 * something the source can request.
 *
 * RULED OUT, with what was measured:
 *   - NOT a mis-read program: the relocation set is complete and correctly
 *     ordered apart from two veneer registers; every callee, every pooled table
 *     and both `__umodsi3` sites are present exactly once in the ROM's order.
 *   - NOT the missing 23 instructions of lever (2), which are now present --
 *     the residual four are a different, purely allocative, class.
 *   - NOT a FILE-STRUCTURE refusal: the split is understood and byte-neutral by
 *     construction; the nine exports are enumerated above.
 *   - NOT the `.call_via` structural veneer blocker: both streams emit a veneer
 *     at every indirect call and only two registers differ.
 *
 * NEXT MOVE: start from b5 (`mask` named, aligncmp 50.3%) rather than from this
 * file, find where its eight extra instructions go, and look for a way to cost
 * `lim` a low register without adding a statement -- e.g. lengthening its live
 * range past the inner loop rather than naming it.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*ClearFn)(void *dst, int n, unsigned int v);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern int *iwram_3001eec[];
extern DrawFn iwram_3001f0c;
extern Part gBuffer[];
extern Part ewram_2010018[];
extern unsigned short Data_ede48[];
extern unsigned short Data_edebe[];
extern unsigned char Data_edeca[];
extern unsigned char Data_eded0[];
extern unsigned char Leebd6[] __asm__(".Leebd6");
extern unsigned char Leebe2[] __asm__(".Leebe2");
extern unsigned char Leebe6[] __asm__(".Leebe6");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int sfx);
extern void _Func_80bd7dc(int a);
extern void Func_80008d8(void *dst, int n, unsigned int v);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern unsigned int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80e3908(Part *g, int a, int b);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Bolt(void *context)
{
    vec3_t v;
    void *ctx;
    int i;
    int frame;
    DrawFn fn;
    u8 *cam;
    char **tbl;
    char **pp;
    u8 *base;
    Part *g;
    int j;
    int u;
    int arg;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    cam = (u8 *)tbl[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(1);
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    fn = (DrawFn)tbl[7];
    LoadVFXFile(FILE_ce, base, 1, 0);
    LoadVFXFile(FILE_c4, base + 0xc56, 1, 1);
    LoadVFXFile(FILE_73, cam, 0, 0);
    i = 0;
    do {
        ewram_2010018[i].x = 0;
        i++;
    } while (i != 0x400);
    i = 0;
    do {
        ((Part *)(base + 0x7080))[i].t = -1;
        i++;
    } while (i != 0x40);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    _PlaySound(0x8a);
    frame = 0;
    while (frame != (*(State **)(base + 0x7828))->f14 * 8 + 0x28) {
        if (frame == 0x18) {
            _Func_80bd7dc(0x85);
        }
        i = 0;
        if ((*(State **)(base + 0x7828))->f14 != 0) {
            ClearFn clear = Func_80008d8;
            do {
                if (frame == i * 8) {
                    clear(ctx, 0x80 << 7, 0x10101010);
                }
                i++;
            } while (i != (*(State **)(base + 0x7828))->f14);
        }
        i = 0;
        while (i != (*(State **)(base + 0x7828))->f14) {
            int t8 = i * 8;
            GetBattleActorPos3((*(State **)(base + 0x7828))->ids[i], &v);
            v.x = v.x / 2;
            if (frame == t8 + 1) {
                *(int *)(base + 0x77a8) = 4;
            }
            if (frame == t8 + 4) {
                Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 6);
                _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[i], 6);
            }
            if (frame >= t8) {
                if (frame < t8 + 0x10) {
                    int w = (frame - t8) * 64;
                    if (w > 0x68) {
                        w = 0x68;
                    }
                    j = 0;
                    while (j != Leebd6[(*(State **)(base + 0x7828))->f18 * 4 + 3]) {
                        fn(ctx, base + 0xc56 + ((i + frame + j) / 2 & 3) * 2880,
                           v.x - 0xc, 0, 0x18, w);
                        j++;
                    }
                    if (frame == t8 + 2) {
                        j = 0;
                        g = &gBuffer[i * 0x80];
                        while (j != Leebd6[(*(State **)(base + 0x7828))->f18 * 4]) {
                            int mag = (Random() & 0x1ff) + 0x40;
                            int ang = (Random() & 0x7fff) - 0x4000;
                            g->x = v.x << 16;
                            g->y = 0xd0 << 15;
                            g->vx = (sin(ang) * mag) >> 5;
                            g->vy = -(cos(ang) * mag) >> 6;
                            g->t = (Random() & 7) + 0x20;
                            j++;
                            g++;
                        }
                    }
                }
            }
            if (frame >= t8 + 2 && frame < t8 + 0x18) {
                j = 0;
                while (j != Leebd6[(*(State **)(base + 0x7828))->f18 * 4 + 1]) {
                    int m;
                    int r;
                    int px;
                    int py;
                    u = j & 3;
                    m = Leebd6[(*(State **)(base + 0x7828))->f18 * 4 + 2];
                    r = Random() % m;
                    py = v.y - r - Data_eded0[u] / 2 + 8;
                    m = m - r + 1;
                    px = v.x + Random() % m - m / 2 - Data_edeca[u] / 2;
                    BuildDraw2DFuncEx(0x2f, 7, 7,
                                      Leebe2[Random() & 3] | 3,
                                      Leebe6[(*(State **)(base + 0x7828))->f18]);
                    iwram_3001f0c(ctx, base + Data_edebe[u], px, py,
                                  Data_edeca[u], Data_eded0[u]);
                    gfree(0x2f);
                    j++;
                }
            }
            i++;
        }
        j = 0;
        g = gBuffer;
        do {
            if (g->t > 0) {
                g->t = g->t - 1;
                Func_80e3908(g, 0x3c, 0x80 << 5);
                if (g->y > (0xd0 << 15)) {
                    g->vy = -g->vy / 2;
                } else if ((u32)g->x <= 0x7effff && g->y >= 0) {
                    int w = g->t / 16 + 1;
                    fn(ctx, cam + Data_ede48[w - 1], (g->x >> 16) - w / 2,
                       (g->y >> 16) - w, w, w * 2);
                }
            }
            j++;
            g++;
        } while (j != 0x400);
        UpdateScreenShake(2, 8);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
