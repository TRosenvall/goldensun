/* Anim_Bind -- 0x080d05fc, 524 instructions.  PARKED, FOUR INSTRUCTIONS LONG.
 * The shallowest of batch 302's four; the other three carry more analysis.
 *
 * NON-MATCHING, 484 of 550 encodings differ.
 *
 * MEASUREMENT -- THE OBJCMP FIGURE IS SATURATED.  SIZE does not match (ref 1240
 * bytes, ours 1248) and COUNT does not match (ref 550, ours 554), so 484 cannot
 * rank anything.  The ranking view is aligncmp (alignment-tolerant, masks
 * NOTHING):
 *
 *     aligned-equal 306 of 550 = 55.6%,  300 differing/ins/del in 91 hunks
 *
 * RELOCATIONS: the line prints, and the SYMBOL SEQUENCE IS IDENTICAL -- all 58
 * rows in the ROM's order, `_call_via_r4` sites included.  Only offsets move
 * (42 of 58), which is the expected consequence of being four instructions
 * long.  So every callee and every pooled symbol is already in the right place
 * in the right order, and the residue is placement and allocation only.
 *
 * WHICH CANDIDATE IS PARKED, AND WHY -- READ THIS BEFORE RE-MEASURING.  Two
 * candidates are within noise of each other and the two metrics disagree:
 *
 *     candidate   objcmp   instructions   aligned-equal      ins/del hunks
 *     n3          482      558 (+8)       305 / 550  55.5%   ~
 *     n4 (THIS)   484      554 (+4)       306 / 550  55.6%   6
 *     n5          489      559 (+9)       311 / 550  56.5%   6
 *
 * Applied literally, the recorded rule ("rank by size-and-count exactness
 * first, then the aligned figure, never the raw count") picks n5: nothing is
 * exact, so the aligned figure decides and n5 leads by 5 encodings.  THIS PARK
 * DELIBERATELY TAKES n4 INSTEAD, and the reason is stated so the owner can
 * overrule it: 5 encodings is 0.9% of the reference and is what one scheduling
 * difference costs, whereas the LENGTH gap is 4 against 9.  Length exactness is
 * the gate that turns objcmp's count back into a true distance, and without it
 * the next attempt has no ranking tool at all.  n5 is kept beside this file in
 * the batch's scratch; its one difference from n4 is written up under lever (3).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Bind.c \
 *     asm/rom_c9000/rom_cfef4.s --func Anim_Bind
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT, THREE WAYS, ONE NEW EXPORT.
 * `python3 tools/datacheck.py asm/rom_c9000/rom_cfef4.s` says
 * `Anim_Bind reads .Lee128  *** SPLIT MUST EXPORT: .global .Lee128`.
 * The file holds SIX functions -- Anim_Condemn, Anim_Unused_ScreenMelt,
 * Anim_Bind, Anim_PsyphonSeal, Anim_AstralBlast, Anim_ShiningStar -- and Bind
 * is the THIRD, so landing it alone is a three-way split:
 *
 *   asm/rom_c9000/rom_cfef4_a.s   Condemn + ScreenMelt
 *   src/rom_c9000/rom_cfef4_b.c   THIS FILE
 *   asm/rom_c9000/rom_cfef4_c.s   PsyphonSeal + AstralBlast + ShiningStar,
 *                                 KEEPING the whole `.section .rodata` block
 *                                 (.Lee10c .. .Lee158), and gaining
 *                                 `.global .Lee128`
 *
 * `.Lee128` is `.incrom 0xee128, 0xee134` -- 12 bytes, a vec3_t.  DO NOT emit it
 * from C: the five neighbouring labels in that block are read by the five other
 * functions and their addresses are adjacent, so the blob must stay in the asm
 * object while any of them is still asm.  Declare it `extern vec3_t Lee128
 * __asm__(".Lee128")` and add the one `.global`.  This is the same call
 * src/non_matching/rom_c9000/Anim_PsyphonSeal.c records for `.Lee134` in this
 * very file, and the two must be decided the same way.
 *
 * NOTE THE ASYMMETRY WITH THAT PARK: PsyphonSeal is the FOURTH function, so its
 * split leaves the .rodata with the _a piece; Bind is the THIRD, so Bind's
 * split leaves it with the _c piece.  stage1.ld names this object TWICE -- line
 * 1803 `(.text)` and line 1956 `(.rodata)` -- and for Bind the `.rodata` row
 * must follow rom_cfef4_c.o.  The batch-300 hazard (a DIFFERENT function in the
 * file owning the .rodata row) is live here and was checked by grepping the
 * script for the stem rather than trusting datacheck, which reads only the .s.
 * WHICHEVER OF THE SIX LANDS FIRST DECIDES THE OTHERS' SPLIT SHAPE, so these two
 * parks should be landed in one planned pass, not independently.
 *
 * `tools/split_s.py` was deliberately NOT run: it rewrites the .s files and the
 * linker script in place, and this was measured from a read-only workspace.
 *
 * SHIMS: NONE.  `python3 tools/shimcount.py` reports zero rows -- PIN-FREE.
 * THAT IS A FINDING, because Anim_PsyphonSeal next door carries TWO REGISTER
 * PINS for the shared `Func_8001af8` palette-DMA block and this function needs
 * neither: see lever (1).  No fakematch.txt row, no per-file flag override.
 *
 * ================================================================
 * LEVERS THAT PAID, IN THE ORDER THEY PAID
 * ================================================================
 *
 * (1) THE ORACLE IS NEXT DOOR, AND IT IS ALMOST A LINE-FOR-LINE TEMPLATE.
 *     src/non_matching/rom_c9000/Anim_PsyphonSeal.c is in the SAME .s, and
 *     Anim_Bind and Anim_PsyphonSeal are the same animation with different
 *     constants: the same REG_BG2PA/REG_BG2X pair, the same
 *     ColorCycleVFXPalette(frame, 0xaaab, 0x5555, 0), the same
 *     `while (frame != f14 * 20 + 0x48)` bound, the same nested
 *     k / j / m / t loops, the same `MatrixScalev`+`MatrixRoll`+`MatrixYaw`
 *     triple, the same `Data_ede5c[nn - 1]` / `w = nn * 2` interpolation, and
 *     the same `*(base + 0x7824) = k` tail.  Transcribing it with Bind's own
 *     constants and its four extra `GetFile`/`DecompressLZ` pairs put the FIRST
 *     candidate at 562 instructions against 550 -- twelve long -- which for a
 *     524-instruction function is one afternoon's reading, not a
 *     reconstruction.  This is the "LOOK UP A SHAPE IN THE SOLVED CORPUS BEFORE
 *     INVENTING A CONSTRUCT" rule at its strongest, and the corpus row worth
 *     adding is:
 *
 *     > TWO ANIMATIONS IN ONE .s WITH THE SAME REG_BG2PA / REG_BG2X / 0xaaab
 *     > COLOUR-CYCLE OPENING ARE THE SAME FUNCTION WITH DIFFERENT CONSTANTS.
 *
 *     AND THE PINS DO NOT TRANSFER.  PsyphonSeal's two `register ... __asm__`
 *     pins on the Func_8001af8 block were tried here verbatim and are EXACTLY
 *     INERT -- 507 of 550 before and after, aligncmp identical at 49.1% and the
 *     same 105 hunks.  Bind reaches the ROM's `mov r0,#0xa0 / ldr r3,
 *     =Func_8001af8 / mov r2,#0x80 / mov r1,r5 / lsl r0,#19 / bl _call_via_r3`
 *     from a plain `copy = Func_8001af8; copy((volatile u16 *)(0xa0 << 19),
 *     data, 0x80);`.  So the two pins next door are load-bearing for
 *     PsyphonSeal's surrounding allocation and NOT for the DMA idiom itself --
 *     which means they are a candidate for removal when PsyphonSeal is next
 *     touched, and they must NOT be copied into any further member of this file
 *     on the assumption that the idiom needs them.
 *
 * (2) NOT NAMING THE FILE POINTERS THE ROM DOES NOT KEEP: 562 -> 558
 *     instructions, aligncmp 49.1% -> 55.5%, the largest single step here.  Four
 *     GetFile/DecompressLZ pairs open this function and they are NOT written
 *     alike.  Read which register the result lands in:
 *       - FILE_73 and FILE_76: the GetFile result flows straight into
 *         DecompressLZ's r0 with no copy, so NO local --
 *         `DecompressLZ(GetFile(FILE_76), base + (0x80 << 5));`
 *       - FILE_79 and FILE_8f: the result goes through r5 and is advanced by
 *         `adds r5, #128` first, so a local IS named -- and it is THE SAME
 *         LOCAL BOTH TIMES (r5 at both sites), reused rather than a second
 *         variable.
 *     Declaring `data2` and `data3` for the latter two spilled three extra
 *     pseudos.  The rule this confirms is the recorded "A local that only holds
 *     an ADDRESS can cost the ordering -- delete it", applied per call site:
 *
 *     > A GetFile RESULT THAT REACHES DecompressLZ IN r0 WITH NO COPY WAS NEVER
 *     > NAMED; ONE THAT IS ADVANCED BY A CONSTANT FIRST WAS.  And when two such
 *     > sites use the SAME register, they share ONE source local.
 *
 * (3) `ang` IS A DECREMENTED LOOP VARIABLE, NOT `ang - k * 0x3000`:
 *     558 -> 554 instructions, and it is where this file DEPARTS from the
 *     PsyphonSeal template.  The ROM keeps the angle at sp+0x08 and subtracts at
 *     the k-loop tail (`ldr r3,[sp,#8] / ldr r1,=0xffffd000 / add r3,r1 /
 *     str r3,[sp,#8]`), then reads it directly for the scale
 *     (`ldr r2,[sp,#8] / subs r2,r3,r2`).  PsyphonSeal's spelling
 *     `scale = (0xa8 << 10) - (ang - k * 0x3000)` does NOT strength-reduce to
 *     that here -- it expands to thirteen instructions where the ROM has two --
 *     because the expression sits inside a conditional the loop optimiser will
 *     not hoist out of.  Writing
 *
 *         scale = (0xa8 << 10) - ang;   ...   ang -= 0x3000;   (at the k tail)
 *
 *     reproduces the accumulator.  Worth recording because PsyphonSeal next door
 *     still carries the expression form and is still 383 of 459 -- THIS IS A
 *     LEVER TO TRY ON THAT PARK.
 *
 *     MEASURED, AND THE REASON n5 EXISTS: writing the m-loop's A and B pointers
 *     as `&((Part *)(base + (0xe1 << 7)))[m + off]` instead of through a named
 *     `pbase` local -- the same "do not name the base + K pointer" lever that
 *     paid on Anim_PlanetDiver in this batch -- moves aligncmp 55.6% -> 56.5%
 *     (300 -> 287 diffs) but costs FIVE instructions (554 -> 559).  The ROM does
 *     rebuild `base + (0xe1 << 7)` at both sites with the 0xe1<<7 rematerialised
 *     rather than holding a hoisted pbase, so the SPELLING is probably right and
 *     something else is absorbing the five.  n5 is the file to start from if the
 *     aligned figure is trusted over the length gate.
 *
 * OTHER FACTS READ OFF THE REFERENCE, all confirmed by the measurement:
 *   - `AnimStart(1)`, not 0 as in PsyphonSeal.
 *   - the Anim_Djinni prologue block is Bind's own and guarded:
 *     `if ((*slot)->f1c == 1) Anim_Djinni(context, 3, (*slot)->f4, 0, &cx, &cy);`
 *     with cx at sp+0x50 and cy at sp+0x4c.
 *   - `DrawFn fns[2]` with `fp = fns` assigned BEFORE the frame loop (the ROM
 *     sets sp+0x14 in the loop preheader at .Ld072c), then `fns[0]` written
 *     directly and `fp[1]` through the pointer -- the d9ab8_StatDown idiom, and
 *     `BuildDraw2DFuncs((*slot)->f4, fp)` takes the POINTER, which is what gives
 *     the ROM's `ldr r1,[sp,#0x14]`.
 *   - the f1c block's geometry: `X = ((sin(frame << 11) * 20) >> 16) + cx + dx
 *     - 0x14` (the ROM builds *20 as `lsl #2 / add / lsl #2`) and
 *     `Y = ((cos(frame << 11) * 4) >> 16) + cy - 0x18`, then
 *     `if (frame > 0x20) Y = Y - frame * 2 + 0x40`.
 *   - the sprite sheet for that block is `base + (0x80 << 6)`, and the three
 *     DecompressLZ destinations are `base`, `gfx` (iwram_3001eec[2]),
 *     `base + (0x80 << 5)` and `base + (0x80 << 6)`.
 *   - `*(base + 0x7784) = 0x4040404` and `+= 0x1010101` when
 *     `frame > 0x10 && (frame & 0xf) == 0`.
 *   - `(unsigned)frame2 <= 0x5f` with `frame2 = frame - k * 8` (the ROM's
 *     `bls` is UNSIGNED and frame2 is a giv in r11 decremented by 8 per k).
 *   - `ids[k]` is reached with the register-offset `ldrsh r0,[r2,r3]` off
 *     `0x24 + k * 2`, not a folded offset.
 *   - the k loop runs ONCE (`while (k != 1)`) and `*(base + 0x7824) = k` uses
 *     the counter, not a literal 1 -- PsyphonSeal's reading, confirmed here.
 *
 * ================================================================
 * THE BLOCKER, BY PASS: reload -- THREE SPILL SLOTS TOO MANY, ON THE WRONG
 * PSEUDOS
 * ================================================================
 *
 * The frame is the inversion of Anim_Ground's blocker in this same batch and is
 * worth the pair: ours is LARGER than the ROM's (0x90 against 0x84, three words)
 * and yet we spill FEWER of the variables the ROM spills.  The ROM's 33-word
 * frame is fully accounted for --
 *
 *   0x08 ang  0x0c off  0x10 ang2  0x14 fp  0x18 m  0x1c B  0x20 base_i  0x24 A
 *   0x28 &pos 0x2c dx   0x30 view  0x34 k   0x38 frame  0x3c ctx  0x40 base
 *   0x44 fns[2]  0x4c cy  0x50 cx  0x54 scalev  0x60 out  0x6c vin  0x78 pos
 *   + 0x00/0x04 outgoing  = 0x84
 *
 * -- and the two surviving insert/delete regions are both reload declining to
 * spill what the ROM spills:
 *
 * (A) `ang`, `off` and `k` stay in registers for us.  The ROM writes all three
 *     back to memory at the k-loop tail (aligncmp deletes six ROM instructions
 *     there: `str r3,[sp,#8] / ldr r3,[sp,#0x34] / str r2,[sp,#0xc] /
 *     movs r2,#8 / ...`) and re-reads `ang` from sp+0x08 for the scale.  Ours
 *     holds `ang` in fp and never stores it, which is cheaper and wrong.
 * (B) the m-loop's A/B pointers: the ROM rebuilds `base + idx * 0x1c +
 *     (0xe1 << 7)` from sp+0x40 and a rematerialised 0xe1<<7 at both sites,
 *     where we load an extra `base_i` copy first.
 *
 * Three pseudos we cannot name took the three extra slots, so the whole 0x44
 * and up region of the frame is shifted and every `fns` / `cx` / `cy` reference
 * carries a wrong offset -- visible directly as `ldr r4,[sp,#0x44]` against our
 * `ldr r4,[sp,#0x4c]`.  This is the recorded class: "gcc assigns spill slots in
 * ascending pseudo number to descending sp offset ... the slot order is a
 * CONSEQUENCE of the allocation, not a handle on it", and the general blocker
 * "REGISTER ALLOCATION: it is systematic, and it is not reachable from C."
 *
 * RULED OUT, with what was measured:
 *   - NOT a mis-read program: the relocation SYMBOL SEQUENCE is identical, all
 *     58 rows in order, so every callee, both matrix helpers, `__modsi3`,
 *     `__divsi3`, `.Lee128` and `Data_ede5c` are present exactly once where the
 *     ROM has them.
 *   - NOT a missing statement: the four extra instructions are all accounted for
 *     by (A) and (B), both of which are stores the ROM makes and we do not, plus
 *     one load we make and it does not.
 *   - NOT a FILE-STRUCTURE refusal: the split is understood, one export, and the
 *     stage1.ld `.rodata` row placement is worked out above.
 *   - NOT the shared-pin idiom: the two pins PsyphonSeal carries are inert here
 *     to the encoding, so the residue cannot be blamed on missing shims.
 *
 * NEXT MOVE: this is the shallowest of the four and the one to come back to
 * LAST.  When it is re-attempted, do it TOGETHER WITH Anim_PsyphonSeal -- they
 * share a .s, their splits constrain each other, lever (3) is untried on
 * PsyphonSeal, and PsyphonSeal's two pins now look removable.
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

extern void *iwram_3001eec[];
extern void *gPtrs[];
extern DrawFn iwram_3001f0c;
extern unsigned short Data_ede5c[];
extern vec3_t Lee128 __asm__(".Lee128");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int DecompressLZ(void *src, void *dst);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void Anim_Djinni(void *context, int a, int b, int c, int *p1, int *p2);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void ColorCycleVFXPalette(int a, int b, int c, int d);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void BuildDraw2DFuncs(int a, DrawFn *fns);
extern void *_GetBattleActor(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixScalev(vec3_t *v);
extern void MatrixRoll(int a);
extern void MatrixYaw(int a);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern int sin(int a);
extern int cos(int a);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Bind(void *context)
{
    vec3_t pos;
    vec3_t vin;
    vec3_t out;
    vec3_t scalev;
    int cx;
    int cy;
    DrawFn fns[2];
    unsigned char *base;
    void *ctx;
    int frame;
    int k;
    void *view;
    int dx;
    DrawFn *fp;
    unsigned char *gfx;
    unsigned char *data;
    State **slot;
    CopyFn copy;
    int two;
    int w;
    int j;
    int m;
    int t;

    base = (unsigned char *)iwram_3001eec[0];
    ctx = iwram_3001eec[1];
    view = *(void **)((char *)iwram_3001eec - 0x6c);
    gfx = (unsigned char *)iwram_3001eec[2];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(1);
    if ((*slot)->f1c == 1) {
        Anim_Djinni(context, 3, (*slot)->f4, 0, &cx, &cy);
    }
    REG_BG2PA = 0x100;
    data = GetFile(FILE_79);
    copy = Func_8001af8;
    copy((volatile u16 *)(0xa0 << 19), data, 0x80);
    data += 0x80;
    DecompressLZ(data, base);
    DecompressLZ(GetFile(FILE_73), gfx);
    DecompressLZ(GetFile(FILE_76), base + (0x80 << 5));
    data = GetFile(FILE_8f);
    data += 0x80;
    DecompressLZ(data, base + (0x80 << 6));
    *(int *)(base + (0xef << 7)) = 3;
    *(int *)(base + 0x7784) = 0x4040404;
    StartTask(Task_BlitAnim, 0x90 << 3);
    GetBattleActorPos2((*slot)->ids[0], &pos);
    dx = 0x40 - pos.x;
    REG_BG2X = dx << 8;
    _PlaySound(0x8e);
    frame = 0;
    fp = fns;
    while (frame != (*slot)->f14 * 20 + 0x48) {
        int ang;
        if (frame == 0x40) {
            _Func_80bd7dc(0);
        }
        ColorCycleVFXPalette(frame, 0xaaab, 0x5555, 0);
        if ((*(State **)(base + 0x7828))->f1c == 1) {
            int a1 = frame << 11;
            int X = ((sin(a1) * 20) >> 16) + cx + dx - 0x14;
            int Y = ((cos(a1) * 4) >> 16) + cy;
            unsigned char *q;
            BuildDraw2DFuncs((*(State **)(base + 0x7828))->f4, fp);
            Y = Y - 0x18;
            if (frame > 0x20) {
                Y = Y - frame * 2 + 0x40;
            }
            q = base + (0x80 << 6);
            fns[0](ctx, q, X, Y, 0x28, 0x28);
            if (frame <= 3) {
                fp[1](ctx, q, X, Y, 0x28, 0x28);
            }
            gfree(0x2f);
            gfree(0x2e);
        }
        two = 2;
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
        fns[0] = (DrawFn)gPtrs[0xb8 / 4];
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, two);
        fp[1] = iwram_3001f0c;
        if (frame > 0x10 && (frame & 0xf) == 0) {
            *(int *)(base + 0x7784) += 0x1010101;
        }
        ang = frame * 3 << 9;
        k = 0;
        do {
            int off = k * 0x20;
            int frame2 = frame - k * 8;
            int *ab = (int *)*(int **)_GetBattleActor((*(State **)(base + 0x7828))->ids[k]);
            if ((unsigned)frame2 <= 0x5f) {
                InitMatrixStack();
                MatrixSetLook(view, (char *)view + 0xc);
                vin.x = ab[2];
                vin.y = ab[3];
                vin.z = ab[4];
                Func_80e3944(&vin, &out);
                out.x = pos.x + dx;
                out.y = out.y - 0x18;
                if (frame2 <= 0x43) {
                    Part *pbase = (Part *)(base + (0xe1 << 7));
                    Part *q = pbase + off;
                    int scale = (0xa8 << 10) - ang;
                    int rot = (0x40 - frame2) << 9;
                    int ang2 = 0;
                    j = 0;
                    do {
                        InitMatrixStack();
                        if (frame2 <= 0x3f) {
                            scalev.x = scale;
                            scalev.y = scale;
                            scalev.z = scale;
                            MatrixScalev(&scalev);
                            MatrixRoll(rot);
                            MatrixYaw(rot);
                        }
                        MatrixRoll(ang2);
                        Func_80e3944(&Lee128, &vin);
                        q->vx = out.x + vin.x;
                        q->vy = out.y + vin.y + 0x10;
                        ang2 += 0x5555;
                        j++;
                        q++;
                    } while (j != 3);
                    m = 0;
                    do {
                        Part *A = pbase + (m + off);
                        Part *B;
                        int nn;
                        m++;
                        B = pbase + (m % 3 + off);
                        nn = 5 - frame2 / 16;
                        w = nn * 2;
                        t = 0;
                        do {
                            int x = A->vx + t * (B->vx - A->vx) / 0x18;
                            int y = A->vy + t * (B->vy - A->vy) / 0x18;
                            fns[0](ctx, base + Data_ede5c[nn - 1] + (0x80 << 5),
                                   x - nn, y - nn, w, w);
                            t++;
                        } while (t != 0x18);
                    } while (m != 3);
                }
                if (frame2 > 0x3f) {
                    fns[0](ctx, base, out.x - 0x18, out.y - 0x18, 0x18, 0x30);
                    fp[1](ctx, base, out.x, out.y - 0x18, 0x18, 0x30);
                }
            }
            ang -= 0x3000;
            k++;
        } while (k != 1);
        gfree(0x2f);
        gfree(0x2e);
        *(int *)(base + 0x7824) = k;
        WaitFrames(1);
        frame++;
    }
    StopTask(Task_BlitAnim);
    AnimEnd();
}
