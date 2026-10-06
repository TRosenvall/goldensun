/* OvlFunc_970_20092ac (0x020092ac) -- NON-MATCHING.
 *
 * NON-MATCHING, 46 of 41 encodings  (MEASURED, batch 319 recipe backfill).
 *   THE LENGTH ARITHMETIC, DONE EXACTLY (batch 331 brief D).  The pair in the
 *   claim line above is a STREAM LENGTH pair, not an instruction-count pair,
 *   which is the eighteen-park labelling class batch 330 brief C diagnosed.
 *   objcmp reports it like this now:
 *     INSTRUCTION COUNT   ref 30, ours 35   (16-bit encodings only)
 *     32-bit stream entries  ref 11, ours 11
 *     alignment pads         ref 0,  ours 1
 *     SIZE                ref 104 bytes, ours 116
 *   and the arithmetic closes on both sides (30*2 + 11*4 = 104;
 *   35*2 + 11*4 + 2 = 116).  BEWARE objcmp's LABEL on that middle number: it
 *   calls those entries "pool words" and a Thumb `bl` is a 32-bit encoding, so
 *   the four calls are counted there too (objcmp.py:213-260, and the assumption
 *   is stated at objcmp.py:539).  Of the eleven, FOUR are the `bl`s and SEVEN
 *   are real literal pool words.  Adding the calls back, the reference is 34
 *   instructions and this body is 39 -- which is EXACTLY what the prose further
 *   down already said, so that prose is sound and only the claim line was ever
 *   a stream reading.
 *
 *   SO THE GAP IS FIVE INSTRUCTIONS OF LENGTH AND NOTHING ELSE.  Literal pool
 *   words are identical on both sides (seven and seven) and the one extra pad is
 *   FORCED by our odd instruction count, not an independent difference: this is
 *   NOT the padding trap and NOT a pool-order problem.  The positional figure in
 *   the claim line therefore measures MISALIGNMENT and is not a distance; the
 *   distance has no meaning until the lengths are equal.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- a
 *   CONSEQUENCE of the length difference, not a separate blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7fa4ec/20092ac.c \
 *     asm/overlays/rom_7fa4ec/ovl_30_c_c_c_c.s --func OvlFunc_970_20092ac
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: duplicate-constant CSE, the class pickable.py rejects on.
 *
 * 39 lines against the ROM's 34, and all five extra lines are one decision.
 * The value 0x100 is passed to __Func_8004970 and then, three calls later, to
 * __UploadSpriteGFX. The ROM builds it TWICE:
 *
 *     mov r0, #0x80 / lsl r0, #0x1      ... bl __Func_8004970
 *     mov r1, #0x80 / lsl r1, #0x1      ... bl __UploadSpriteGFX
 *
 * gcc CSEs the two into one value and, having to keep it live across three
 * calls with r4-r7 already committed, parks it in r8 -- which costs a
 * `mov r6, r8 / push {r6}` prologue pair and a `pop {r3} / mov r8, r3` epilogue
 * pair on top of the two `mov rN, r8` uses. Five lines, exactly.
 *
 * MEASURED (rom 34 lines):
 *   baseline                                              39, 38
 *   -fno-gcse                                             39, 38
 *   -fno-rerun-cse-after-loop                             39, 38
 *   -fno-strength-reduce                                  39, 38
 *   -fno-strict-aliasing                                  39, 38
 *   __UploadSpriteGFX's size parameter as `unsigned short`
 *     so the two uses have different modes                39, 38
 *
 * -fno-gcse being inert is the informative one: this is LOCAL cse, not the
 * global pass, so there is no flag for it. And the mode-difference probe was
 * the one idea with a mechanism behind it -- a u16 and an int use of the same
 * literal are different RTL -- and gcc folds the narrowing before CSE runs, so
 * even that does not separate them.
 *
 * This is the wall pickable.py was built to predict, and it is here in its
 * purest form: ONE repeated constant, nothing else wrong with the function.
 * Everything else is byte-exact, including the pooled `ldr r3, =0x30` for a
 * halfword store -- which is worth noting, because 0x30 IS an eight-bit
 * immediate and gcc pools it anyway. That is the second confirmation (after
 * batch 174's `ldr r2, =0x21`) that a pooled small constant stored through a
 * halfword pointer is NORMAL gcc output and not a `_CONST_*` symbol tell.
 *
 * WHAT IS RIGHT: DMA3_FILL(p, 0x11111111, 0x100) for the
 * `str` + `stmia r3!, {r0, r1, r2}` + `sub r3, #0xc` sequence with
 * cnt = 0x85000040; the two `.lcomm` halfwords reached as
 * `extern short L1c1a __asm__(".L1c1a");` (the `ldrsh r0, [r5, r3]` with r3
 * zeroed is a plain `short` read, not a cast); and `0xc8 << 4` for the task
 * priority.
 *
 * NEXT: see the batch-331 section below; the class HAS a source-level lever
 * for its sibling functions, and it does not transfer to this one.
 *
 * ============================ BATCH 331 BRIEF D =============================
 *
 * THE DIAGNOSIS ABOVE SURVIVES RE-MEASUREMENT.  The five extra instructions are
 * exactly the one decision this park names, and they decompose as:
 *   `mov r6, r8` + `push {r6}` prologue pair, `pop {r3}` + `mov r8, r3`
 *   epilogue pair, and the constant's own build costing four instructions
 *   (`mov r3,#0x80 / lsl r3,r3,#1 / mov r8,r3 / mov r0,r8`) against the
 *   reference's two, less one where the reference rebuilds it for the second
 *   call and this body only copies it down.
 *
 * READ OUT OF THE DUMPS, not inferred.  `.00.rtl` holds THREE
 * `(set (reg) (const_int 256))` insns -- the first call's argument, the inlined
 * DMA3_FILL `size` parameter (a `reg/v`, i.e. a user variable), and the
 * __UploadSpriteGFX argument.  `.03.cse` holds ONE, with the third site's use
 * rewritten to the survivor.  So cse1 is the pass that does it, and the pseudo
 * then has to cross two calls.  `.18.greg` says ";; 0 regs to allocate:" and the
 * dispositions line reads "33 in 8", so GLOBAL allocation never runs here at
 * all: local-alloc put it in r8.  `.18.greg` also carries a REG_EQUIV for the
 * constant on that pseudo, so reload KNEW the value and still did not
 * rematerialise it -- gcc-2.96 has no rematerialisation pass.
 *
 * THE INFERENCE THAT LOOKED RIGHT AND IS REFUTED, recorded because it is the
 * obvious next idea.  gcse's constant propagation is the only pass that could
 * push a cse1-unified constant back out to each use, and `gcse.c:675` returns
 * immediately when `n_basic_blocks <= 1`, which is true of this branch-free
 * function.  That predicts "add a branch and the constant splits in two".
 * MEASURED, with a branch added purely as an instrument: it does NOT split.
 * What changes instead is the register -- the shared pseudo moves from r8 to r7
 * and the high-register prologue and epilogue pairs vanish.  So the cost here is
 * REGISTER PRESSURE, not the unification itself, and the lever to look for is
 * one that frees a low call-saved register or keeps the pseudo out of a
 * call-crossing live range.
 *
 * THE CORPUS NUMBER, which is suggestive and is NOT a proof of unreachability.
 * Over the 4,483 gcc-generated `.s` files in asm/, a repeated same-value
 * `mov rX,#imm` + `lsl` synthesis inside one straight-line region (no label and
 * no branch between; a `bl` does not end a cse extended basic block) occurs in
 * 61 of 4,026 BRANCHY generated functions and in ZERO of 880 BRANCH-FREE ones.
 * This reference is branch-free and does it twice.  Of 96 such hits across the
 * corpus, 89 are in files carrying register pins or PIN macros and only seven
 * are pin-free, so the pin-free precedent base is thin -- and in all seven the
 * repeated constant feeds a MEMORY STORE, never a call's argument register,
 * which is this function's shape.  Treat this as a bound WITH ITS EVIDENCE
 * ATTACHED, not as a closed door: the mechanism that would explain it (gcse) is
 * refuted above, so the correlation is real and unexplained.
 *
 * MEASURED INERT, batch 331 -- four more spellings of the size, all identical to
 * the body below:
 *   1. a single `int size` local used at all three sites
 *   2. the second site written as a different shift of the same value
 *   3. the first site written as the plain literal instead of a shift
 *   4. a separate `size` local assigned AFTER the first call
 * With the park's own six, the SPELLING of this constant is a closed dimension.
 *
 * THE DIMENSION NOT YET VARIED.  The sibling class that landed in this batch
 * (OvlFunc_888_2008848 and OvlFunc_882_200c5b8, both parked on a unified
 * `const_int 12`) moved only when the CONSTRUCT that generates the constant
 * changed -- a bitfield assignment, so store_bit_field builds the mask and cse1
 * never sees two equal constant loads.  There is no analogous construct for a
 * CALL ARGUMENT, which is why that lever does not transfer here.  What has never
 * been varied is the LIVE SET: whether the `.L1c1a` address or `p` can be made
 * to not cross a call, which would leave a low call-saved register for the
 * constant and -- per the instrumented branch probe above -- cost nothing.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern short L1c18 __asm__(".L1c18");
extern short L1c1a __asm__(".L1c1a");
extern void *__Func_8004970(int size);
extern int __AllocSpriteSlot(void);
extern void __UploadSpriteGFX(int slot, int size, void *src);
extern int __StartTask(void *fn, int pri);
extern void OvlFunc_970_20091c4(void);

void OvlFunc_970_20092ac(void)
{
    void *p;

    p = __Func_8004970(0x80 << 1);
    L1c1a = __AllocSpriteSlot();
    DMA3_FILL(p, 0x11111111, 0x100);
    __UploadSpriteGFX(L1c1a, 0x80 << 1, p);
    L1c18 = 0x30;
    __StartTask(OvlFunc_970_20091c4, 0xc8 << 4);
}
