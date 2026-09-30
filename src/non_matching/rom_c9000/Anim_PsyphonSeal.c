/* Anim_PsyphonSeal (asm/rom_c9000/rom_cfef4.s, 436 instructions) --
 * NON-MATCHING, 383 of 459 encodings differ.  SIZE IS EXACT (1036 bytes both)
 * AND INSTRUCTION COUNT IS EXACT (459 = 459), so THIS FIGURE IS A TRUE DISTANCE
 * and objcmp's count ranks candidates here.  tools/aligncmp.py reads 239 of 459
 * aligned-equal (52.1%, 66 hunks); that is NOT an objcmp number and must not be
 * quoted as one -- it is recorded only because it says the residue is a
 * permutation, not wrong code.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/cfef4_PsyphonSeal.c \
 *     asm/rom_c9000/rom_cfef4.s --func Anim_PsyphonSeal
 *
 * THE RELOCATION SEQUENCE IS ALREADY THE ROM'S -- 46 relocations, the same
 * symbols in the same order including both `_call_via_r5` sites and the
 * `_call_via_r4`/`_call_via_r5` pair in the tail.  objcmp still prints
 * RELOCATIONS differ because every OFFSET is shifted; read the sequence.
 *
 * Objcmp progression, for the record: 437 (candidate 1) -> 351 -> 383-with-size-
 * and-count-exact.  THE 351 IS THE LOWER NUMBER AND THE WORSE CANDIDATE: it was
 * 461 instructions against 459 and 1040 bytes against 1036, so its count was
 * saturated.  This file is the one to build on.
 *
 * SPLIT SHAPE: TEXT/DATA SPLIT, AND IT NEEDS ONE NEW EXPORT.
 * `python3 tools/datacheck.py asm/rom_c9000/rom_cfef4.s` says
 * `Anim_PsyphonSeal reads .Lee134  *** SPLIT MUST EXPORT: .global .Lee134`.
 * The file holds SIX functions -- Anim_Condemn, Anim_Unused_ScreenMelt,
 * Anim_Bind, Anim_PsyphonSeal, Anim_AstralBlast, Anim_ShiningStar -- and
 * PsyphonSeal is the FOURTH, so landing it alone is a three-way split:
 *
 *   asm/rom_c9000/rom_cfef4_a.s   Condemn + ScreenMelt + Bind, KEEPS the whole
 *                                 `.section .rodata` block (.Lee10c .. .Lee158)
 *                                 and gains `.global .Lee134`
 *   src/rom_c9000/rom_cfef4_b.c   THIS FILE
 *   asm/rom_c9000/rom_cfef4_c.s   AstralBlast + ShiningStar
 *
 * `.Lee134` is `.incrom 0xee134, 0xee140` -- 12 bytes, a vec3_t -- and it is read
 * ONLY by this function, so it could also be emitted from C with
 * `__asm__(".Lee134")` the way src/rom_c9000/rom_d82b0_b.c does for `.Lee9f8`.
 * DO NOT: the five other labels in that block are read by the five other
 * functions and the addresses are adjacent, so the blob must stay in the asm
 * object while any of them is still asm.  Declare it `extern` here and add the
 * one `.global`.  stage1.ld's `.text` row (line 1803) becomes the three above in
 * order; its `.rodata` row (line 1954) follows rom_cfef4_a.o.
 *
 * SHIMS: TWO REGISTER PINS, both in the Func_8001af8 DMA block, lifted verbatim
 * from src/rom_c9000/rom_cc5d8_a_a_b.c (Anim_UnleashIntro) exactly as
 * src/non_matching/rom_c9000/cfef4_ShiningStar.c records for this file's idiom.
 * `python3 tools/shimcount.py` reports `register pins : 2` and warns there is no
 * fakematch.txt row -- A LANDING MUST BOOK ONE; a park does not.  No per-file
 * Makefile flag override applies to this stem.
 *
 * ================================================================
 * THE ONE THAT MATTERED: THE PROLOGUE IS ARRAY SUBSCRIPTS, NOT A WALKING POINTER
 * ================================================================
 *
 * Worth 437 -> 351 of 459 in one edit, and it CONTRADICTS WHAT THE DISASSEMBLY
 * LOOKS LIKE.  The ROM opens
 *
 *     ldr r1, =iwram_3001eec / mov r8, r1 / mov r3, r8 / ldmia r3!, {r2}
 *
 * and an `ldmia rX!, {..}` is the recorded tell for `pp = tbl; base = *pp++;`.
 * Here that spelling is WRONG.  The plain subscript form
 *
 *     base = (unsigned char *)iwram_3001eec[0];
 *     ctx  = iwram_3001eec[1];
 *     gfx  = (unsigned char *)iwram_3001eec[2];
 *     view = *(void **)((char *)iwram_3001eec - 0x6c);
 *
 * scores 86 encodings better even though it emits `ldr r1,[r3,#0] / ldr r2,[r3,#4]`
 * where the ROM has the `ldmia`, because it stops `tbl` and `pp` from becoming two
 * long-lived pseudos and the WHOLE SPILL MAP falls into place behind it.  The
 * walking-pointer form was re-measured on top of every later fix and is flat at
 * 463/1044/437.  cfef4_ShiningStar.c's advice for this very file -- "THE
 * ARRAY-SUBSCRIPT SPELLING IS THE LOAD-BEARING PART" -- is confirmed, and its
 * `ldmia` claim is the part to distrust.
 *
 * > AN `ldmia rX!, {rY}` IN THE REFERENCE DOES NOT PROVE A WALKING POINTER IN THE
 * > SOURCE.  Measure the subscript form too: two fewer live pseudos can be worth
 * > more than the two-instruction prologue shape they cost.
 *
 * SECOND, AND IT IS WHAT MADE SIZE AND COUNT EXACT: TWO GetFile RESULTS ARE TWO
 * LOCALS, worth 461/1040 -> 459/1036.  `data = GetFile(FILE_79); ... data += 0x80;
 * DecompressLZ(data, base);` and then `data2 = GetFile(FILE_76);` -- reusing ONE
 * `data` across both keeps a single pseudo live through five calls, which costs a
 * spill word and two instructions.  The ROM puts both in r7 in turn.  This is the
 * one-variable-per-region rule on a VALUE (not a counter), and it is the converse
 * of what the same session had to do for Anim_Unsummon's counter.
 *
 * ================================================================
 * REMAINING BLOCKER: TWO EXTRA SPILL WORDS, FRAME 0x80 AGAINST 0x78
 * ================================================================
 *
 * Every sp offset in the body is 8 too high, and that alone is most of the 383.
 * The ROM's sixteen spill words are
 *
 *   0x44 base  0x40 ctx  0x3c frame  0x38 fnB  0x34 fnA  0x30 k  0x2c view
 *   0x28 dx  ... then EIGHT gcc-created ones: 0x24 &pos, 0x20 the A pointer,
 *   0x1c the copy of `off`, 0x18 the middle counter, 0x14 the 0x5555 angle,
 *   0x10 `off`, 0x0c the frame angle, 0x08 `nn`
 *
 * above four expand-time vec3_t (0x6c pos, 0x60 vin, 0x54 out, 0x48 scalev) and
 * eight bytes of outgoing args: 8 + 64 + 48 = 0x78.  This candidate has
 * SEVENTEEN source-side spills plus nine compiler ones.  The declaration order
 * base, ctx, frame, fnB, fnA, k, view, dx already reproduces the ROM's top eight
 * EXACTLY, just shifted -- so the declaration list is right and the two extras
 * are what to hunt.
 *
 * ONE OF THEM IS `slot`.  The ROM does NOT hold a `State **`: it derives
 * `base + 0x7828` in r5 for the prologue and re-derives it inside the frame loop.
 * Ours spills it at 0x2c.  But deleting it is WORSE, measured:
 *   `slot` local for the prologue, inline in the frame loop (this file)
 *                                                       1036 / 459, 383 diff
 *   `*(State **)(base + 0x7828)` inline everywhere       1040 / 461, 431 diff
 * so the fix is to shorten its live range or win it a register, not to remove it.
 *
 * MEASURED INERT, so do not re-try: dropping the `two` local for two bare `2`s
 * (CSE merges them into the same pseudo anyway), and moving `copy` from a
 * function-scope local into the DMA block.  Both byte-identical to this file.
 *
 * ================================================================
 * THE SHAPE, READ OFF THE REFERENCE AND CONFIRMED BY THE EXACT COUNT
 * ================================================================
 *
 *   - LOOP BOUND `(*slot)->f14 * 20 + 0x48` as a plain `while`.  The ROM's entry
 *     test is `cmp r3, r5` against `-0x48` -- that is `0 != f14*20 + 0x48` with
 *     frame folded to 0, i.e. duplicate_loop_exit_test's peel of a `while`, NOT
 *     the `if (c) { do ... while (c); }` form.  `(f14 * 4 + f14) << 2` is what
 *     `* 20` compiles to; do not spell the shifts by hand.
 *   - THE k LOOP RUNS ONCE.  `mov r5,#0` ... `add r5,#1 / cmp r5,#1 / beq`.  It
 *     is a real do-while with bound 1 and it must stay one, because `off`,
 *     `frame2` and the frame angle are all givs OF THAT LOOP: `off = k * 0x20`,
 *     `frame2 = frame - k * 8` (the ROM's descending r11, `add r11, -8`), and
 *     `ang - k * 0x3000`.  Writing them as accumulators instead puts their
 *     initialisers in the wrong half of the preheader.
 *   - THREE NESTED GUARDS, and they are three separate tests, not a chain:
 *     `(unsigned)frame2 <= 0x5f` around the matrix work, `frame2 <= 0x43` around
 *     the 3-particle loop and the interpolation loop, `frame2 > 0x3f` around the
 *     two tail blits.  The middle one is an unsigned/signed pair on purpose --
 *     the outer is `bls`, the inner `ble`.
 *   - `vin` AT sp+0x60 IS ONE VARIABLE USED TWICE OVER: first as the SOURCE of
 *     `Func_80e3944(&vin, &out)` (loaded from the actor at ab[2..4]), then as the
 *     DESTINATION of `Func_80e3944(&Lee134, &vin)` inside the 3-particle loop.
 *     Two separate vec3_t would add 12 bytes of frame.
 *   - `m++` BEFORE the `% 3`: the ROM computes the A pointer from the
 *     pre-increment value, increments, and calls `__modsi3` on the NEW m, so
 *     `A = pbase + (m + off); m++; B = pbase + (m % 3 + off);` is the order.
 *   - The interpolation is two `__divsi3` calls, not a shift:
 *     `A->vx + t * (B->vx - A->vx) / 0x18`, with `nn = 5 - frame2 / 16` and
 *     `w = nn * 2` computed INSIDE the m loop even though they are invariant in it.
 *   - `Data_ede5c[nn - 1]` beside `w = nn * 2` so gcc reuses `w - 2` as the byte
 *     index (`sub r3, r7, #2 / ldrh r1, [r2, r3]`), the Anim_Break idiom.
 *   - `REG_BG2PA = 0x100` pools as `ldrh r3, .L..` -- correct, a CONST_INT stored
 *     in HImode has no Thumb immediate form; and `REG_BG2X = dx << 8` where
 *     `dx = 0x40 - pos.x` is a named local because it is read again next frame.
 *   - `fnA = (DrawFn)gPtrs[0xb8 / 4]` and `fnB = iwram_3001f0c` are TWO function
 *     pointers reloaded EVERY frame, which is why both sit in spill slots
 *     (0x34, 0x38) rather than registers.
 *
 * NEXT STEP: the frame.  Find the two extra spilled pseudos -- `slot` is one and
 * the ninth compiler pseudo is the other -- and every offset in the body aligns
 * at once.  Because size and count are already exact, objcmp's count is a real
 * ranking here, so a spelling sweep is cheap and honest on this function.  Of the
 * six in this file, cfef4_ShiningStar.c recommends Anim_Unused_ScreenMelt (198
 * instructions, a different idiom) as the easiest first.
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
extern vec3_t Lee134 __asm__(".Lee134");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void *GetFile(int id);
extern void Func_8001af8(volatile u16 *dst, void *src, int len);
extern int DecompressLZ(void *src, void *dst);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void GetBattleActorPos2(int id, vec3_t *out);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void _Func_80b82c4(int a, int b, int c, int d);
extern void ColorCycleVFXPalette(int a, int b, int c, int d);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void *_GetBattleActor(int id);
extern void InitMatrixStack(void);
extern void MatrixSetLook(void *a, void *b);
extern void MatrixScalev(vec3_t *v);
extern void MatrixRoll(int a);
extern void MatrixYaw(int a);
extern int Func_80e3944(vec3_t *in, vec3_t *out);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_PsyphonSeal(void *context)
{
    vec3_t pos;
    vec3_t vin;
    vec3_t out;
    vec3_t scalev;
    unsigned char *base;
    void *ctx;
    int frame;
    DrawFn fnB;
    DrawFn fnA;
    int k;
    void *view;
    int dx;
    unsigned char *gfx;
    unsigned char *data;
    unsigned char *data2;
    State **slot;
    CopyFn copy;
    int two;
    int j;
    int m;
    int t;

    base = (unsigned char *)iwram_3001eec[0];
    ctx = iwram_3001eec[1];
    gfx = (unsigned char *)iwram_3001eec[2];
    view = *(void **)((char *)iwram_3001eec - 0x6c);
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(0);
    REG_BG2PA = 0x100;
    data = GetFile(FILE_79);
    {
        register unsigned q0 __asm__("r0");
        register int q2 __asm__("r2");
        q0 = 0xa0;
        copy = Func_8001af8;
        q2 = 0x80;
        q0 <<= 19;
        copy((volatile u16 *)q0, data, q2);
    }
    data += 0x80;
    DecompressLZ(data, base);
    DecompressLZ(GetFile(FILE_73), gfx);
    data2 = GetFile(FILE_76);
    DecompressLZ(data2, base + (0x80 << 5));
    *(int *)(base + (0xef << 7)) = 3;
    *(int *)(base + 0x7784) = 0x4040404;
    StartTask(Task_BlitAnim, 0x90 << 3);
    GetBattleActorPos2((*slot)->ids[0], &pos);
    dx = 0x40 - pos.x;
    REG_BG2X = dx << 8;
    _PlaySound(0x8e);
    frame = 0;
    while (frame != (*slot)->f14 * 20 + 0x48) {
        int ang;
        if (frame == 0x40) {
            _Func_80bd7dc(0);
        }
        if (frame == 0x2e) {
            _Func_80b82c4((*(State **)(base + 0x7828))->f8,
                          (*(State **)(base + 0x7828))->ids[0], 0x10, 0);
        }
        ColorCycleVFXPalette(frame, 0xaaab, 0x5555, 0);
        two = 2;
        BuildDraw2DFuncEx(0x2e, 7, 7, 3, two);
        fnA = (DrawFn)gPtrs[0xb8 / 4];
        BuildDraw2DFuncEx(0x2f, 7, 7, 7, two);
        fnB = iwram_3001f0c;
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
                    int scale = (0xa8 << 10) - (ang - k * 0x3000);
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
                        Func_80e3944(&Lee134, &vin);
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
                        int w;
                        m++;
                        B = pbase + (m % 3 + off);
                        nn = 5 - frame2 / 16;
                        w = nn * 2;
                        t = 0;
                        do {
                            int x = A->vx + t * (B->vx - A->vx) / 0x18;
                            int y = A->vy + t * (B->vy - A->vy) / 0x18;
                            fnA(ctx, base + Data_ede5c[nn - 1] + (0x80 << 5),
                                x - nn, y - nn, w, w);
                            t++;
                        } while (t != 0x18);
                    } while (m != 3);
                }
                if (frame2 > 0x3f) {
                    fnA(ctx, base, out.x - 0x18, out.y - 0x18, 0x18, 0x30);
                    fnB(ctx, base, out.x, out.y - 0x18, 0x18, 0x30);
                }
            }
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
