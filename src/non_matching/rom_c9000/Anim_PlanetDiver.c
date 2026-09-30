/* Anim_PlanetDiver -- 0x080ce034, 516 instructions.  PARKED, VERY CLOSE:
 * size and count EXACT and the RELOCATION SYMBOL SEQUENCE IDENTICAL.
 *
 * NON-MATCHING, 353 of 538 encodings differ.
 *
 * READ THE TWO NUMBERS TOGETHER -- 353 IS THE INFLATED ONE, AND THE LOWER
 * FIGURE BELONGS TO THE WORSE CANDIDATE.  This is the recorded trap, hit head
 * on.  SIZE matches exactly and COUNT matches exactly (538 = 538), so objcmp's
 * figure is not saturated in the usual way -- but the stream still carries
 * insert/delete PAIRS, and index-by-index comparison charges every instruction
 * after the first insert.  aligncmp (alignment-tolerant, masks NOTHING, a moved
 * pool offset still counts) is therefore the ranking view here:
 *
 *     candidate   objcmp   aligned-equal      ins/del hunks
 *     p3          205      453 / 538  84.2%   6
 *     p4          353      458 / 538  85.1%   4
 *     p5 (THIS)   353      459 / 538  85.3%   4
 *
 * p3 reads 148 LOWER on objcmp and is STRUCTURALLY FARTHER: it has two more
 * insert/delete regions.  p4/p5's inserts land EARLIER (ref index 71 rather
 * than 162), so a longer middle stretch is index-misaligned and the raw count
 * balloons while the actual agreement improves.  Ranked as the discipline
 * requires -- size-and-count exactness first, then the aligned figure, never
 * the raw count -- p5 is the park.  p3 is kept in the batch's scratch as the
 * lower-objcmp/worse-structure specimen.
 *
 * RELOCATIONS: the line prints, and the SYMBOL SEQUENCE IS IDENTICAL -- all 54
 * rows, in order, including every `_call_via_rN` veneer REGISTER.  Only offsets
 * move: 26 of 54 rows, all by -4 bytes, beginning at `_GetBattleActor`
 * (ref 0xc2, ours 0xbe) and never diverging further.  So the entire call and
 * indirect-call structure of the function is already right and the residue is
 * TWO 16-bit instructions of placement, not of content.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_PlanetDiver.c \
 *     asm/rom_c9000/rom_cd508_c_a.s --func Anim_PlanetDiver
 *
 * SPLIT SHAPE: TEXT-ONLY, THREE WAYS, NO NEW EXPORTS, ONE LINKER LINE.
 * `python3 tools/datacheck.py asm/rom_c9000/rom_cd508_c_a.s` is SILENT and
 * exits 0 -- the file carries NO data section, so nothing can be stranded.  It
 * holds FOUR functions in this order:
 *
 *   asm/rom_c9000/rom_cd508_c_a_a.s  InitRenderTilemapBG1 + DrawLine (asm as-is)
 *   src/rom_c9000/rom_cd508_c_a_b.c  THIS FILE
 *   asm/rom_c9000/rom_cd508_c_a_c.s  Anim_Haunt                      (asm as-is)
 *
 * stage1.ld names this object ONCE, at line 1780
 * `asm/rom_c9000/rom_cd508_c_a.o(.text)`; there is NO .rodata line, so the
 * batch-300 "the script can name an object twice because of a DIFFERENT
 * function" hazard does not apply -- but it was CHECKED by grepping the script
 * for the stem rather than trusting datacheck, which cannot see the script.
 * That one line becomes three, in the order above.
 * Per "COPY THE `.include` LINES, NOT THE FILE HEAD": rom_cd508_c_a.s opens
 * with macros.inc + gba.inc and each asm piece needs both plus its own leading
 * comment block.
 *
 * EXPORTS: NONE NEW.  The function reads exactly two data symbols and both are
 * already externs used by landed siblings -- `Data_ede48` (halfwords, defined
 * in asm/rom_c9000/rom_eda78.s) and `ewram_2010018` (the gBuffer particle array
 * offset to its +0x18 field; the ROM pools that SYMBOL, so it must be named as
 * such and NOT written as `gBuffer[i].t`, which would relocate against gBuffer
 * with an addend and show up as a relocation difference).
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE, no
 * barriers, no per-file flag override, no fakematch.txt row.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * (1) THE `f1` RELOAD-INHERITANCE LEVER: 457 of 538 -> 211, AND IT IS WHAT MADE
 *     SIZE AND COUNT EXACT.  The single largest step here by a wide margin.
 *     The ROM sets the two blit slots like this --
 *
 *         ldr  r3, [r6, #0x20]     @ tbl[8], loaded FIRST
 *         mov  r0, sp
 *         add  r0, #0x28           @ fp = blit
 *         str  r0, [sp, #8]
 *         str  r3, [r0, #4]        @ fp[1] = that value
 *
 *     -- and writing it as `fp = blit; fp[1] = (DrawFn)tbl[8];` costs an extra
 *     `str`/`ldr` pair, because the address pseudo is spilled and then reloaded
 *     before the store can use it:
 *
 *         mov r3,sp / adds r3,#40 / str r3,[sp,#16] / ldr r0,[sp,#16]
 *
 *     Naming the VALUE in a local FIRST and assigning `fp` second --
 *     `f1 = (DrawFn)tbl[8]; fp = blit; fp[1] = f1;` -- makes the store inherit
 *     the register the address was just computed into and the pair disappears.
 *     This is src/rom_c9000/rom_d9ab8_c_c_c_c_c_b.c's recorded rule ("`DrawFn
 *     fns[2]` written through the ARRAY for [0] and through `fp = fns` for [1],
 *     with the g[0x2f] value loaded into `f1` BEFORE `fp = fns`") reproduced
 *     verbatim on a second function, which is the confirmation that rule wanted.
 *
 * (2) MUTATING THE FILE POINTER INSTEAD OF OFFSETTING IT: 211 -> 205.
 *     `p += 0x80; DecompressLZ(p, base);` gives the ROM's `adds r5, #128`;
 *     `DecompressLZ(p + 0x80, base)` gives a bare `adds r0, r5, #0` copy with
 *     the addition folded elsewhere.  Both GetFile sites want the mutating form.
 *
 * (3) NOT NAMING THE TILE OFFSET -- DELETE THE ADDRESS LOCAL: aligncmp
 *     84.2% -> 85.1% (objcmp 205 -> 353, the inflation explained above).
 *     The 16-step ring loop draws from `base + ((frame - 4) / 2 << 11)`.  With
 *     `int off = ((frame - 4) / 2) << 11;` named, loop.c hoists the WHOLE
 *     invariant `base + off` and the loop body reads `mov r1, r9`.  The ROM
 *     hoists ONLY the shift into r9 and leaves the add inside the loop:
 *
 *         ldr r1, [sp, #0x24]      @ base, reloaded every iteration
 *         add r1, r9               @ + the hoisted shift
 *
 *     Writing the whole expression inline at the call site reproduces that:
 *     loop.c hoists the largest invariant subexpression it can give a register
 *     to, and with `base` memory-resident that is the shift alone.  gcc CSEs
 *     `frame - 4` out of the range test for free.  This is the recorded "A local
 *     that only holds an ADDRESS can cost the ordering -- delete it", and the
 *     new part is the diagnostic:
 *
 *     > WHEN THE ROM RELOADS A MEMORY-RESIDENT BASE INSIDE A LOOP AND ADDS A
 *     > HOISTED REGISTER TO IT, THE SOURCE DID NOT NAME `base + K`.  Naming it
 *     > hands loop.c a bigger invariant than the ROM's allocator could afford.
 *
 * (4) THE ewram INIT LOOP AS A WALKING POINTER, POINTER FIRST: aligncmp
 *     85.1% -> 85.3%.  Its preheader is `mov r2,#0x80 / ldr r3,=ewram_2010018 /
 *     mov r8,r0 / mov r1,#0 / lsl r2,#3` -- the POOL LOAD IS BORN BEFORE THE
 *     COUNTER.  That is the Anim_Hail signature read the other way round: a
 *     pool load ahead of the hoisted constants means a SOURCE-LEVEL POINTER
 *     INIT, where a pool load after them means an index and a giv.  So
 *     `g = ewram_2010018;` then `i = 0;` then `do { g->x = 0; i++; g++; }`.
 *
 * OTHER FACTS READ OFF THE REFERENCE, all confirmed by the measurement:
 *   - `int two = 2;` as a named local.  The ROM's `mov r5, #2` serves the fifth
 *     argument of BOTH BuildDraw2DFuncEx calls AND the `*(base + (0xef << 7))`
 *     store; a bare literal would be rebuilt at each site.
 *   - `Func_8001af8` is reached THROUGH A POINTER (`ldr r3,=Func_8001af8 /
 *     bl _call_via_r3`), and the pool load repeats at both call sites, so it is
 *     a BLOCK-LOCAL `CopyFn copy = Func_8001af8;` inside each `{ }`, not one
 *     function-scope local -- the 80ecef4 / Anim_Whirlwind `clear =
 *     Func_80008d8` idiom.
 *   - `_Actor_TravelTo(src, src[2] * 3, 0, src[4])` -- the third argument IS a
 *     source-level 0.  The ROM never loads r2 for the call; it inherits the
 *     `mov r2, #0` set up for the two `strb` writes at offsets 0x5a and 0x58,
 *     which is only legal if the source really passes zero there.
 *   - `if (frame >= 2 && frame <= 3)` and `if (frame >= 4 && frame <= 0xf)` --
 *     the compound conditions FUSE here and that is wanted: gcc emits the ROM's
 *     `sub r3,#2 / cmp r3,#1 / bhi` and `sub r2,#4 / cmp r2,#0xb / bhi`.  This
 *     is the one direction of the "A COMPOUND CONDITION FUSES; SPLIT IT INTO
 *     STATEMENTS" rule where you leave it fused.
 *   - `sin(ang) * frame` and `cos(ang) * mag`, not the reverse: the ROM copies
 *     the multiplier into the destination (`mov r3, r11 / mul r3, r0`) and that
 *     is the operand the source names SECOND.
 *   - `dy = -0xf0000;` then `if (src[2] <= 0) dy = 0xf0 << 12;` -- the ROM
 *     SPECULATES the first store and overwrites it, so this is the shape, not
 *     a store inside each arm.
 *   - `fp[k & 1](...)` in the draw loop, through the POINTER, giving the ROM's
 *     `lsl r4,#2 / ldr r4,[r4,r0]` with the scaled subscript as the first
 *     operand; the other call sites go through `blit[0]` / `fp[1]` directly.
 *   - `w = nt / 8 + 1` with `Data_ede48[w - 1]` and `w * 2` as the last
 *     argument: gcc CSEs `w*2` and derives the halfword byte index as `2w - 2`,
 *     exactly as d9ab8_StatDown records.
 *   - the frame-loop bound is 0x58 and BOTH particle loops run 0x100, while the
 *     ewram clear runs 0x400.
 *
 * MEASURED WORSE (evidence against this base only, not disproved in general):
 *   - naming the `src[2]` read (`z = src[2]; dy = -0xf0000; if (z <= 0) ...`)
 *     to pull the load ahead of the pool constant: aligncmp 85.3% -> 83.3%
 *     (93 diffs against 83).  The ROM's interleave there is sched2's, not the
 *     source's.
 *
 * ================================================================
 * THE BLOCKER, BY PASS: THREE SCHED2/local_alloc RESIDUES, ALL LOCAL
 * ================================================================
 *
 * With the symbol sequence identical and size and count exact, what is left is
 * three insert/delete regions and a register rotation, and nothing else:
 *
 * (A) reload/local_alloc -- THE ewram CLEAR LOOP'S COUNTER.  The ROM puts it in
 *     r8 and rematerialises `mov r0, #1` INSIDE the loop to add to it
 *     (`mov r0,#1 / add r8,r0`); ours keeps it in a low register with
 *     `adds r2,#1`, so the ROM's extra `movs r0,#0` preheader instruction has
 *     no counterpart.  Low registers are demonstrably free at that point, so
 *     this is an allocno-priority pick, not pressure -- the same class as
 *     Anim_Ground's blocker in this batch and the recorded "REGISTER
 *     ALLOCATION: it is systematic, and it is not reachable from C."
 *     Corroborated by a UNIFORM ONE-POSITION-LOW ROTATION: the ROM holds the
 *     source actor in sl/r10 where we hold it in r8.
 *
 * (B) sched2 -- THE `dy` POOL CONSTANT.  The ROM materialises `ldr r0,
 *     =0xfff10000` and spills it AFTER reading `src[2]`; we do it four
 *     instructions earlier.  Same instructions, same block, different order.
 *     Attacking it from the source made it worse (see MEASURED WORSE), which is
 *     what identifies it as sched2's own choice rather than a statement order.
 *
 * (C) one extra `movs r2, #0` in the `frame == 4` block.  The ROM's `mov r2,#0`
 *     is written once and serves the two `strb` stores AND _Actor_TravelTo's
 *     third argument; ours does not inherit it, so the zero is rebuilt.  A
 *     register-inheritance consequence of (A), not an independent fault.
 *
 * RULED OUT, with what was measured:
 *   - NOT a mis-read program and NOT a length error: SIZE and COUNT are exact
 *     and the relocation SYMBOL SEQUENCE is identical row for row, so every
 *     call, every indirect call and every pooled symbol is in the right place
 *     in the right order.
 *   - NOT a FILE-STRUCTURE refusal: datacheck is silent, no data section, no
 *     new export, one linker line.
 *   - NOT the recorded `.call_via` veneer blocker: the veneer REGISTERS all
 *     agree here, which is exactly what distinguishes this function from
 *     Anim_Ground and Anim_Bolt in the same batch.
 *
 * NEXT MOVE: this is the closest of batch 302's four and the likeliest to
 * close.  Go at (A) alone -- the counter's register -- because (C) follows from
 * it and (B) is two instructions of schedule.  Everything else agrees.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef int (*CopyFn)(void *dst, void *src, int len);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, vx, fc, vy, f14, t;
} Part;

extern int *iwram_3001eec[];
extern void *iwram_3001e80;
extern Part gBuffer[];
extern Part ewram_2010018[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);

extern int DecompressLZ(void *src, void *dst);
extern int Func_8001af8(void *dst, void *src, int len);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern int **_GetBattleActor(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void GetBattleActorPos3(int id, vec3_t *out);
extern unsigned int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void _Actor_TravelTo(void *a, int x, int y, int z);
extern void _Actor_SetAnim(void *a, int n);
extern void _Actor_Stop(void *a);
extern void _PlaySound(int sfx);
extern void _Func_80bd7dc(int a);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_PlanetDiver(void *context)
{
    vec3_t v;
    DrawFn blit[2];
    u8 *base;
    void *ctx;
    u8 *cam;
    int *dst;
    int dy;
    DrawFn *fp;
    DrawFn f1;
    char **tbl;
    char **pp;
    int *src;
    Part *g;
    char *p;
    int i;
    int k;
    int frame;
    int two;
    int arg;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    cam = (u8 *)tbl[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    DecompressLZ(GetFile(FILE_73), cam);
    p = (char *)GetFile(FILE_7d);
    {
        CopyFn copy = Func_8001af8;
        copy((void *)(0xa0 << 19), p, 0x80);
    }
    p += 0x80;
    DecompressLZ(p, base);
    two = 2;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
    blit[0] = (DrawFn)tbl[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, two);
    f1 = (DrawFn)tbl[8];
    fp = blit;
    fp[1] = f1;
    *(int *)(base + (0xef << 7)) = two;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    g = ewram_2010018;
    i = 0;
    do {
        g->x = 0;
        i++;
        g++;
    } while (i != 0x400);
    src = *_GetBattleActor((*(State **)(base + 0x7828))->f8);
    dst = *_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
    dy = -0xf0000;
    if (src[2] <= 0) {
        dy = 0xf0 << 12;
    }
    frame = 0;
    do {
        void *look = iwram_3001e80;
        InitMatrixStack();
        MatrixSetLook(look, (char *)look + 0xc);
        if (frame > 0x11 || frame == 0) {
            GetBattleActorPos3((*(State **)(base + 0x7828))->f8, &v);
            v.x = v.x / 2;
        }
        if (frame >= 2 && frame <= 3) {
            blit[0](ctx, base, v.x - 0x10, v.y - 0x40, 0x20, 0x40);
        }
        if (frame >= 4 && frame <= 0xf) {
            k = 0;
            do {
                int ang = k << 12;
                int X = v.x + ((sin(ang) * frame) >> 16) - 0x10;
                int Y = v.y + ((cos(ang) * frame) >> 16) - frame - 0x40;
                blit[0](ctx, base + (((frame - 4) / 2) << 11), X, Y, 0x20, 0x40);
                k++;
            } while (k != 0x10);
        }
        if (frame == 4) {
            src[10] = 0xa0 << 13;
            src[13] = 0x80 << 9;
            src[12] = 0xc0 << 10;
            src[18] = 0xab85;
            ((char *)src)[0x5a] = 0;
            ((char *)src)[0x58] = 0;
            _Actor_TravelTo(src, src[2] * 3, 0, src[4]);
            _Actor_SetAnim(src, 2);
            *(int *)(base + 0x77a8) = frame;
            _PlaySound(0x88);
        }
        if (frame == 0x10) {
            p = (char *)GetFile(FILE_89);
            {
                CopyFn copy = Func_8001af8;
                copy((void *)(0xa0 << 19), p, 0x80);
            }
            p += 0x80;
    DecompressLZ(p, base);
            src[18] = 0;
            src[9] = 0;
            src[10] = 0;
            src[4] = dst[4];
            _Actor_Stop(src);
        }
        if (frame > 0x11) {
            if (src[3] > 0) {
                src[2] = src[2] + dy;
                src[3] = src[3] - 0x80000;
                if ((*(State **)(base + 0x7828))->f4 == 0) {
                    blit[0](ctx, base, v.x - 0x14, v.y - 0x34, 0x28, 0x40);
                    v.x = v.x - 8;
                } else {
                    fp[1](ctx, base, v.x - 0x1a, v.y - 0x34, 0x28, 0x40);
                    v.y = v.y + 8;
                }
            }
            if (src[3] < 0) {
                src[3] = 0;
                k = 0;
                g = gBuffer;
                do {
                    int mag = (Random() & 0x3ff) + 0x20;
                    int ang = Random() & 0xffff;
                    g->x = v.x << 16;
                    g->y = (v.y - 0x18) << 16;
                    g->vx = (sin(ang) * mag) >> 6;
                    g->vy = -((cos(ang) * mag) << 1) >> 6;
                    g->t = (Random() & 7) + 0x20;
                    k++;
                    g++;
                } while (k != 0x100);
                *(int *)(base + 0x77a8) = 8;
                _Func_80bd7dc(0x91);
                _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[0], 4);
                Func_80d6888((*(State **)(base + 0x7828))->ids[0], 7, 5, 0, 8);
            }
        }
        k = 0;
        g = gBuffer;
        do {
            if (g->t > 0) {
                int nx = g->x + g->vx;
                int ny = g->y + g->vy;
                int nt = g->t - 1;
                g->x = nx;
                g->t = nt;
                g->y = ny;
                g->vx = g->vx * 56 / 64;
                g->vy = g->vy * 56 / 64 + (0x80 << 6);
                if (ny > (0xe0 << 15)) {
                    g->vy = -g->vy / 2;
                } else if ((u32)nx <= 0x7effff && ny >= 0) {
                    int w = nt / 8 + 1;
                    fp[k & 1](ctx, cam + Data_ede48[w - 1], (nx >> 16) - w / 2,
                              (ny >> 16) - w, w, w * 2);
                }
            }
            k++;
            g++;
        } while (k != 0x100);
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    } while (frame != 0x58);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
