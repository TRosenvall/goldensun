/* Anim_Douse -- 0x080c972c.  NON-MATCHING, 384 of 602 encodings differ.
 * UNATTEMPTED before batch 303 (recovered by the census address-suffix fix; the
 * batch-302 Anim_Ice park in this same file mentions Anim_Douse only as the
 * REMAINDER of its own split, never as a reconstruction of it).
 * Reference asm/rom_c9000/rom_c91dc_a.s (2 functions: Anim_Ice first,
 * Anim_Douse second).
 *
 * 384 IS SATURATED AND RANKS NOTHING.  SIZE is NOT exact (ref 1332 bytes, ours
 * 1336) and the instruction COUNT is NOT exact (ref 602, ours 603 -- ONE LONG),
 * so objcmp compares index-by-index past an insertion.  Every ranking below is
 * tools/aligncmp.py's figure instead:
 *
 *     aligned-equal 419 (69.6% of ref), 183 differing/ins/del in 90 hunks
 *
 * RELOCATIONS: the SYMBOL SEQUENCE IS IDENTICAL EXCEPT FOR ONE EXTRA ENTRY --
 * ours emits `_call_via_r6` between the ROM's second and third `_call_via_r4`.
 * That single extra relocation IS the single extra instruction, and it is the
 * whole residue's origin (see THE BLOCKER).  All 60 other relocations are the
 * same symbols in the same order, on the FIRST candidate: objcmp's
 * "RELOCATIONS differ" line here is that one insertion plus the offset shifts
 * it causes, not a shape disagreement.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Douse.c \
 *     asm/rom_c9000/rom_c91dc_a.s --func Anim_Douse
 *
 * SPLIT SHAPE: TEXT-ONLY, NO NEW EXPORTS.  tools/datacheck.py on
 * asm/rom_c9000/rom_c91dc_a.s is SILENT -- the file has no data section.
 * `tools/split_s.py --dry-run asm/rom_c9000/rom_c91dc_a.s Anim_Douse` reports:
 *     would write asm/rom_c9000/rom_c91dc_a_a.s (1 function, 671 lines)  <- Anim_Ice
 *     would write asm/rom_c9000/rom_c91dc_a_b.s (1 function, 646 lines)  <- this
 *     would REMOVE asm/rom_c9000/rom_c91dc_a.s, would rewrite stage1.ld
 * so the landed file is src/rom_c9000/rom_c91dc_a_b.c.  NOTHING needs a new
 * `.global`: Anim_Douse reads exactly TWO data labels and both already exist --
 *   - `.Lededc` is ALREADY `.global` at asm/rom_c9000/rom_c91dc_c_c_c_c_c_c.s:650
 *   - `Data_ede5c` is an `.incdata` export at asm/rom_c9000/rom_eda78.s:32
 *     (0xede5c..0xede84) and is already an extern in three landed/parked files.
 * Anim_Ice's park lists .Leded6 / Data_ede84 / Data_ede96 for ITS half; this
 * half reads none of those, and Anim_Ice reads neither of these two, so the
 * split is clean in both directions.
 * PIN-FREE: tools/shimcount.py is clean (exit 0).  No register pins, no "+r"
 * barriers, no volatile beyond gba/io.h's own registers, no do{}while(0), no
 * .equ, and NO per-file flag override -- plain -O2 is the floor here (see FLAGS).
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * The first candidate was already 360 aligned (59.8%) with the relocation
 * symbol sequence exact but for two `_call_via_r4` -> `_call_via_r5`, and only
 * 2 instructions and 4 bytes short -- so the program shape was right before any
 * allocation work, which is what the relocation sequence is for.
 *
 * (1) ONE COUNTER FOR ALL SIX DISJOINT OUTER LOOPS AND A SECOND FOR BOTH INNER
 *     LOOPS, worth 382 -> 416 aligned (63.5% -> 69.1%).  THE BIGGEST LEVER IN
 *     THE FUNCTION, and it is elevation.md's "counters unifying across disjoint
 *     loops" read at full strength.
 *
 *     The ROM keeps ONE pseudo in r8 for the counter of every OUTER loop -- the
 *     `ewram_2010158` t-field init (0x200 iterations), the base+0x7080 particle
 *     init (0x40), BOTH gradient loops (0xa0 each), the main particle loop, and
 *     the second ewram_2010140 sweep (0x200) -- and ONE pseudo in r4/sp+8 for
 *     BOTH inner loops (the sub-particle spawn loop and the f14 id loop).  r8 is
 *     a HIGH register, so every use pays a `mov r7, r8` copy; that cost is the
 *     tell, because gcc only puts a counter in HI_REGS when LO_REGS is full.
 *
 *     The first candidate used `i` for five of the six outer loops but `k` for
 *     BOTH the second ewram_2010140 sweep AND the inner f14 loop -- one counter
 *     straddling the inner/outer split.  That single misassignment cost the
 *     whole allocation: `i`'s allocno never reached a global register, local-alloc
 *     gave it r4 (which `-fcall-used-r4` makes CALL-CLOBBERED), and it was
 *     spilled and reloaded at 18 sites around the Random/draw calls, dragging in
 *     two extra frame slots.  Swapping the second sweep onto `i` and the f14 loop
 *     onto `j` put `i` in r8 unspilled, exactly as the ROM has it.
 *
 * (2) `sx` AND `sy` ARE NAMED LOCALS HOISTED OUT OF THE SPAWN LOOP, worth
 *     366 -> 382 (60.8% -> 63.5%).  This is Anim_Ice's lever (1) -- its own
 *     file-mate's -- applied to the same construct: the ROM computes the two
 *     sub-particle seeds ONCE before the `j` loop and keeps them across it,
 *     `ax + 0xc` SPILLED to sp+0xc and `ay << 16` in r11:
 *
 *         adds r6, #0xc            @ ax + 0xc
 *         lsls r5, r5, #16         @ ay << 16
 *         movs r3, #0xff
 *         str  r6, [sp, #0xc]      @ the spill slot IS the declaration
 *         mov  fp, r5
 *         mov  r9, r3
 *
 *     A COMPILER TEMP AT THE BOTTOM OF THE FRAME IS EVIDENCE THAT A VALUE IS
 *     COMPUTED EARLIER THAN ITS USE, i.e. that the source named it.  Written
 *     inline as `q->x = (ax + 0xc) << 16` inside the loop they are re-derived
 *     per iteration.  THE SPLIT MATTERS: `sx = ax + 0xc` with the `<< 16` left
 *     at the STORE is 419, while `sx = (ax + 0xc) << 16` with the shift folded
 *     into the local is 416 -- the ROM adds before the loop and shifts inside.
 *     Their declaration SCOPE is inert (function-scope, block-scope inside the
 *     `else` arm, and `sy` declared before `sx` all measure 419), and naming
 *     only ONE of the pair is WORSE both ways (sx alone 393, sy alone 400) --
 *     so it is the pair, not either half.
 *
 * (3) `int mask = 0xff;` AS A SOURCE LOCAL, worth 360 -> 366.  The ROM parks
 *     0xff in r9 across the spawn loop (`movs r3, #0xff / mov r9, r3`, then
 *     `mov r1, r9 / and r0, r1` at both `Random() & 0xff` sites).  r9 is
 *     otherwise COMPLETELY UNUSED in the un-levered candidate -- the prologue
 *     saves it and nothing writes it -- which is the "one MISSING allocno"
 *     reading of a register rotation, and the landed Anim_Fireball next door
 *     carries the identical `int mask = 0x7f;` for the identical reason.  Where
 *     `mask` is assigned is inert (function top or just before the frame loop,
 *     366 both ways).
 *
 * ALSO LOAD-BEARING, found on the first candidate and never moved:
 *   - `g = iwram_3001eec; pp = g; base = *pp++; ctx = *pp;` -- the bank's
 *     ldmia-walker prologue, verbatim from the landed Anim_Fireball.
 *   - `drawA = g[7]` and `drawB = g[8]`, NOT `gPtrs[0x2e]` / `gPtrs[0x2f]`.
 *     They are the SAME ADDRESSES (gPtrs = 0x03001E50, iwram_3001eec =
 *     0x03001EEC, and 0x3001E50 + 0x2e*4 == 0x3001EEC + 0x1c), but only the
 *     g-relative spelling reuses the iwram_3001eec pool word the way the ROM
 *     does instead of adding a =gPtrs one.  Same trick as Anim_Fireball's
 *     `cam = *(void **)((char *)g - 0x6c)`, with the sign flipped.
 *   - `tt = &ewram_2010158; ... *tt = -1; tt += 7;` -- a walking pointer at the
 *     `t` FIELD, not `ewram_2010140[i].t`.  ewram_2010158 == ewram_2010140 +
 *     0x18 == &parts[0].t, and it is the symbol the ROM pools, so the source
 *     must name it; `tt += 7` on an `int *` is the ROM's `adds r3, #28`.
 *   - `ax = p->x / 8` and `ay = p->y / 8` are NAMED LOCALS COMPUTED BEFORE the
 *     `p->t == -1` test, unconditionally -- Anim_Ice's lever (1) again, and the
 *     signed `/ 8` is what produces the ROM's `cmp/bge/add #7/asr #3`.
 *   - `0x90 << 3` written INLINE at both StartTask calls, not via a shared
 *     local: the ROM REBUILDS `movs r1,#0x90 / lsls r1,#3` at each, the
 *     opposite of Anim_Froth's shared `arg`.
 *   - `(unsigned)t <= 3` (UNSIGNED, `bhi`) beside `t <= 7` (SIGNED, `bgt`) --
 *     the mixed pair is in the ROM and both halves are load-bearing.
 *   - `p->t <= 0xe` re-reads `p->t` from memory after the indirect call, and
 *     `q->t = q->t - 1` likewise -- the bank's re-read idiom.
 *   - `nn = q->t + 1` clamped to 6 with `w = nn * 2` NAMED and
 *     `Data_ede5c[nn - 1]` beside it, so gcc CSEs `w` and derives the byte
 *     index as `w - 2` (`subs r3, r0, #2 / ldrh r1, [r6, r3]`).  This is
 *     Anim_PsyphonSeal's and Anim_Bind's lever transplanted unchanged.
 *   - `*(short *)((char *)q + 2)` / `+ 6` for the two `ldrsh` reads of the high
 *     halves of q->x and q->y -- the family's recorded halfword spelling.
 *   - The two `t <= 3` / `t <= 7` draw arms written as separate calls, which is
 *     what gcc's crossjump pass is supposed to merge (see THE BLOCKER).
 *
 * MEASURED INERT (all still 419, or noted): `DrawFn fns[2]` as an array in
 * place of the two scalars (Anim_Fireball's shape -- 416, i.e. no worse and no
 * better once (1) is in); `i * Lededc[...]` instead of `Lededc[...] * i` at
 * BOTH multiplies (the Thumb 2-address seed does not move); `base` typed `int`
 * with integer address arithmetic (Anim_Fireball's PLUS-canonicalisation lever
 * does NOT reach here); `*(State **)&base[0x7828]` for the state slot;
 * `gp` declared at block scope in each gradient arm; `unsigned char gBuffer[]`
 * with `*(unsigned short *)gp` stores and `gp += 2`.  MEASURED WORSE: storing
 * the gradient value through an `int v` local before the halfword store (329).
 *
 * ================================================================
 * THE BLOCKER, ATTRIBUTED TO A PASS: jump.c's cross-jumping, and it is BLOCKED
 * BY RELOAD -- the two draw arms get DIFFERENT function-pointer registers
 * ================================================================
 *
 * The ONE extra instruction, the ONE extra relocation and 4 of the 4 extra
 * bytes are all the same thing.  The ROM's `p->t != -1` arms share a SINGLE
 * indirect call:
 *
 *     .Lc9b14: cmp r2,#3 / bhi .Lc9b2a      @ (unsigned)t <= 3
 *                <r1 = base+0x844, r2 = ax, r3 = ay, [sp]=[sp+4]=0x18>
 *                b .Lc9b42
 *     .Lc9b2a: cmp r2,#7 / bgt .Lc9b4a      @ t > 7 -> no draw at all
 *                <r1 = base+0xa84, r2 = ax-9, r3 = ay-9, [sp]=[sp+4]=0x2a>
 *     .Lc9b42: ldr r4, [sp, #0x10] / bl _call_via_r4     <- SHARED TAIL
 *
 * -- two argument set-ups falling into one `ldr`+`bl` pair that cross-jumping
 * merged.  We emit the identical two set-ups and the identical two-instruction
 * tail, but ONE arm reloads drawA into r4 and the other into r6, so the tails
 * are not identical insn-for-insn and jump.c cannot merge them.  Hence
 * `_call_via_r6` beside `_call_via_r4`, one extra `ldr`+`bl` pair, and the
 * 4-byte / 1-instruction overshoot.
 *
 * WHAT RULES OUT THE ALTERNATIVES.
 *   - NOT the source's control flow: the arms, the mixed unsigned/signed tests,
 *     the `t > 7` fall-through that skips the call, and the post-call `p->t`
 *     re-read are all already exact -- everything around the two calls aligns.
 *   - NOT cross-jumping being disabled: `-fno-crossjumping` is not a gcc-2.96
 *     option (the driver rejects it), and the pass demonstrably RAN on our
 *     output elsewhere -- the two gradient loops' `b .L18` tails are merged.
 *     It declined here for the register reason above, which is the documented
 *     precondition (identical insn streams), not a pass-level toggle.
 *   - NOT a scheduling difference: `-fno-schedule-insns2` is far worse (372
 *     aligned, 131 hunks) and leaves both calls in place.
 *   - IS reload/local-alloc: the differing operand is which hard register the
 *     drawA RELOAD lands in at each site, with r4 and r6 both free at both.
 *     This is the reload-inheritance class the landed Anim_Confuse and
 *     Anim_Fireball both document -- there the cure was to load the SECOND
 *     function pointer into a named local BEFORE the array assignment, which
 *     is why `DrawFn fns[2]` was measured here; it is inert, so the cure does
 *     not transplant and the remaining spellings are untested, not disproved.
 *
 * TWO SMALLER, SEPARATELY NAMED RESIDUES, both consequences of allocation and
 * both measured to a floor:
 *
 * (A) ONE EXTRA LOOP-INVARIANT HOIST IN THE FIRST GRADIENT LOOP.  The ROM
 *     hoists FOUR values into the loop 1 preheader (0x34, 0xb4, 0x80 and
 *     gBuffer -- the three small constants as WORD pool loads, which is itself
 *     the tell that they are hoisted rather than rebuilt) and loads 0x100 FROM
 *     THE POOL INSIDE the `i > 0x87` arm.  We hoist FIVE, 0x100 included.  That
 *     costs one instruction in the preheader and, worse, adds a pool word ahead
 *     of the jump table so EVERY later `ldr rN, [pc, #imm]` in the function is
 *     off by 4 -- which is where a large share of the 90 hunks comes from, and
 *     why the raw hunk count overstates the real residue here.
 *     MEASURED INERT: `0x80 << 1` for the 0x100; testing `i > 0x87` before
 *     `i <= 0x87` (415, marginally worse); `gp` at block scope.  loop.c's
 *     move_movables threshold is register-pressure-driven and the second
 *     gradient loop, which hoists only THREE, comes out exact -- so this is one
 *     invariant too many in ONE loop, not a systematic difference.
 *
 * (B) A THREE-WAY POINTER ROTATION IN THE PARTICLE-INIT LOOP.  The ROM puts the
 *     walker `p` in r5 and `.Lededc` in r6; we use r7 and r5.  Operand
 *     POSITIONS match everywhere (`ldrb r3, [rTable, rIndex]`, `str r3, [rP,
 *     #k]`), so it is purely which hard register each pseudo received, with r5,
 *     r6 and r7 all free.
 *
 * FLAGS DO NOT REACH ANY OF IT, all measured and all WORSE, diagnostic only:
 * -fno-rerun-cse-after-loop 415 (and 607 instructions, five long);
 * -fno-gcse 409; -fno-schedule-insns2 372; -fno-strength-reduce 362;
 * -fno-expensive-optimizations 364.  Plain -O2 with the production rule is the
 * floor, so a landing needs NO Makefile row.
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
extern unsigned short gBuffer[];
extern Part ewram_2010140[];
extern int ewram_2010158;
extern unsigned short Data_ede5c[];
extern unsigned char Lededc[] __asm__(".Lededc");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int  Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80c91a4(void);
extern void Func_80c9048(void);
extern void Func_80cd52c(void);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void Func_80e3908(Part *p, int a, int b);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Douse(void *context)
{
    void *ctx;
    int frame;
    DrawFn drawB;
    DrawFn drawA;
    void **g;
    void **pp;
    unsigned char *base;
    unsigned short *gp;
    int *tt;
    Part *p;
    Part *q;
    int i;
    int j;
    int mask;
    int sx;
    int sy;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0x2001);
    REG_BG2PA = 0x100;
    LoadVFXFile(FILE_cc, base + 0x604, 1, 1);
    LoadVFXFile(FILE_76, base, 0, 0);
    Func_80c9048();
    REG_BLDCNT = 0x3f44;
    REG_WININ = 0x3337;
    BuildDraw2DFuncEx(0x2e, 7, 7, 2, 2);
    drawA = (DrawFn)g[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 2, 3);
    drawB = (DrawFn)g[8];
    mask = 0xff;
    tt = &ewram_2010158;
    i = 0;
    do {
        i++;
        *tt = -1;
        tt += 7;
    } while (i != (0x80 << 2));
    p = (Part *)(base + (0xe1 << 7));
    i = 0;
    do {
        int r = Random() & 0x3f;
        int yy = -(Lededc[(*(State **)(base + 0x7828))->f18 * 4 + 2] * i + 0x10);
        if ((*(State **)(base + 0x7828))->f4 == 1)
            r = r + yy / 2 - 0x30;
        else
            r = r - yy / 2 + 0x48;
        p->x = r << 3;
        p->y = yy << 3;
        p->t = -1;
        i++;
        p++;
    } while (i != 0x40);
    if ((*(State **)(base + 0x7828))->f4 == 0) {
        i = 0;
        gp = gBuffer;
        do {
            if ((unsigned)(i - 8) <= 0x5f) {
                int h = i / 2;
                *gp = ((0x34 - h) << 8) | (0xb4 - h);
            } else if (i <= 0x87) {
                *gp = 0x80;
            } else {
                *gp = 0x100;
            }
            i++;
            gp++;
        } while (i != 0xa0);
    } else {
        i = 0;
        gp = gBuffer;
        do {
            if ((unsigned)(i - 8) <= 0x5f) {
                int h = i / 2;
                *gp = ((h + 0x3c) << 8) | (h + 0xbc);
            } else if (i <= 0x87) {
                *gp = 0x70f0;
            } else {
                *gp = 0x100;
            }
            i++;
            gp++;
        } while (i != 0xa0);
    }
    StartTask(Func_80c91a4, 0x90 << 3);
    if ((*(State **)(base + 0x7828))->f18 == 0) {
        *(int *)(base + (0xef << 7)) = 1;
        *(int *)(base + 0x7784) = 0;
    } else if ((*(State **)(base + 0x7828))->f18 == 1) {
        *(int *)(base + (0xef << 7)) = 2;
        *(int *)(base + 0x7784) = 0x32;
    } else {
        *(int *)(base + (0xef << 7)) = 2;
        *(int *)(base + 0x7784) = 0x4b;
    }
    StartTask(Task_BlitAnim, 0x90 << 3);
    frame = 0;
    while (frame != Lededc[(*(State **)(base + 0x7828))->f18 * 4 + 3]) {
        if (frame == Lededc[(*(State **)(base + 0x7828))->f18 * 4 + 3] - 0x40)
            _Func_80bd7dc(0x84);
        i = 0;
        if (Lededc[(*(State **)(base + 0x7828))->f18 * 4] != 0) {
            p = (Part *)(base + (0xe1 << 7));
            do {
                int ax = p->x / 8;
                int ay = p->y / 8;
                int t = p->t;
                if (t == -1) {
                    drawA(ctx, base + 0x604, ax, ay, 0x18, 0x18);
                    if (p->y <= 0x27f) {
                        if ((*(State **)(base + 0x7828))->f4 == 0)
                            p->x = p->x - 0x20;
                        else
                            p->x = p->x + 0x20;
                        p->y = p->y + 0x40;
                    } else {
                        p->t = 0;
                        sx = ax + 0xc;
                        sy = ay << 16;
                        j = 0;
                        if (Lededc[(*(State **)(base + 0x7828))->f18 * 4 + 1] != 0) {
                            do {
                                q = &ewram_2010140[Lededc[(*(State **)(base + 0x7828))->f18 * 4 + 1] * i + j];
                                q->x = sx << 16;
                                q->y = sy;
                                q->vx = ((Random() & mask) - 0x80) << 9;
                                if ((*(State **)(base + 0x7828))->f18 == 2)
                                    q->vy = ((Random() & 0x1ff) - 0x180) << 10;
                                else
                                    q->vy = ((Random() & mask) - 0xff) << 10;
                                q->t = (Random() & 0xf) + 0x10;
                                j++;
                            } while (j != Lededc[(*(State **)(base + 0x7828))->f18 * 4 + 1]);
                        }
                        if ((i & 3) == 0)
                            _PlaySound(0x84);
                        j = 0;
                        if ((*(State **)(base + 0x7828))->f14 != 0) {
                            do {
                                Func_80d6888((*(State **)(base + 0x7828))->ids[j], 7, 5, j, 2);
                                j++;
                            } while (j != (*(State **)(base + 0x7828))->f14);
                        }
                    }
                } else {
                    if ((unsigned)t <= 3)
                        drawA(ctx, base + 0x844, ax, ay, 0x18, 0x18);
                    else if (t <= 7)
                        drawA(ctx, base + 0xa84, ax - 9, ay - 9, 0x2a, 0x2a);
                    if (p->t <= 0xe)
                        p->t = p->t + 1;
                }
                i++;
                p++;
            } while (i != Lededc[(*(State **)(base + 0x7828))->f18 * 4]);
        }
        q = ewram_2010140;
        i = 0;
        do {
            if (q->t != -1) {
                int nn = q->t + 1;
                int w;
                if (nn > 6)
                    nn = 6;
                w = nn * 2;
                drawB(ctx, base + Data_ede5c[nn - 1],
                      *(short *)((char *)q + 2) - nn,
                      *(short *)((char *)q + 6) - nn, w, w);
                Func_80e3908(q, 0x3c, 0x80 << 6);
                q->t = q->t - 1;
            }
            i++;
            q++;
        } while (i != (0x80 << 2));
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Func_80c91a4);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
    Func_80c9048();
}
