/* Anim_Condemn -- 0x080cfef4, 580 instructions.  PARKED.
 *
 * NON-MATCHING, 555 of 623 encodings differ.  THAT FIGURE IS SATURATED AND
 * RANKS NOTHING: SIZE is NOT exact (ref 1396 bytes, ours 1372) and the COUNT is
 * NOT exact (ref 623, ours 612 -- ELEVEN SHORT).  tools/aligncmp.py reads
 *
 *     aligned-equal 292 (46.9% of ref), 427 differing/ins/del in 120 hunks
 *
 * RELOCATIONS: the SYMBOL SEQUENCE IS ALREADY IDENTICAL -- all 52 relocations,
 * same symbols in the same order, ON THE FIRST CANDIDATE.  Only the offsets
 * differ.  Read objcmp's "RELOCATIONS differ" line as the offset half only.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Condemn.c \
 *     asm/rom_c9000/rom_cfef4.s --func Anim_Condemn
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT NEEDED, TWO NEW EXPORTS.  tools/datacheck.py on
 * asm/rom_c9000/rom_cfef4.s reports a .rodata section and SIX functions
 * (Anim_Condemn -- first -- Anim_Unused_ScreenMelt, Anim_Bind, Anim_PsyphonSeal,
 * Anim_AstralBlast, Anim_ShiningStar).  Anim_Condemn reads exactly two data
 * labels, so the split
 *     *** MUST EXPORT: .global .Lee10c
 *     ***              .global .Lee11a
 * and nothing else.  Both are `.incrom` blobs of FOURTEEN bytes each
 * (0xee10c..0xee11a and 0xee11a..0xee128) -- two rows of seven, which is what
 * the `c * 7 + n` / `d * 7 + n` indexing below is reading.  Leave them in the
 * remainder's .rodata; do NOT emit them from C.
 * NO SHIMS, NO PINS: tools/shimcount.py is clean.
 *
 * WARNING: tools/split_s.py HAS NO --dry-run.  It rewrites asm/ and stage1.ld
 * the moment you run it; the flag is ignored, not honoured.
 *
 * ================================================================
 * THE HEADLINE FINDING: A VALUE ASSIGNED 0 ON *BOTH* PATHS IS NOT PROPAGATED,
 * AND THAT IS WORTH 22 INSTRUCTIONS
 * ================================================================
 *
 * The seven draw calls index two 7-byte rows as `Lee10c[c * 7 + n]` and
 * `Lee11a[d * 7 + n]`, and the ROM computes BOTH products at runtime:
 *
 *         lsl  r3, r7, #3
 *         sub  r3, r7              @ c * 7
 *         ldrb r3, [r1, r3]
 *
 * even though `d` is provably 0 -- the ROM's own code sets r9 to zero in both
 * arms of the `f4` test and never changes it, so `d * 7 + n` is just `n`.
 * Written the obvious way,
 *
 *         if (f4 == 0) { c = 0; } else { c = 1; }
 *         d = 0;                                   <-- ONE definition
 *
 * `d` has a SINGLE reaching definition, gcse's cprop folds it, and all seven
 * `Lee11a` accesses collapse to `ldrb r3, [r3, #0]`.  The candidate then comes
 * out THIRTY-SEVEN instructions short.  Moving the assignment INSIDE BOTH ARMS
 *
 *         if (f4 == 0) { c = 0; d = 0; } else { c = 1; d = 0; }
 *
 * gives it TWO reaching definitions.  gcc-2.96's cprop requires a single
 * reaching def and does NOT value-number two equal constants across a join, so
 * the multiply survives at every site: 586 -> 608 instructions, +22, and the
 * shortfall drops from 37 to 15.
 *
 * SO: A ROM THAT COMPUTES A PROVABLY-CONSTANT INDEX AT RUNTIME IS TELLING YOU
 * THE VALUE IS ASSIGNED ON MORE THAN ONE PATH.  This is the cprop counterpart of
 * the recorded "a value stored on three paths is gcse PRE's pseudo" rule -- same
 * pass, opposite direction: there the multiple paths CREATE a pseudo, here they
 * PREVENT a fold.  Both are read off dead work in the disassembly.
 *
 * THE AGGREGATE MEASURE WENT DOWN WHEN THIS LANDED -- 293 -> 284 aligned-equal
 * -- AND IT IS STILL THE RIGHT CHANGE.  The 22 recovered instructions are
 * instructions the ROM has and we did not, so they move from the ins/del column
 * into the differing column, where the register rotation below makes every one
 * of them count.  Ranked the documented way (size-and-count exactness first),
 * eleven short beats thirty-seven short and the remaining gap is allocation
 * rather than missing program.  Do not "improve" the aligned figure by reverting
 * it.
 *
 * ================================================================
 * SECOND FINDING: `px` AND `c` ARE ONE SOURCE VARIABLE
 * ================================================================
 *
 * Worth 608 -> 612 instructions AND 284 -> 292 aligned-equal, i.e. the only
 * change measured here that improved both numbers at once.  The ROM's r7 carries
 * three roles in sequence -- the sin-derived x (`mov r7,r3` / `adds r7,#0x30`),
 * then the `(6 - px) << 8` row base fed to the 0xa0 loop, then the 0/1 selector
 * that indexes `Lee10c` -- and their live ranges are disjoint.  Declaring one
 * `int px` and assigning 0 or 1 into it in the f4 arms reproduces that; two
 * separate variables put the selector in a different register and cost 4
 * instructions and 8 encodings.  This is the same shape as the counter-reuse
 * rule (Anim_Whirlwind's r8/r9, Anim_Break's r8, Anim_Ice's sixfold `i`) but the
 * quantities here are NOT counters, so the rule is wider than the recorded
 * wording: A HARD REGISTER SERVING SEVERAL DISJOINT ROLES IS ONE SOURCE
 * VARIABLE, counter or not.
 *
 * ================================================================
 * THIRD FINDING: THE FRAME ARITHMETIC PROVES `pos` IS A TWO-ELEMENT ARRAY
 * ================================================================
 *
 * The ROM's frame is `sub sp, #0x4c` and the map reads
 *
 *     0x00-0x04  outgoing arguments (two stack words, six-argument DrawFn)
 *     0x08       amp, spilled across the inner sin call
 *     0x0c       buf, spilled across the same call
 *     0x10       fp           <-- READ AS `ldr r1,[sp,#0x10]` AT ALL EIGHT DRAW SITES
 *     0x14       base + 0x7828, a LICM invariant spilled to memory
 *     0x18       flag
 *     0x1c       dx
 *     0x20       dy
 *     0x24       ctx
 *     0x28       base
 *     0x2c-0x33  fns[2]
 *     0x34-0x4b  pos            <-- TWENTY-FOUR BYTES, not twelve
 *
 * With a single `vec3_t pos` the frame comes out `sub sp, #0x38`; the gap to the
 * ROM's 0x4c is exactly 20 bytes and it decomposes with nothing left over: 12
 * for pos's second element, 4 for fp's slot, 4 for amp's slot.  `vec3_t pos[2]`
 * with `&pos[0]` and `pos[0].x` takes the frame to 0x44, the predicted value.
 * Only pos[0] is ever read -- GetBattleActorPos2 is called once -- but the
 * declaration has to be there, and the sibling rom_dd2ac_c_c_b.c shows the
 * idiom this came from (two GetBattleActorPos2 calls into `a` and `b`).
 * THE CHANGE IS INERT ON THE RESIDUE (still 292 aligned-equal, identical
 * encodings): pos sits at the TOP of the frame, so adding 12 bytes above every
 * other slot moves no offset.  It is recorded because THE FRAME SIZE IS THE ONLY
 * EVIDENCE FOR IT and the next person will otherwise re-derive it from scratch.
 *
 * ALSO LOAD-BEARING, right on the first candidate:
 *   - `pp = g; base = *pp++; ctx = *pp;` with `g` kept for `g[7]` / `g[8]`.
 *   - ONE shared `int one = 1;` for BOTH BuildDraw2DFuncEx stack arguments AND
 *     `*(int *)(base + (0xef << 7))`, and ONE shared `int zero = 0;` for
 *     base+0x7784.  Anim_Whirlwind's ROM shares the BuildDraw2DFuncEx constant
 *     but NOT the base-relative store; this one shares both, and the register
 *     file is what says which way round (`str r5,[r3]` and `mov r1,sl`).
 *   - ONE shared `int arg = 0x90 << 3;` for both StartTask calls -- the OPPOSITE
 *     of Anim_Ice next door, which recomputes `mov r1,#0x90 / lsl r1,#3` at both
 *     sites.  The tell is whether the ROM recomputes it; here it does not.
 *   - `REG_BG2PA = 0x100;` THEN `REG_BLDALPHA = 0x1010;`, no int carrier: cse
 *     derives 0x4000052 from 0x4000020 as `add r2, #0x32`, exactly as the ROM
 *     has it.  Same device as Anim_Ice's BLDCNT -> WININ `sub r2, #8`.
 *   - The pooled `0x1f`, `0x3f` and `0x1000` are HImode constants, not mistakes:
 *     `(0x1f - i) | 0x1000` is stored with `strh`, and Thumb-1 has no
 *     PC-relative LDRH so gas assembles `ldrh` to `ldr`.  Do not replace them
 *     with `mov`, and do not add an int carrier -- that is Anim_Whirlwind's
 *     lever 5 and it cost 8 encodings there.
 *   - `amp = i * 2;` / `REG_BLDALPHA = ...;` / `amp -= 0x20;` as THREE
 *     statements: the ROM interleaves the register store between `mov r4,r3` and
 *     `sub r4,#0x20`, which a single `amp = i * 2 - 0x20` cannot produce.
 *   - `*buf++ = ...` for the `stmia r1!, {r3}`, and `sin(...) * amp` rather than
 *     `amp * sin(...)` -- *thumb_mulsi3 ties the destination to the first
 *     operand and the ROM's destination is the sin result's register (+1).
 *   - a do-while: `i = 0; do { ... } while (i != 0x84);`.  There is no guard
 *     block before the loop in the ROM, so duplicate_loop_exit_test never ran,
 *     which per jump.c:1137 is only possible for a do-while.
 *
 * MEASURED INERT: `amp` declared last, `amp` declared first (declaration order
 * is only allocno_compare's TIEBREAK and amp's n_refs/live_length dominate it);
 * `sin(i << 9)` / `cos(i << 9)` inline instead of a shared `int a`;
 * `sin((i + k) << 11)` for the inner angle.
 *
 * ================================================================
 * THE BLOCKER: A WHOLE-FUNCTION REGISTER ROTATION, AND IT PIVOTS ON ONE SPILL
 * ================================================================
 *
 * ATTRIBUTED TO local-alloc / global-alloc, not to any earlier pass.  The
 * register files line up as
 *
 *     ROM   r5 = angle    r6 = k   r7 = px/c   r8 = i   r9 = d   r11 = py
 *     ours  r5 = angle    r6 = k   r7 = amp    r8 = px  r9 = py  r10 = i
 *
 * -- ONE swap and everything downstream of it moves.  The ROM SPILLS `amp` to
 * sp+0x08 and reloads it inside the 0xa0 loop, which frees r7 for `px`; we give
 * r7 to `amp` and `px` is pushed into r8, `py` into r9 and `i` into r10.  The
 * ROM likewise spills `fp` to sp+0x10 and reads it at all eight draw sites,
 * where we keep it in a register.  Those two spills are the two missing frame
 * slots, and they are the whole remaining 8 bytes of frame.
 *
 * WHAT RULES OUT THE ALTERNATIVES.  allocno_compare's only three inputs are
 * n_refs, live_length and declaration order, and all three have been moved:
 *   - declaration order: amp first and amp last are BOTH inert (292 either way),
 *     so the tiebreak is not deciding this.
 *   - n_refs / live_length: `amp` has ~9 refs over a SHORT range and `px` ~5 over
 *     a LONG one, so priority (floor_log2(refs)*refs/live_length) FAVOURS amp --
 *     which is what we get and NOT what the ROM has.  For the ROM's assignment
 *     amp must lose, and no source spelling found here makes it lose.
 * So this is not an ordering problem and not a naming problem: it is the
 * allocator preferring the short-lived high-reference quantity, and it is the
 * SAME class as the 78 parks that already name REG_ALLOC_ORDER.  This function
 * is a good test case for that experiment because the swap is a single pair and
 * the rest of the program is now known to be right (eleven instructions, and the
 * relocation sequence was exact from the first candidate).
 *
 * NOT YET TRIED, in priority order: a `-da` dump of local-alloc's qty ordering
 * against the landed Anim_Break, which spills its context pointer the way this
 * one spills fp; and a candidate that raises inner-loop pressure deliberately to
 * push amp out, which is the only lever left that could move the allocator from
 * the C level.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef void (*CopyFn)(volatile u16 *dst, void *src, int len);
typedef void (*FillFn)(void *dst, int size, int value);
typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

extern void *iwram_3001eec[];
extern unsigned char *iwram_3001e74;
extern unsigned char gBuffer[];
extern unsigned char ewram_2012d80[], ewram_2014b00[], ewram_20158d2[];
extern unsigned char Lee10c[] __asm__(".Lee10c");
extern unsigned char Lee11a[] __asm__(".Lee11a");

extern void AnimEnd(void);
extern void Func_80cdb24(int a);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern void Func_80008d8(void *dst, int size, int value);
extern int  DecompressLZ(void *src, void *dst);
extern int  BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Func_80dbb9c(void);
extern int  sin(int a);
extern int  cos(int a);
extern void _PlaySound(int id);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern int *_GetBattleActor(int id);
extern void _Actor_TravelTo(void *actor, int x, int a2, int y);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _AnimTransitionIn(int a, int b, int c);
extern void WaitFrames(unsigned int n);
extern void gfree(int tag);

void Anim_Condemn(void *context)
{
    vec3_t pos[2];
    DrawFn fns[2];
    unsigned char *base;
    void *ctx;
    int dy;
    int dx;
    int flag;
    void **g;
    void **pp;
    unsigned char *data;
    CopyFn copy;
    FillFn fill;
    DrawFn *fp;
    DrawFn f1;
    int one;
    int zero;
    int arg;
    int i;
    int k;
    int d;
    int px;
    int py;
    int amp;
    int *buf;

    g = iwram_3001eec;
    pp = g;
    base = (unsigned char *)*pp++;
    ctx = *pp;
    *(State **)(base + 0x7828) = (State *)context;
    zero = 0;
    Func_80cdb24(0);
    REG_BG2PA = 0x100;
    REG_BLDALPHA = 0x1010;
    data = GetFile(FILE_ab);
    {
        int d0;
        d0 = 0xa0;
        copy = Func_8001af8;
        d0 <<= 19;
        copy((volatile u16 *)d0, data, 0x80);
    }
    data += 0x80;
    DecompressLZ(data, base);
    data = GetFile(FILE_ac);
    data += 0x80;
    DecompressLZ(data, gBuffer);
    one = 1;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, one);
    fns[0] = (DrawFn)g[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, one);
    f1 = (DrawFn)g[8];
    fp = fns;
    arg = 0x90 << 3;
    fp[1] = f1;
    StartTask(Func_80dbb9c, arg);
    *(int *)(base + (0xef << 7)) = one;
    *(int *)(base + 0x7784) = zero;
    StartTask(Task_BlitAnim, arg);
    flag = one;
    if ((*(State **)(base + 0x7828))->f4 == 1) {
        dx = 0xffb00000;
    } else {
        dx = 0xe0 << 15;
    }
    dy = 0xffe00000;
    i = 0;
    do {
        int a = i << 9;
        px = (dx >> 16) + ((sin(a) << 4) >> 16);
        py = (dy >> 16) + ((cos(a) << 2) >> 16) + 0x10;
        px += 0x30;
        if (i == 0x58) {
            _PlaySound(0x86);
        }
        if (i == 0x20) {
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                dx = 0xffe00000;
            } else {
                dx = 0x90 << 15;
            }
            dy = 0xc0 << 13;
            flag = 0;
        }
        if (i == 0x21) {
            REG_BLDALPHA = 0x1010;
            flag = 1;
        }
        if (i == 0x40) {
            GetBattleActorPos2((*(State **)(base + 0x7828))->ids[0], &pos[0]);
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                dx = (pos[0].x - 0x80) << 16;
            } else {
                dx = (pos[0].x - 0x40) << 16;
            }
            dy = 0;
            flag = 0;
        }
        if (i == 0x41) {
            REG_BLDALPHA = 0x1010;
            flag = 1;
        }
        buf = (int *)(base + (0xd3 << 7));
        amp = 0;
        if (i <= 0x1f) {
            if (i > 0xf) {
                amp = i * 2;
                REG_BLDALPHA = (0x1f - i) | 0x1000;
                amp -= 0x20;
            }
        } else if (i <= 0x3f) {
            if (i > 0x2f) {
                amp = i * 2;
                REG_BLDALPHA = (0x3f - i) | 0x1000;
                amp -= 0x60;
            }
        }
        if (amp < 0) {
            amp = 0;
        }
        px = (6 - px) << 8;
        k = 0;
        do {
            *buf++ = px - ((sin(i * 0x800 + k * 0x800) * amp) >> 10);
            k++;
        } while (k != 0xa0);
        if (flag != 0) {
            if ((*(State **)(base + 0x7828))->f4 == 0) {
                px = 0;
                d = 0;
            } else {
                px = 1;
                d = 0;
            }
            if (i <= 0x57) {
                fp[(*(State **)(base + 0x7828))->f4](ctx, base, Lee10c[px * 7],
                                                     py + Lee11a[d * 7], 0x39, 0x62);
            } else {
                if (i <= 0x5b) {
                    fp[(*(State **)(base + 0x7828))->f4](
                        ctx, base, Lee10c[px * 7], py + Lee11a[d * 7], 0x39, 0x62);
                }
                fp[(*(State **)(base + 0x7828))->f4](
                    ctx, base + 0x15d2, Lee10c[px * 7 + 1], py + Lee11a[d * 7 + 1],
                    0x63, 0x45);
                if ((unsigned)(i - 0x58) <= 1) {
                    fill = Func_80008d8;
                    fill(ctx, 0x80 << 7, 0x3f3f3f3f);
                }
                if ((unsigned)(i - 0x5a) <= 1) {
                    fp[(*(State **)(base + 0x7828))->f4](
                        ctx, base + 0x3081, Lee10c[px * 7 + 2],
                        py + Lee11a[d * 7 + 2], 0x80, 0x5b);
                }
                if ((unsigned)(i - 0x5c) <= 1) {
                    fp[(*(State **)(base + 0x7828))->f4](
                        ctx, gBuffer, Lee10c[px * 7 + 3], py + Lee11a[d * 7 + 3],
                        0x80, 0x5b);
                }
                if ((unsigned)(i - 0x5e) <= 1) {
                    fp[(*(State **)(base + 0x7828))->f4](
                        ctx, ewram_2012d80, Lee10c[px * 7 + 4],
                        py + Lee11a[d * 7 + 4], 0x80, 0x3b);
                }
                if ((unsigned)(i - 0x60) <= 1) {
                    fp[(*(State **)(base + 0x7828))->f4](
                        ctx, ewram_2014b00, Lee10c[px * 7 + 5],
                        py + Lee11a[d * 7 + 5], 0x7a, 0x1d);
                }
                if ((unsigned)(i - 0x62) <= 1) {
                    fp[(*(State **)(base + 0x7828))->f4](
                        ctx, ewram_20158d2, Lee10c[px * 7 + 6],
                        py + Lee11a[d * 7 + 6], 0x4c, 0x19);
                }
            }
        }
        if (i == 0x58) {
            int *actor = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
            actor[0xa] = 0x80 << 9;
            actor[0xd] = 0x80 << 10;
            actor[0xc] = 0x80 << 10;
            actor[0x12] = 0;
            *((unsigned char *)actor + 0x5a) = 0;
            *((unsigned char *)actor + 0x58) = 0;
            _Actor_TravelTo(actor, actor[2] * 2, 0, actor[4]);
            Func_80d6888((*(State **)(base + 0x7828))->ids[0], -1, 5, -1, 0);
        }
        if (i == 0x78) {
            int *actor = (int *)*_GetBattleActor((*(State **)(base + 0x7828))->ids[0]);
            actor[0x12] = 0xab85;
        }
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        i++;
    } while (i != 0x84);
    StopTask(Task_BlitAnim);
    StopTask(Func_80dbb9c);
    _AnimTransitionIn(1, *(unsigned short *)(iwram_3001e74 + (0xc9 << 3)), 0x18);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
