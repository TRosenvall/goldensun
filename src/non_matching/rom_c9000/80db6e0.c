/* BaseAnim_Blast -- 0x080db6e0, 470 instructions.  PARKED, VERY CLOSE.
 *
 * NON-MATCHING: 46 encodings of 493 differ (objcmp).  (The draft claimed 48; re-measured
 * against the placed file, which parkcheck flagged.  46 is a TRUE distance -- instruction
 * count is 470 = 470 exact, and objcmp's SIZE line is a false positive by the length of
 * the 94-byte .rodata this candidate does not define.)
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_c9000/80db6e0.c asm/rom_c9000/rom_db6c8_c_c.s --whole
 *
 * objcmp's SIZE line reads "ref 1186 bytes, ours 1092" and the 94-byte gap is EXACTLY
 * 0xeeb40 - 0xeeae2, the three .rodata blobs the reference .s carries and this candidate
 * declares `extern`.  That is the documented objcmp section-sum artefact seen from the
 * other side (Anim_Break's header records the version where the CANDIDATE defines the
 * blob).  ENCODINGS and instruction count are the real measurements and both are aligned:
 * 470 instructions, ref and ours.
 *
 * ================================================================
 * SPLIT SHAPE (asked for explicitly)
 * ================================================================
 *
 * rom_db6c8_c_c.s holds BaseAnim_Blast alone (`grep -ci func_start` = 1) plus a
 * `.section .rodata` tail of three blobs:
 *     .Leeae2  .incrom 0xeeae2, 0xeeaec   (10 bytes, unsigned char)
 *     .Leeaec  .incrom 0xeeaec, 0xeeafa   (14 bytes, unsigned short[7])
 *     .Leeafa  .incrom 0xeeafa, 0xeeb40   (70 bytes, unsigned short)
 *
 * So the landing is the "text + data tail" shape: cut the code off the data with
 * tools/split_s.py, keep the data-only remainder in asm/, and add three lines
 *
 *     .global .Leeae2
 *     .global .Leeaec
 *     .global .Leeafa
 *
 * IMMEDIATELY BEFORE THEIR LABELS (not in the preamble -- split_s.py refuses a preamble
 * holding more than includes), exactly as asm/rom_c9000/rom_cc5d8_c_c.s already does for
 * .Lee058/.Lee05c/.Lee060.  None of the three is `.global` today; datacheck.py's EXPORTS
 * line does not name them, because it cannot tell which labels a function READS.  DO NOT
 * emit them from C: all three sit inside the long .rodata run that ends at 0xeeb40, so
 * moving them out of the middle would move their addresses.
 *
 * Index widths, read off the ROM's own addressing:
 *   .Leeae2  `ldrb r3, [r0, r1]`                 -> unsigned char, index nlv*3 {,+1,+2}
 *   .Leeaec  `lsl r0,#1 / ldrh r1, [r3, r0]`     -> unsigned short, index t/3
 *   .Leeafa  `ldrh r4, [r3, r0]`, then `lsr #1`  -> UNSIGNED short (the /2 is an lsr)
 *
 * ================================================================
 * THE WHOLE REMAINING RESIDUE, IN THREE NAMED PIECES
 * ================================================================
 *
 * (A) THE FRAME IS 4 BYTES SHORT: `sub sp, #0x34` against the ROM's `sub sp, #0x38`, and
 *     that single missing word shifts EVERY slot offset by 4 and accounts for about 40 of
 *     the 48 differing encodings.  It is not a shape difference -- it is one extra
 *     spilled pseudo in the ROM that this source does not create.
 *
 *     The evidence is unusually sharp.  Outgoing args are 8 bytes (`str r0,[sp]` /
 *     `str r4,[sp,#4]`, two words, both calls), so the ROM's locals run 8..0x37 = 48
 *     bytes: the 12-byte vec3_t at 0x2c (`add r5, sp, #0x2c`, read at +0/+4/+8) plus NINE
 *     scalar slots.  We produce EIGHT: variant@0x24, ctx@0x20, blit@0x1c, nlv@0x18 and
 *     the four LICM invariants nlv*2 / idx / idx+1 / idx+2.  The ROM's ninth slot is
 *     sp+0x20 and IT IS NEVER REFERENCED -- `grep 'sp, #0x' asm/rom_c9000/rom_db6c8_c_c.s`
 *     lists 0xc, 0x10, 0x14, 0x18, 0x1c, 0x24, 0x28, 0x2c and nothing at 0x20.
 *
 *     An allocated-but-unreferenced slot is reload's signature for a pseudo that got NO
 *     hard register (so alter_reg called assign_stack_local for it) and then had all its
 *     references rematerialised or deleted -- the REG_EQUIV path.  It is NOT frame
 *     rounding: this build emits `sub sp, #0x34` for 0x34 of frame, so gcc-2.96/thumb
 *     does not pad to an 8-byte boundary here, and the ROM's 0x38 must come from real
 *     content.  MEASURED AND REJECTED as the source of it: a shared `int xneg =
 *     0xffce0000;` local for the two `q->x = 0xffce0000` stores (the obvious REG_EQUIV
 *     candidate) gets a HARD register instead, steals r9 from `frame`, and costs 2
 *     instructions -- 472 against 470, span 88 against 48; and naming `idx = nlv*3` as an
 *     explicit local collapses the frame to 0x28 and loses 6 instructions.
 *
 * (B) ONE r5/r6 SWAP, twice.  In the gBuffer init loop the ROM has r5 = the particle
 *     pointer and r6 = the shared zero; we have them the other way round.  The identical
 *     two-way tie appears again in the second draw loop (ROM r6 = q, r5 = &v).  About 14
 *     encodings, all of them the same two register names.  MEASURED INERT: `g++; i++;`
 *     against `i++; g++;` at the bottom of either loop, and of both together -- all three
 *     permutations give byte-identical output.  That is the Func_80cc960 lever (c)
 *     failing to fire here, so the tie is being broken somewhere other than insn order.
 *
 * (C) THREE ENCODINGS OF SCHEDULING in the first draw call's source address.  The ROM
 *     emits `mov r2,#0xc8 / lsl r2,#6 / lsr r3,r0,#31 / add r1,r10 / add r1,r2`; we hoist
 *     `add r1,r10` two slots earlier.  MEASURED INERT: writing the sum as
 *     `base + 0x3200 + Data_ede48[w-1] + (i&1)*0x302` (52), and as
 *     `(i&1)*0x302 + Data_ede48[w-1] + base + 0x3200` (60, worse) -- fold puts the
 *     constant outermost regardless, so source order does not reach this.
 *
 * ================================================================
 * FIVE LEVERS THAT PAID, EACH WITH ITS SINGLE-DROP MEASUREMENT
 * ================================================================
 *
 * (1) ONE COUNTER VARIABLE FOR ALL SIX LOOPS.  Worth span 204 -> 166 and it is what put
 *     the counter in r8.  The ROM keeps EVERY loop counter in r8 -- both particle init
 *     loops, both draw loops, and both actor loops -- which is Anim_Break's recorded "a
 *     counter reused across several loops is ONE variable" rule, and here it is worth
 *     eight instructions: separate `i`/`j`/`k` locals put the counters in LOW registers,
 *     where `i++` is one instruction instead of the ROM's `mov r2,#1 / add r8,r2` pair,
 *     so the candidate came out SHORTER than the ROM (462 against 470).  A count that is
 *     too LOW because the ROM used an expensive register is a distinctive signature.
 *
 * (2) `i = 0;` OUTSIDE THE INIT-LOOP BLOCK, NOT INSIDE IT NEXT TO `int z = 0;`.  This is
 *     the single biggest step in the whole function: span 87 -> 54, encodings 392 -> 55,
 *     and it made the instruction count exact.  MECHANISM: the ROM initialises the
 *     counter and the shared zero carrier from TWO DIFFERENT zero constants
 *     (`mov r1,#0 / mov r8,r1` for the counter, `mov r6,#0` for the carrier).  With
 *     `i = 0;` written inside the same block as `int z = 0;` gcc's cse unifies the two
 *     zeros into one pseudo and emits ONE `mov #0` -- one instruction short, and the
 *     register assignment of the whole loop rotates off it.  Hoisting `i = 0;` above the
 *     block puts the two zeros in different basic-block positions, cse leaves them apart,
 *     and the ROM's pair comes back.  Applying the same edit to the FIRST init loop as
 *     well is worth a further 55 -> 52.  THIS IS THE COUNTERPART OF Func_80cc960's
 *     `int z = 0;` CARRIER LEVER: the carrier is right, but it must not swallow the
 *     counter's own zero.
 *
 * (3) `arg <<= 3;` AS ITS OWN STATEMENT before `StartTask(Task_BlitAnim, arg)`, worth
 *     52 -> 50.  Unlike Anim_Vine, this function does NOT want the pinned-r0 shim: the
 *     ROM has `lsl r1,#3` BEFORE `ldr r0,=Task_BlitAnim`, i.e. plain "fill r0 last", and
 *     the only thing needed is that the shift be a separate statement so it gets the
 *     lower insn UID.  No pin, no fakematch row.
 *
 * (4) THE FRAME LOOP IS THE `while` FORM, worth 50 -> 48.  `frame = 0; while (frame !=
 *     Leeae2[nlv*3+2]) { ... }` beats `if (...) { do ... while (...); }` here -- the
 *     opposite of Anim_Break and the same as Anim_Drain.  The deciding question is the
 *     recorded one: the ROM RE-READS `Leeae2[idx+2]` at both the guard and the bottom
 *     test through two different registers (`ldrb r3,[r0,r1]` at entry, `ldrb r3,[r2,r4]`
 *     at the bottom), so the two tests are duplicate_loop_exit_test copies, not one
 *     gcse-shared expression.
 *
 * (5) NO SHIMS AT ALL.  No register pins, no `asm volatile`, no `volatile` beyond the
 *     `volatile u16 *` palette pointer that the hardware write genuinely needs, no
 *     DMA3_SET, no `.equ`, no flag override.  If (A) and (B) are closed this lands with a
 *     clean fakematch ledger -- which is worth saying, because Anim_Vine and BaseAnim_Growth
 *     in the same family both needed a shim.
 *
 * ALSO LOAD-BEARING, each confirmed by the diff collapsing when it was written this way:
 *   - `pp = tbl; base = *pp++; ctx = *pp;` for the `ldmia r3!, {r1}` pointer-table idiom;
 *   - `if ((unsigned)q->t <= 0x14)` for the FIRST test and `if (q->t <= 0x14)` for the
 *     second: the ROM's `bhi` then `bgt` on the same value is `0 <= t && t <= 0x14` folded
 *     into an unsigned range test, then a plain signed compare on the value RE-READ after
 *     the call (cse's memory table is invalidated by `bl _call_via_r4`, which is why the
 *     ROM has a second `ldr r0,[r6,#0x18]`);
 *   - `(i & 1) * 0x302`, which gcc synthesises as `lsl/add/lsl #7/add/lsl #1` -- the
 *     ROM's exact five instructions for 770 = ((3 << 7) + 1) << 1;
 *   - `w = 9 - (v.z - 0xa0) / 0x40` for the `cmp #0 / bge / add #0x3f / asr #6` signed
 *     division, and `unsigned w = Leeafa[m]` so that `w / 2` is `lsr`, not the signed
 *     three-instruction form;
 *   - both actor loops written out IN FULL in their two arms (the variant-0 arm with
 *     `_SetBattleActorKnockback`, the other without), which is the "duplicated ROM code
 *     means duplicated source" rule; gcc compiles the two arms DIFFERENTLY by itself
 *     (arm 0 holds the state address in r5, arm 1 re-reads it with the register-offset
 *     `ldr r3,[r4,r2]`), so that asymmetry is not something the source asks for.
 *
 * MEASURED INERT: `Leeae2[nlv + nlv*2]` vs `Leeae2[nlv*2 + nlv]`; all three
 * increment-order permutations; declaration order of `z` vs `mask` inside either block.
 *
 * No .sym entry is warranted -- every constant here is a plain literal or an existing
 * FILE_* id.  No per-file Makefile flag override applies to this stem.
 */
#include "gba/types.h"
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
extern void *iwram_3001e80;
extern void *gPtrs[];
extern Part gBuffer[];
extern unsigned short Data_ede48[];

extern unsigned char Leeae2[] __asm__(".Leeae2");
extern unsigned short Leeaec[] __asm__(".Leeaec");
extern unsigned short Leeafa[] __asm__(".Leeafa");

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
extern int Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void BaseAnim_Blast(void *context, int variant)
{
    vec3_t v;
    int **tbl;
    int **pp;
    unsigned char *base;
    void *ctx;
    State **slot;
    DrawFn blit;
    Part *q;
    Part *g;
    int i;
    int frame, nlv, arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(1);
    LoadVFXFile(FILE_c0, base, 1, 0);
    if (variant == 1) {
        volatile u16 *pal;
        i = 0;
        pal = (volatile u16 *)(0xa0 << 19);
        do {
            int c = i / 2;
            *pal = (c << 10) | (c << 5) | c;
            i++;
            pal++;
        } while (i != 0x40);
        nlv = 1;
    } else {
        CopyFn copy;
        void *data = GetFile(FILE_96);
        copy = Func_8001af8;
        copy((volatile u16 *)(0xa0 << 19), data, 0x80);
        nlv = (*slot)->f18;
    }
    i = 0;
    {
        int z = 0;
        unsigned mask = 0x3f;
        q = (Part *)(base + (0xe1 << 7));
        do {
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                q->x = 0xc8 << 14;
            } else {
                q->x = 0xffce0000;
            }
            q->y = z;
            q->z = z;
            q->vx = ((Random() & mask) - 0x20) << 13;
            q->vy = ((Random() & mask) + 0x10) << 12;
            q->vz = ((Random() & mask) - 0x20) << 13;
            q->t = z;
            i++;
            q = (Part *)((char *)q + 0x1c);
        } while (i != 0x20);
    }
    i = 0;
    {
        int z = 0;
        unsigned mask = 0x3f;
        g = gBuffer;
        do {
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                g->x = 0xc8 << 14;
            } else {
                g->x = 0xffce0000;
            }
            g->y = z;
            g->z = z;
            g->vx = ((Random() & mask) - 0x20) << 13;
            g->vy = ((Random() & 0x1f) + 8) << 13;
            g->vz = ((Random() & mask) - 0x20) << 13;
            g->t = z;
            i++;
            g++;
        } while (i != (0x80 << 3));
    }
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 2);
    blit = (DrawFn)gPtrs[0xb8 / 4];
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    frame = 0;
    while (frame != Leeae2[nlv * 2 + nlv + 2]) {
      {
        void *view = iwram_3001e80;
        InitMatrixStack();
        MatrixSetLook(view, (char *)view + 0xc);
        if (frame == 2) {
            _PlaySound(0x90);
        }
        if (frame == Leeae2[nlv * 2 + nlv + 2] - 0x30) {
            _Func_80bd7dc(0x85);
        }
        i = 0;
        if (Leeae2[nlv * 2 + nlv] != 0) {
            g = gBuffer;
            do {
                if (g->y >= 0) {
                    int w;
                    Func_80e3944(g, &v);
                    v.x >>= 1;
                    v.x = v.x + ((*(State **)(base + 0x7828))->f4 << 5) - 0x10;
                    if (v.z < 0xa0) {
                        v.z = 0xa0;
                    }
                    if (v.z > 0x31f) {
                        v.z = 0x31f;
                    }
                    w = 9 - (v.z - 0xa0) / 0x40;
                    blit(ctx, base + Data_ede48[w - 1] + (i & 1) * 0x302 + 0x3200,
                         v.x - w / 2, v.y - w, w, w * 2);
                    Func_80e38b8(g, 0x40, 0xffffe000);
                }
                i++;
                g++;
            } while (i != Leeae2[nlv * 2 + nlv]);
        }
        if (frame > 2) {
            i = 0;
            if (Leeae2[nlv * 2 + nlv + 1] != 0) {
                q = (Part *)(base + (0xe1 << 7));
                do {
                    if (i < frame && q->y >= 0) {
                        Func_80e3944(q, &v);
                        v.x >>= 1;
                        v.x = v.x + ((*(State **)(base + 0x7828))->f4 << 5) - 0x10;
                        if ((unsigned)q->t <= 0x14) {
                            int m = q->t / 3;
                            unsigned w = Leeafa[m];
                            blit(ctx, base + Leeaec[m], v.x - w / 2,
                                 v.y - w / 2, w, w);
                        }
                        if (q->t <= 0x14) {
                            q->t = q->t + 1;
                        }
                        Func_80e38b8(q, 0x40, 0xffffe000);
                    }
                    i++;
                    q = (Part *)((char *)q + 0x1c);
                } while (i != Leeae2[nlv * 2 + nlv + 1]);
            }
        }
        if (variant == 0) {
            i = 0;
            while (i != (*(State **)(base + 0x7828))->f14) {
                if (frame == i + 6) {
                    Func_80d6888((*(State **)(base + 0x7828))->ids[i],
                                 7, 5, i, 0xa);
                    _SetBattleActorKnockback(
                        (*(State **)(base + 0x7828))->ids[i], 2);
                }
                i++;
            }
        } else {
            i = 0;
            while (i != (*(State **)(base + 0x7828))->f14) {
                if (frame == i + 6) {
                    Func_80d6888((*(State **)(base + 0x7828))->ids[i],
                                 7, 5, i, 0xa);
                }
                i++;
            }
        }
        if (frame == 2) {
            *(int *)(base + 0x77a8) = 6;
        }
        UpdateScreenShake(0x10, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
      }
    }
    StopTask(Task_BlitAnim);
    gfree(0x2e);
    AnimEnd();
}
