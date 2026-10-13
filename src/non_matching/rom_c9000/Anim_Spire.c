/* Anim_Spire -- 0x080e2538, the SECOND of the two functions in
 * asm/rom_c9000/rom_e0564_c.s (Anim_Thor is the first and is unattempted).
 *
 * NON-MATCHING, 36 of 432 encodings differ.  WAS 249 (batch 347 brief E).
 * THE LENGTH GAP IS CLOSED: 384 = 384 16-bit instructions, 46 = 46 32-bit
 * entries, 956 = 956 bytes, and the aligned residue is 31 in 14 hunks
 * (tools/aligncmp.py: 407 of 432 aligned-equal, 94.2%).  Both figures moved the
 * same way for once: 249 -> 36 and aligned 64 -> 31.
 *
 * ---------------------------------------------------------------------------
 * THE HEADER THIS REPLACES SAID 249 WAS A TRUE DISTANCE.  IT WAS NOT, AND THAT
 * SENTENCE IS WHY THIS PARK SAT.
 * ---------------------------------------------------------------------------
 * It read "SIZE is the ROM's (956 = 956) and INSTRUCTION COUNT is the ROM's
 * (432 = 432), so 249 is a TRUE distance".  432 is objcmp's STREAM figure: it
 * counts pool words, `bl` calls and `.short 0x0000` PADS alongside the 16-bit
 * instructions.  The reference carried TWO pads and the old body ONE, so a pad
 * absorbed exactly one extra instruction and left both the stream length and
 * the byte size equal while the instruction counts were 384 against 385.
 * objcmp prints `INSTRUCTION COUNT  ref 384, ours 385` on the line DIRECTLY
 * ABOVE the figure, plus its own NOTE that a pad is absorbing the difference.
 *
 * WHAT THE FALSE CLAIM CAUSED THE PARK TO CONCLUDE.  Having declared the figure
 * trustworthy, the header then reasoned about the residue -- and reached for
 * RELOAD rather than for the one-instruction surplus it had itself spotted two
 * paragraphs earlier ("ONE extra instruction at index 185").  Its verdict was
 * "This is reload's inheritance decision (reload_reg_reaches_end /
 * reload_inheritance_insn), not a source construct, so the lever -- if there is
 * one -- is whatever changes `last`'s reload class or its death point.  It is
 * NOT the REG_ALLOC_ORDER class."  Every word of that is refuted:
 *   - IT IS A SOURCE CONSTRUCT.  Deleting the `last` variable -- writing
 *     `Leecf7[cnt - 1]` at both sites instead -- removes the extra instruction
 *     and takes the figure 249 -> 36 in one edit.
 *   - AND NO INHERITANCE PASS COULD HAVE REACHED IT ANYWAY.  The loop top is a
 *     real CODE_LABEL (`.L19` in the generated asm, the back edge's target), and
 *     a reload register's contents are discarded at every label:
 *     `reload1.c:3977-3978` CLEAR_HARD_REG_SET (reg_reloaded_valid) under the
 *     comment "A reload reg's contents are unknown after a label", which is the
 *     gate `choose_reload_regs` tests at `reload1.c:5339-5340`.  reload_combine
 *     refuses to cross one too (`reload1.c:8417-8420`, "We cannot do our
 *     optimization across labels"), and reload_cse's cselib forgets everything
 *     at a CODE_LABEL (`simplify-rtx.c:3180-3181`).  A park that names reload
 *     inheritance across a loop-top label is naming a mechanism that does not
 *     exist.
 * The park's recorded-negative list was produced under the same premise and two
 * of its three rows are now VOID, because they are about a variable that no
 * longer exists ("`frame = 0;` before `last = cnt - 1;` -- 434 instructions" and
 * "`last` declared before `cnt` -- 252").
 *
 * ---------------------------------------------------------------------------
 * THE LEVER: `cnt - 1` IS NOT A SOURCE VARIABLE.  IT IS gcc's OWN CSE OF A
 * REPEATED SUBEXPRESSION, AND NAMING IT DESTROYED THE REUSE.
 * ---------------------------------------------------------------------------
 *     was    last = cnt - 1;
 *            while (frame != Leecf7[last] + 0x50) {
 *                if (frame == Leecf7[last] + 0x30) {
 *     is     while (frame != Leecf7[cnt - 1] + 0x50) {
 *                if (frame == Leecf7[cnt - 1] + 0x30) {
 * with the `int last` declaration deleted.  249 -> 36, 385 -> 384 instructions,
 * aligned 375 -> 407 of 432, hunks 29 -> 14.
 *
 * WHY, READ OUT OF `.18.greg`.  Spelled as a local, `last` gets a low-numbered
 * DECL pseudo that lands in this function's spilled frame-map group -- the
 * dispositions printed no reg_renumber for 33, 34, 35, 36, 37, 157 and 335, and
 * those are exactly the declaration list the frame map mirrors (pos1 0x2c,
 * pos2 0x20, base 0x1c, ctx 0x18, fn 0x14, cnt 0x10, last 0xc).  Each of its
 * three uses then takes its own reload, and the one at the loop top cannot
 * inherit the guard's register because of the label, so it pays
 * `ldr r0, [sp, #0xc]`.  Left as a repeated expression the index becomes a
 * HIGH-numbered cse temporary (the spilled set becomes 33, 34, 35, 36, 158, 328,
 * 338 -- 37 is gone, 328/338 are cse's), it still occupies sp+0xc and the frame
 * is still `sub sp, #0x38`, and the guard now reaches the loop top with the
 * ROM's own register roles:
 *     ldr r2,[sp,#0x10] / mov r1,#0 / sub r2,#1 / mov fp,r1 / str r2,[sp,#0xc]
 *     ldr r1,=.Leecf7 / mov r4,#0x50 / ldrb r3,[r1,r2] / neg r4,r4 / cmp r3,r4
 *     bne .LCB / b .exit / .LCB: .Ltop: ldrb r3,[r1,r2]
 * byte-for-byte, base in r1 and index in r2 as the ROM has them, with no load at
 * the top.  Note the shape of the error: the OLD body also carried the symbol
 * across the label without reloading it, which is what made "inheritance" look
 * plausible -- but that register is an ALLOCATED pseudo, not a reload, and only
 * the index was spilled.
 *
 * > GENERAL FORM, and it is the join-point twin of the note in
 * > src/non_matching/rom_8a000/808d0c8.c ("the ROM's four redraw entry points are
 * > gcse's own PRE hoist, not source variables; naming them destroyed them"):
 * > A VALUE THE ROM APPEARS TO HOLD IN A VARIABLE MAY BE A CSE TEMPORARY, AND
 * > NAMING IT MOVES IT INTO THE DECL/FRAME-MAP GROUP WHERE IT IS SPILLED PER USE.
 * > The tell is a reference that reloads the value at some uses and not others
 * > across a label -- a DECL pseudo cannot do that and a cse temporary can.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Spire.c \
 *     asm/rom_c9000/rom_e0564_c.s --func Anim_Spire
 *   (residue view, and the number to rank variants by:
 *    docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *      goldensun-build python3 tools/aligncmp.py \
 *      src/non_matching/rom_c9000/Anim_Spire.c \
 *      asm/rom_c9000/rom_e0564_c.s Anim_Spire -v
 *    per-REGION instruction vs stream delta: the same two paths through
 *    scratch_elev/b347/E/e_region.py)
 *   THE REFERENCE KEEPS A LITERAL POOL INSIDE THE FUNCTION (the `.align 2, 0 /
 *   .Le25a0: .word 0x100 / .Le25a4: .word 0 / .pool` block after the
 *   `b .Le25b8` at +0xa0), so `tryc.py --align` is USELESS here -- objcmp only.
 *
 * SHIMS: none -- `python3 tools/shimcount.py` is clean, 0 pins, no fakematch row.
 *
 * SPLIT SHAPE IF IT LANDS -- NOT RUN NOW, because install_batch.py refuses a
 * `split` on an entry whose figure is not 0 and is right to: a split for a park
 * is tree churn plus promoted `.global` labels for no match.
 * `python3 tools/datacheck.py asm/rom_c9000/rom_e0564_c.s`:
 *     data sections : .rodata
 *     functions     : Anim_Thor, Anim_Spire
 *     EXPORTS       : .Leec5f .Leec63 .Leec68 .Leec70 .Leec74 .Leec7d .Leec86
 *                     .Leec98 .Leeca1   (already global -- NOT the set a split needs)
 *     Anim_Spire reads .Leecb2 .Leecf2 .Leecf7 .Leecfc .Leecff .Leed0e .Leed1e
 *     *** SPLIT MUST EXPORT: .global for all SEVEN of those
 * So TWO reasons for split_s.py, not one: the piece holds two functions and only
 * this one is parked, AND the `.s` defines nine non-function symbols that
 * converting whole would delete (piece.py's `!! EXPORTS 9 NON-FUNCTION
 * SYMBOL(S)`; a gate went red on exactly this in batch 345).  The .rodata keeps
 * its own object beside Anim_Thor and gains seven `.global` lines; this file
 * reads all seven as `extern ... __asm__(".LeecXX")`.  Types, all read off the
 * ROM: .Leecf2 is SIGNED char (`ldrsb`), .Leecb2 / .Leecf7 / .Leecfc / .Leecff /
 * .Leed0e unsigned char (`ldrb`), .Leed1e unsigned short (`ldrh`, doubled index).
 * And ignore "neither ships alone": split_s.py cuts before/target/after, so
 * Anim_Thor and Anim_Spire each split out ALONE.
 *
 * ================================================================
 * THE THREE LEVERS THAT TOOK IT 317 -> 249 ARE UNCHANGED AND STILL LOAD-BEARING
 * ================================================================
 * 317 -> 315  THE PARTICLE-SEED LOOP'S BASE IS `boff + i * 0x8c`, NOT
 *   `&gBuffer[i * 0x15]`.  r7 starts at 0 and gains 0xe0 << 1 per outer
 *   iteration, and `movs r3,#0x8c / muls r4,r3` adds 5 Parts more.  A single
 *   `&gBuffer[i * 0x15]` gives one `muls` by 0x24c and no giv at all.
 * 315 -> 314  THE BLIT'S THREE TABLES ARE READ IN ARGUMENT ORDER, VIA EMBEDDED
 *   ASSIGNMENTS (Anim_Flare's pool-ORDER lever).  Assigning w and h as
 *   statements before the call gives Leecff the first `ldr` where the ROM gives
 *   it to Leed1e.
 * 314 -> 249  THE Leecb2 BASE MUST BE A NAMED POINTER HOISTED OUT OF THE OUTER
 *   LOOP.  `tbase = Leecb2;` in the preheader, `tp = tbase;` in the body; a
 *   `ptr = SYMBOL;` written INSIDE the loop leaves the `ldr` in the body where
 *   the ROM keeps &Leecb2 in sl for the whole loop.
 *
 * WHAT IS ALREADY RIGHT AND MUST NOT BE DISTURBED
 *   - `pp = tbl; base = *pp++; slot = (State **)(base + 0x7828); ctx = *pp;`
 *     -- `base` is SPILLED (sp+0x1c) and `slot` lives in r8; the ROM computes
 *     the 0x7828 address ONCE here and re-derives it from `base` later, so the
 *     knockback loop must NOT reuse `slot`.
 *   - `GetBattleActorPos2((*slot)->ids[(*slot)->f14 - 1], &pos2)`.
 *   - `mid = pos1.x + (pos2.x - pos1.x) / 2; pos1.x = mid;
 *      REG_BG2X = (0x40 - mid) << 8;`
 *   - `REG_BG2PA = 0x100; REG_BLDCNT = 0;` -- BOTH become pool words on Thumb,
 *     which is what puts two `.word`s in the mid-function pool.
 *   - the frame-loop test as a `while`, so duplicate_loop_exit_test manufactures
 *     the entry guard the ROM has (`movs r4,#0x50 / negs r4,r4 / cmp r3,r4`).
 *   - TWO SEPARATE `if`s, not an if/else, around the k loop: the ROM RE-TESTS
 *     after the k loop (`cmp r11,r3 / bge / b`) instead of jumping.
 *   - the k loop INDEXES, it does not walk: `Part *g = &gBuffer[i * 0x15 + k];`
 *     recomputed every iteration, with `i * 4` CSE'd to sp+8.
 *   - `Leecf2[i]` and `Leecf7[i]` stay INLINE inside the inner loops: the loops
 *     call Random / __modsi3 / the blit, so LICM cannot hoist a load across
 *     them -- and the ROM does not.
 *   - `((u32)Random() % 0x60 - 0x30)` for `bl __umodsi3` (UNSIGNED) against
 *     `k % 5` and `g->t / 0x60 % 3` for `bl __modsi3` / `bl __divsi3` (signed).
 *   - `q->y += q->vy;` read BEFORE the `if (frame > Leecf7[i])` that adds
 *     0x80 << 9 -- the ROM reuses the OLD q->vy in r2 for the second store.
 *   - THE DECLARATION LIST IS THE FRAME MAP, DESCENDING: pos1 0x2c, pos2 0x20,
 *     base 0x1c, ctx 0x18, fn 0x14, cnt 0x10, then gcc's OWN cse temporary at
 *     0xc (where `last` used to sit) and its `i * 4` at 0x8, `sub sp, #0x38`.
 *
 * ================================================================
 * WHAT IS LEFT: 36 IN 14 HUNKS, ALL OF IT REGISTER NAMING AND FOUR LOCAL
 * SCHED2 ORDERINGS.  NO LENGTH ERROR ANYWHERE: e_region.py reports i16 +0 and
 * stream +0 with every hunk netting zero.
 * ================================================================
 *  1. stream 102-108, the seed loop's preheader.  The ROM delays
 *     `adds r3, r4, r0` (p = base + (0xe1 << 7)) until AFTER the pool load of
 *     0xffc00000 and the `movs r2, #0`; ours emits it first and can then reuse
 *     r1 for the pool word because base has died.  Also the reload register for
 *     the spilled `base` is r4 in the ROM and r1 here, with the 0xe1 temp r0
 *     against r2.  Order and register choice are coupled through that reuse.
 *  2. stream 123-132, the same block one loop later.  Identical instructions in
 *     a different order: the ROM emits `movs r3,#0 / mov r8,r3` FIRST, ours
 *     emits the pair LAST, after `adds r5, r3, r2`.  `k = 0;` written as the
 *     first statement of the loop body (inside an extra brace, so the two
 *     block-scope declarations still follow it) is WORSE, 56 and aligned 394.
 *  3. stream 234-239 and 251-264, the blit arguments.  Ours hoists the
 *     `ldrsh r2,[r7,#2]` above the Leecff load and the .Leed0e pool load
 *     earlier than the ROM does.  Forcing the evaluation order with comma
 *     expressions -- `(w = Leecff[idx], *(short *)((char *)g + 2) - (w >> 1))`
 *     -- is inert (37 for w alone, 36 for w and h, both with MORE hunks: 17 and
 *     18 against 14).  The ROM's order is asymmetric between the two arguments
 *     (table-then-field for w, field-then-table for h), which is sched2's
 *     output and not an evaluation rule.
 *  4. stream 373-378, an r3/r4 exchange on `movs #1 / movs #0x1c / add r9 /
 *     add sl`.
 * So the class is "right instructions, wrong registers" plus post-reload
 * scheduling ties, with the one reload-register choice at (1) upstream of most
 * of it.
 *
 * MEASURED INERT OR WORSE ON *THIS* BODY (the old list's `last` rows are void):
 *   `boff = i;` instead of `boff = 0;` to share the ROM's single zero .... 36,
 *     and BIT-IDENTICAL output -- cse folds i to 0, so this row measures nothing
 *   `p->vy = 0;` before `p->y = 0xffc00000;` in the seed loop ........... 38
 *   `tp = tbase;` before the `g` initialiser ............................ 37
 *   `k = 0;` as the first statement of the outer loop body .............. 56
 *   comma-expression `w` only ........................................... 37
 *   comma-expression `w` and `h` ........................................ 36 (18 hunks)
 * BOTH CHEAP AXES ARE CLOSED ON THIS BODY, re-swept after the edit because the
 * old sweep measured a different program (tools/autosweep.py):
 *   ANCHOR  all 79 of 79 statement boundaries inert or worse (best 36 = base).
 *   FLAG    -fno-strict-aliasing 39, -fno-strength-reduce 91,
 *           -fno-rerun-loop-opt 125, -fno-schedule-insns2 162,
 *           -fno-rerun-cse-after-loop 274, -fno-gcse 280, -O1 345,
 *           -ffixed-r7 393.  None beats 36 and every one of them moves the
 *           instruction count or the aligned residue the wrong way.
 * (Caution worth carrying: autosweep prints "Both cheap axes are CLOSED" even
 *  when invoked with only `--flags` or only `--anchor`.  It means the axis it
 *  ran.)
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
    frame = 0;
    while (frame != Leecf7[cnt - 1] + 0x50) {
        if (frame == Leecf7[cnt - 1] + 0x30) {
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
