/* Anim_Ice -- 0x080c91dc, 586 instructions.  PARKED.
 *
 * NON-MATCHING, 367 of 619 encodings differ.  THAT FIGURE IS SATURATED AND
 * RANKS NOTHING: SIZE is NOT exact (ref 1360 bytes, ours 1344) and the
 * instruction COUNT is NOT exact (ref 619, ours 611 -- EIGHT SHORT), so objcmp
 * compares index-by-index past a deletion and its number is meaningless as a
 * distance.  Every ranking below is tools/aligncmp.py's figure instead:
 *
 *     aligned-equal 455 (73.5% of ref), 190 differing/ins/del in 83 hunks
 *
 * RELOCATIONS: the SYMBOL SEQUENCE IS ALREADY IDENTICAL -- all 41 relocations,
 * same symbols in the same order, on the FIRST candidate.  Only the offsets
 * differ, and they differ only because we are 16 bytes short.  objcmp's
 * "RELOCATIONS differ" line here is the offset half of that finding, not the
 * sequence half; read them separately.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Ice.c \
 *     asm/rom_c9000/rom_c91dc_a.s --func Anim_Ice
 *
 * SPLIT SHAPE: TEXT-ONLY, NO NEW EXPORTS.  tools/datacheck.py on
 * asm/rom_c9000/rom_c91dc_a.s is silent -- the file has NO data section.  It
 * holds TWO functions, Anim_Ice (first) and Anim_Douse (0x080c972c), so
 * converting one still needs a split: tools/split_s.py leaves Anim_Ice in
 * rom_c91dc_a_b.s and Anim_Douse in rom_c91dc_a_c.s, and the .c takes
 * src/rom_c9000/rom_c91dc_a_b.c.  NOTHING needs a new `.global`:
 *   - `.Leded6` is ALREADY `.global` in asm/rom_c9000/rom_c91dc_c_c_c_c_c_c.s:649
 *   - `Data_ede84` / `Data_ede96` are `.incdata` exports in asm/rom_c9000/rom_eda78.s:33-34
 *   - Anim_Douse reads NONE of those three labels, so the remainder loses nothing.
 * NO SHIMS, NO PINS: tools/shimcount.py is clean (checked against Anim_Froth.c,
 * a known-clean file, to confirm the silent-means-clean output form).  No
 * volatile beyond io.h's registers, no "+r" barrier, no do{}while(0), no .equ,
 * no per-file flag override.
 *
 * WARNING FOR WHOEVER PICKS THIS UP: split_s.py HAS NO --dry-run.  It writes
 * asm/ and rewrites stage1.ld the moment you run it.  Passing --dry-run does
 * not stop it -- the flag is ignored and the tree is modified.
 *
 * ================================================================
 * THE LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * The first candidate was already 429 aligned-equal (69.3%) with the relocation
 * sequence exact, so everything below is the last 26.
 *
 * (1) `ax` AND `ay` ARE NAMED LOCALS COMPUTED BEFORE THE `(unsigned)(p->t - 1)
 *     <= 0xd` TEST, worth 430 -> 450 (69.5% -> 72.7%) AND +2 instructions.
 *     THE BIGGEST LEVER IN THE FUNCTION.  The ROM computes `p->x / 8` and
 *     `p->y / 8` at the TOP of the `p->t != -1` block -- UNCONDITIONALLY, before
 *     the test that guards their only use -- and then SPILLS ax to sp+0xc across
 *     the `__divsi3` call:
 *
 *         mov  r1, #3
 *         str  r2, [sp, #0xc]      @ ax spilled
 *         bl   __divsi3
 *         ...
 *         ldr  r2, [sp, #0xc]      @ ax reloaded as argument 3
 *         mov  r3, r6              @ ay survived in a callee-saved register
 *
 *     Written inline as call arguments the two divisions sink INSIDE the `if`
 *     and land AFTER __divsi3, so there is nothing to spill, sp+0xc never
 *     exists and the frame comes out 8 bytes short.  This is the
 *     spill-slot-is-a-declaration rule read backwards: A COMPILER TEMP AT THE
 *     BOTTOM OF THE FRAME IS EVIDENCE THAT A VALUE IS COMPUTED EARLIER THAN ITS
 *     USE, i.e. that the source named it.  Their declaration SCOPE is inert
 *     (block-scope and function-scope measure identically, 455 both ways); only
 *     the POSITION of the computation matters.
 *
 * (2) NAME THE HALF-WIDTH, NOT THE WIDTH, IN THE TAIL LOOP, worth 450 -> 453
 *     and +6 instructions.  The ROM reads `Data_ede96[n]` TWICE:
 *
 *         ldrb r3, [r6, r0]        @ first read -> lsr r1, r3, #1
 *         lsr  r1, r3, #1
 *         ...
 *         ldrb r3, [r6, r0]        @ SECOND read, for the two stack arguments
 *         str  r3, [sp]
 *         str  r3, [sp, #4]
 *
 *     so the source cannot hold it in a local.  `unsigned int h =
 *     Data_ede96[n] >> 1;` with `Data_ede96[n]` written out again for arguments
 *     5 and 6 reproduces both reads.  A single `unsigned int w` used for all
 *     three sites reads the table once and keeps w in r6, which is one
 *     instruction short and costs r6.
 *
 * (3) THE `>> 1` MUST BE UNSIGNED, and the `- 0xf` MUST NOT BE FOLDED THROUGH
 *     THE SHIFT.  Two one-line corrections, together worth 429 -> 430, and
 *     INVISIBLE TO OBJCMP -- all three of v1/v2/v3 report the identical
 *     "367 of 619" because the count was already saturated.  Only aligncmp saw
 *     them.
 *       - `Data_ede96[n] >> 1` on an `int` gives `asr`; the ROM has `lsr`, so
 *         the carrier must be `unsigned int`.  Data_ede96 is `unsigned char`,
 *         which PROMOTES TO INT and re-signs the shift -- the promotion is the
 *         trap, not the array's type.
 *       - `((Random() & 0xf) + p->y / 8 - 0xf) * 8` is folded by `associate:`
 *         into `sum * 8 - 0x78` (`subs r3, #120`).  The ROM has `subs r3, #15`
 *         THEN `lsls r3, #3`, so write `<< 3` instead of `* 8`: fold does not
 *         distribute a constant through LSHIFT_EXPR the way it does through
 *         MULT_EXPR.  This is batch 298's "distribute the shift by hand" lever
 *         running in the OTHER direction -- here the job is to STOP the fold,
 *         and `<< 3` is the spelling that does it.  Note the sibling
 *         `q->x = (... ) * 8 + 8` is NOT affected: with a trailing `+ 8` there
 *         is no constant for fold to carry through the multiply.
 *
 * (4) `p->y = y;` BEFORE `p->x = v << 3;` IN THE INIT LOOP, worth 453 -> 455.
 *     The ROM's two stores are `str r1,[r6,#4]` then `str r3,[r6,#0]` -- offset
 *     4 before offset 0.  Pure scheduling, but it is a free 2.
 *
 * (5) `fp = fns;` MUST COME AFTER `f1` IS LOADED, worth 438 -> 455 the other
 *     way.  The cd508_Confuse device transfers whole -- `fns[0]` written through
 *     the ARRAY, `f1 = gPtrs[0xbc/4]` into its own local, THEN `fp = fns;
 *     fp[1] = f1;` -- and moving `fp = fns;` one statement earlier costs 17
 *     aligned encodings.  The ROM's order is visible directly:
 *         ldr r2,[gPtrs+0xb8] / str r2,[sp,#0x1c] / ldr r3,[gPtrs+0xbc]
 *         / add r1,sp,#0x1c   / str r3,[r1,#4]    / mov r11,r1
 *     i.e. fns[0] by slot, then f1, then fp, then fns[1] THROUGH fp.
 *
 * ALSO LOAD-BEARING, right on the first candidate and never moved:
 *   - `pp = g; base = *pp++; ctx = *pp;` for the `ldmia r3!, {r1}`, with the
 *     THIRD table entry read separately as `g[2]` (it is the sheet that the
 *     second LoadVFXFile fills and the tail loop draws from).
 *   - ONE `i` SHARED ACROSS SIX DISJOINT LOOPS -- the two init loops, both
 *     palette loops, the inner particle loop and the tail loop.  The ROM keeps
 *     every one of them in r8; gcc-2.96 gives one hard register per variable and
 *     never splits a live range, so six loops in r8 is one source variable.
 *     This is the Anim_Whirlwind / Anim_Break counter rule, sixfold.
 *   - `REG_BLDCNT = 0x3f44;` THEN `REG_WININ = 0x3337;` in that order, with no
 *     int carrier.  cse then derives the second address from the first's
 *     register as `sub r2, #8` (0x4000050 -> 0x4000048), which is exactly what
 *     the ROM has.  Writing WININ first loses the derivation.
 *   - NO SHARED LOCAL FOR THE StartTask ARGUMENT.  Both sites emit `mov r1,#0x90
 *     / lsl r1,#3` because that IS how gcc-2.96 synthesises 0x480, so plain
 *     `0x90 << 3` at both calls is right.  Anim_Froth needed a shared `arg`
 *     local; this one does not, and the tell is that the ROM recomputes it.
 *   - TWO SEPARATE STORES for base+0x7784 (0x4b / 0x32) and for base+0x77ac's
 *     +-0x40, never a `?:` -- the ROM computes the address in BOTH arms and
 *     cross-jumps only the `str`.  Anim_Flare's rule, twice.
 *   - RE-DERIVE `base + 0x7828` EVERYWHERE; DO NOT NAME IT.  Anim_Froth's lever
 *     (5) holds here and was re-measured from scratch: naming a `State **st` for
 *     the inner-loop region costs 455 -> 435 and +2 instructions.  The plain
 *     `(*(State **)(base + 0x7828))` spelling by itself already gets the
 *     FRAME-loop hoist the ROM has in r5.
 *   - The two palette loops are separate `if`/`else` bodies, not one loop with a
 *     conditional value, and their accumulators are loop.c GIVS from
 *     `(0x70 - i) << 8` and `(0x18 + i) << 8`.  The ROM's `mov r1,#0xe0 /
 *     lsl r1,#7` IS 0x7000 = 0x70<<8, and `mov r1,#0xc0 / lsl r1,#5` IS
 *     0x1800 = 0x18<<8 -- both giv initialisers, not source constants.
 *   - The pooled `0x100` and the synthesised `0x100` in the SAME palette loop
 *     are the HImode/SImode tell, confirmed here for the third time: the stored
 *     `pal[i] = 0x100` pools (Thumb-1 has no PC-relative LDRH, so gas assembles
 *     `ldrh` to `ldr`), while the giv's `+0x100` step is SImode and comes out as
 *     `mov r0,#0x80 / lsl r0,#1`.  Do not "fix" either one.
 *
 * MEASURED INERT (all still 455, and several bit-identical): `ax`/`ay` at
 * function scope; `n`/`h` at function scope; `(unsigned)(i - 8) < 0x60` for
 * `<= 0x5f`.
 * MEASURED WORSE: `(Random() & 7) + (Random() & 0x3f)` with the masks swapped
 * (453 -- the 0x3f call is evaluated FIRST); a named `int hi = 0xf0;` for the
 * palette loop's reverse-subtract constant (446 -- this is the CONVERSE of
 * Anim_Froth's `int mask = 0xff;` lever, and the two functions are the
 * controlled pair on it); `fp = fns;` moved one statement earlier (438); a
 * named `State **st` (435).
 *
 * ================================================================
 * THE BLOCKER: TWO RESIDUES, BOTH IN REGISTER ALLOCATION, AND THEY ARE ONE
 * ================================================================
 *
 * RESIDUE A -- THE PRE-SCALED 0/4 FUNCTION-POINTER SELECT, 3 sites, 3 of the 8
 * missing instructions.  At all three `fns[]` calls the ROM materialises a
 * BYTE OFFSET and uses a register+register load:
 *
 *         mov  r1, #4
 *         cmp  r3, #2
 *         beq  .L
 *         mov  r1, #0
 *     .L: mov  r3, r11             @ fp, a high register, copied down
 *         ldr  r4, [r1, r3]
 *
 * That is `expand_expr` on a COND_EXPR whose ARMS ARE 4 AND 0 -- i.e. the
 * ARRAY_REF's x4 scale was distributed INTO the arms before expand.  We emit
 * the ADDRESS select instead (`mov r4,r9 / cmp / bne / add r4,#4` then
 * `ldr r4,[r4]`), one instruction shorter, at every site.
 *
 * ATTRIBUTED TO fold, NOT TO RELOAD.  gcc-2.96's `fold` simplifies
 * `COND_EXPR(cmp, 1, 0)` to the bare comparison, and it does that to the INDEX
 * SUBEXPRESSION before the ARRAY_REF's scale is applied.  So by the time the x4
 * exists the index is an EQ_EXPR, `expand_expr` routes it through
 * `emit_store_flag`, and the distribution rule never sees a COND_EXPR.  RULES
 * OUT THE ALTERNATIVES:
 *   - `fp[cond ? 1 : 0]` is BIT-IDENTICAL to `fp[cond]` (verified by diffing the
 *     compiled .s, not by objcmp -- objcmp reported the same saturated 361 for
 *     both and could not have told them apart).  fold collapses it.
 *   - `*(DrawFn *)((char *)fp + (cond ? 4 : 0))`, which writes the ROM's offset
 *     select out by hand, is ALSO BIT-IDENTICAL: gcc canonicalises the
 *     pointer-plus-offset back into the address select.  So the shape is not
 *     reachable from the index side OR the pointer side.
 * This is a fold/expand boundary, not a spelling, and it is the same class as
 * the recorded "gcc-2.96 never chains plain CONST_INTs" finding: a constant
 * folded one way at tree level cannot be un-folded by naming.
 *
 * RESIDUE B -- THE INNER LOOP'S OWN HOIST OF `base + 0x7828`, ~4 instructions,
 * AND IT IS WHY ctx IS NOT SPILLED.  The ROM hoists that address TWICE: once
 * into r5 for the frame loop (WE GET THIS ONE FOR FREE) and AGAIN into r9 for
 * the inner particle loop, where it serves four reads as `mov r1,r9 / ldr r3,[r1]`
 * instead of our three-instruction `ldr r3,[pc] / add r3,sl / ldr r3,[r3]`.
 * loop.c hoists the invariant into EACH loop's preheader, so the ROM carries
 * ONE EXTRA ALLOCNO that we do not.
 *
 * AND THE FRAME PROVES IT.  The ROM's frame is `sub sp, #0x24`; ours is
 * `sub sp, #0x1c`.  The ROM's map is
 *
 *     0x00-0x08  outgoing arguments
 *     0x0c       ax, spilled across __divsi3        (lever 1 recovers this one)
 *     0x10       sheet
 *     0x14       frame
 *     0x18       ctx          <-- WE KEEP THIS IN r11
 *     0x1c-0x20  fns[2]
 *
 * and ctx at 0x18 is the whole story: the ROM SPILLS ctx, which frees a high
 * register for the inner-loop hoist; we give ctx r11 and there is then no high
 * register left for r9, so loop.c's hoist is not made.  Declaration order is
 * already the ROM's (the aggregate `fns` takes the top, then ctx, frame, sheet
 * descend), so THIS IS NOT AN ORDERING PROBLEM -- it is allocno_compare
 * preferring ctx, whose only movable inputs are n_refs (3), live_length (the
 * whole function) and declaration order (already right).  Residue A and
 * Residue B are therefore ONE BLOCKER seen twice: both are decided after
 * expand, and nothing at the C level moves either.
 *
 * WHERE TO GO NEXT.  The two residues are 7 of the 8 missing instructions and
 * both need a register the allocator will not give up.  The single highest-value
 * experiment is a `-da` dump of global-alloc on this function against the
 * landed Anim_Break in the same bank, which spills its context pointer the way
 * this ROM does; that pair is a controlled experiment on Residue B.  This
 * function is also the second corpus data point (with Anim_Froth) naming
 * REG_ALLOC_ORDER, and if that flag is ever rebuilt this park should be
 * re-screened first -- it is eight instructions and two register decisions away.
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
extern void *gPtrs[];
extern unsigned char gBuffer[];
extern unsigned char Leded6[] __asm__(".Leded6");
extern unsigned char Data_ede96[];
extern unsigned short Data_ede84[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void Func_80c9048(void);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int  Random(void);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80c91a4(void);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Ice(void *context)
{
    DrawFn fns[2];
    void *ctx;
    int frame;
    void *sheet;
    DrawFn *fp;
    DrawFn f1;
    void **g;
    void **pp;
    unsigned char *base;
    Part *p;
    Part *q;
    int i;
    int j;
    int y;
    int t;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    sheet = g[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0x2001);
    REG_BG2PA = 0x100;
    LoadVFXFile(FILE_VFX_ICE, base, 1, 1);
    LoadVFXFile(FILE_VFX_BOREAS_SPARKLE, sheet, 0, 0);
    Func_80c9048();
    REG_BLDCNT = 0x3f44;
    REG_WININ = 0x3337;

    i = 0;
    y = -0x80;
    t = -0x10;
    p = (Part *)(base + (0xe1 << 7));
    do {
        int v = (Random() & 0x3f) + (Random() & 7) + 0x18;
        if ((*(State **)(base + 0x7828))->f4 == 1) {
            v = v + t + 0x18;
        } else {
            v = v - t + 0x50;
        }
        p->y = y;
        p->x = v << 3;
        p->t = -1;
        i++;
        y -= 0x40;
        t -= 8;
        p++;
    } while (i != 0x20);

    i = 0;
    do {
        Part *r = (Part *)(base + (0xe8 << 7)) + i;
        r->t = -1;
        i++;
    } while (i != 0x20);

    if ((*(State **)(base + 0x7828))->f4 == 0) {
        BuildDraw2DFuncEx(0x2e, 7, 7, 2, 2);
        BuildDraw2DFuncEx(0x2f, 7, 7, 2, 3);
    } else {
        BuildDraw2DFuncEx(0x2e, 7, 7, 6, 2);
        BuildDraw2DFuncEx(0x2f, 7, 7, 6, 3);
    }

    fns[0] = (DrawFn)gPtrs[0xb8 / 4];
    f1 = (DrawFn)gPtrs[0xbc / 4];
    fp = fns;
    fp[1] = f1;

    if ((*(State **)(base + 0x7828))->f4 == 0) {
        i = 0;
        do {
            if ((unsigned)(i - 8) <= 0x5f) {
                ((unsigned short *)gBuffer)[i] = ((0x70 - i) << 8) | (0xf0 - i);
            } else if (i <= 0x87) {
                ((unsigned short *)gBuffer)[i] = 0x888;
            } else {
                ((unsigned short *)gBuffer)[i] = 0x100;
            }
            i++;
        } while (i != 0xa0);
    } else {
        i = 0;
        do {
            if ((unsigned)(i - 8) <= 0x57) {
                ((unsigned short *)gBuffer)[i] = ((0x18 + i) << 8) | (i + 0x98);
            } else if (i <= 0x87) {
                ((unsigned short *)gBuffer)[i] = 0x78f8;
            } else {
                ((unsigned short *)gBuffer)[i] = 0x100;
            }
            i++;
        } while (i != 0xa0);
    }

    StartTask(Func_80c91a4, 0x90 << 3);
    *(int *)(base + (0xef << 7)) = 2;
    if ((*(State **)(base + 0x7828))->f18 == 1) {
        *(int *)(base + 0x7784) = 0x4b;
    } else {
        *(int *)(base + 0x7784) = 0x32;
    }
    StartTask(Task_BlitAnim, 0x90 << 3);

    frame = 0;
    while (frame != Leded6[(*(State **)(base + 0x7828))->f18 * 2 + 1]) {
        if (frame == Leded6[(*(State **)(base + 0x7828))->f18 * 2 + 1] - 0x10) {
            _Func_80bd7dc(0x84);
        }
        i = 0;
        while (i != Leded6[(*(State **)(base + 0x7828))->f18 * 2]) {
            p = (Part *)(base + (0xe1 << 7)) + i;
            if (p->t == -1) {
                fp[(*(State **)(base + 0x7828))->f18 == 2 ? 1 : 0](ctx, base, p->x / 8,
                                                           p->y / 8, 0x20, 0x20);
                if (p->y <= 0x27f) {
                    if ((*(State **)(base + 0x7828))->f4 == 0) {
                        p->x = p->x - 0x40;
                    } else {
                        p->x = p->x + 0x40;
                    }
                    p->y = p->y + 0x40;
                } else {
                    if ((i & 3) == 0) {
                        _PlaySound(0x73);
                    }
                    *(int *)(base + 0x77a8) = 2;
                    p->t = 0;
                    j = 0;
                    while (j != (*(State **)(base + 0x7828))->f14) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[j], 9, 5, j, 8);
                        j++;
                    }
                }
            }
            if (p->t != -1) {
                int ax = p->x / 8;
                int ay = p->y / 8;
                if ((unsigned)(p->t - 1) <= 0xd) {
                    fp[(*(State **)(base + 0x7828))->f18 == 2 ? 1 : 0](
                        ctx, base + ((p->t / 3) << 10) + 0x400, ax, ay,
                        0x20, 0x20);
                }
                if ((unsigned)(p->t - 9) <= 2) {
                    int k = 0;
                    q = (Part *)(base + (0xe8 << 7));
                    while (1) {
                        if (q->t == -1) {
                            q->t = 0x12;
                            q->x = ((Random() & 0x1f) + p->x / 8) * 8 + 8;
                            q->y = ((Random() & 0xf) + p->y / 8 - 0xf) << 3;
                            break;
                        }
                        k++;
                        q++;
                        if (k == 0x20) {
                            break;
                        }
                    }
                }
                if (p->t <= 0xe) {
                    p->t = p->t + 1;
                }
            }
            i++;
        }
        i = 0;
        q = (Part *)(base + (0xe8 << 7));
        do {
            if (q->t != -1) {
                if (q->t <= 0x11) {
                    int n = q->t / 2;
                    unsigned int h = Data_ede96[n] >> 1;
                    fp[(*(State **)(base + 0x7828))->f18 == 2 ? 1 : 0](
                        ctx, (char *)sheet + Data_ede84[n], q->x / 8 - h,
                        q->y / 8 - h, Data_ede96[n], Data_ede96[n]);
                }
                if (q->t > -1) {
                    q->t = q->t - 1;
                }
            }
            i++;
            q++;
        } while (i != 0x20);
        Func_80cd52c();
        UpdateScreenShake(4, 4);
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    StopTask(Func_80c91a4);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
    Func_80c9048();
}
