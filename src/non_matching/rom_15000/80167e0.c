/* Func_80167e0 (ScrollTextBuffer) -- NON-MATCHING.
 *
 * NON-MATCHING, 47 of 63 encodings  (RE-DERIVED batch 324 brief E; unchanged).
 *   SIZE EXACT (136 bytes both).  ENCODING COUNT EXACT (63 = 63).
 *   *** RELOCATIONS DIFFER IN THEIR SYMBOLS, NOT ONLY THEIR OFFSETS:
 *       ref  [0x52 R_ARM_THM_CALL _call_via_r3] [0x84 R_ARM_ABS32 Func_80008d8]
 *       ours [0x50 R_ARM_THM_CALL _call_via_fp] [0x80 R_ARM_ABS32 Func_80008d8]
 *       SO THIS FIGURE IS NOT A DISTANCE.  `make compare` cannot pass a
 *       relocation difference.  Fix this before trusting the encoding count. ***
 *   No edit tried in batch 324 moves the symbol.  The body below is UNCHANGED;
 *   what changed is the diagnosis.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_15000/80167e0.c \
 *     asm/rom_15000/rom_15e8c_c_a_a_a_a.s --whole
 *
 * ========== CORRECTION 1: THE "SOLVED" INDIRECT CALL IS NOT SOLVED ==========
 *
 * The previous header said, under SOLVED and worth keeping: "the indirect call.
 * `fp = Func_80008d8; fp(...)` reproduces the ROM's
 * `ldr r3, =Func_80008d8 / bl _call_via_r3`".  IT DOES NOT.  This body emits
 * `bl _call_via_fp`.  Per docs/elevation.md, "The `_call_via_` veneer names
 * follow `reg_names`" -- the veneer name IS the register, so `_call_via_fp` says
 * the pointer landed in r11.  The old header's own relocation warning already
 * recorded `_call_via_fp`; the two claims contradicted each other and the
 * relocation line is the true one.  The function-pointer LOCAL is still right --
 * it is what produces an indirect call at all -- but the REGISTER is wrong.
 *
 * ========== CORRECTION 2: THE 47 AND THE RELOCATION ARE ONE DEFECT ==========
 *
 * The previous header's register-pressure diagnosis is CORRECT and is the only
 * part of it that survived re-measurement.  What it lacks is the consequence.
 *
 *   ROM   r5=src r6=dst r7=i r8=stride r9=cnt sl=off fp=ctl, `cur` ON THE STACK
 *         (`sub sp,#8`, reloaded and restored every iteration), and the control
 *         word rebuilt per iteration: `mov r2,r9 / mov r4,fp / orrs r2,r4`.
 *   ours  r5=src r7=dst r6=`ctl|cnt` ALREADY ORED r8=stride sl=cur
 *         fp=THE FUNCTION POINTER, and `i` spilled (`sub sp,#4`).
 *
 * gcc hoists the loop-invariant `ctl | cnt` into ONE register where the ROM
 * keeps `ctl` and `cnt` in TWO.  That frees exactly one callee-saved register;
 * r11 is then available, the function pointer takes it, and the veneer becomes
 * `_call_via_fp`.  With a register spare gcc also keeps `cur` in sl and spills
 * `i` instead of the other way round.  SO THERE IS ONE ROOT CAUSE, NOT TWO:
 * defeat the hoist and the relocation, the frame size and the spill choice all
 * follow.
 *
 * ========== MEASURED AND BIT-IDENTICALLY INERT -- DO NOT REPEAT =============
 *
 * All of these give 47, the SAME five runs and the SAME two relocations:
 *   DMA3_SET_RW for DMA3_SET                                             0
 *     dma.h's own doc says `"+r" (_cnt)` withdraws the promise that the count
 *     survives and brings the per-transfer re-issue back.  It cannot help here:
 *     the OR is hoisted into a PSEUDO at the call site, and the clobber only
 *     concerns hard register r2, which gcc re-copies from that pseudo anyway.
 *   an INSTRUMENT (labelled; the device-free body is what ships) -- a file-local
 *     DMA3_SET_OR(src, dst, ctl, cnt) doing the OR inside the pinned
 *     `register u32 _cnt __asm__("r2") = ctl | cnt;`                     0
 *     So making the OR's DESTINATION a hard register does not defeat the hoist
 *     either: integrate.c's parameter copy puts a hoistable pseudo in front.
 *   `int`-returning function-pointer type                                0
 *   `cur + off` as a named local before the call                         0
 *   `off + cur` operand order                                            0
 * MEASURED AND WORSE:
 *   `cur += 0x80` moved last                                            +1
 *   `off` initialised before `ctl`                                      +1
 *   the pointer assigned ONCE BEFORE the loop                           +1
 *   `ctl` dropped and the literal written at the call site              +5, and
 *                                      61 instructions against 63
 *   the control word and count as separate locals ORed at the call site  0
 *                                      (previous header; reproduced)
 *
 * ========== THE SECOND PARK NAMED IN BATCH 324's BRIEF IS A PHANTOM ========
 *
 * `src/non_matching/rom_15000/80168f4.c` does NOT define a body for this
 * function.  It is the park for AdvanceMsgText (0x080168f4), a 628-instruction
 * function; it declares `extern void Func_80167e0(int a);` at line 257 and calls
 * it at line 305.  There is exactly ONE body for Func_80167e0, so there was
 * nothing for tools/crossfire.py to cross.
 *
 * SOLVED and worth keeping: the shift chain (lines*3, then *2 and *8), the DMA
 * per row, the four pointers advancing by 0x80, the countdown from 0x1d, and the
 * function-pointer local that makes the call indirect at all.
 *
 * NEXT: ONE question, and it is the whole function -- stop loop.c hoisting
 * `ctl | cnt`.  Both operands are loop-invariant pseudos, so `scan_loop` makes
 * the `(set pseudo (ior A B))` a movable unconditionally.  Two ways of putting
 * the OR behind a hard register are measured inert above, so the next thing to
 * try is NOT another spelling of the OR: it is whatever makes one of the two
 * operands non-invariant in the ROM's source.
 *
 * ========== BATCH 335 BRIEF D: THE HOIST IS THE WHOLE GAP, AND THE SIZE
 * ========== ROUTE TO DEFEATING IT IS CLOSED BY ARITHMETIC ==================
 *
 * Re-measured whole: 47 of 63 encodings, ours 63, SIZE 136 bytes both, and the
 * relocation symbols still differ, so the figure above stands and is still not
 * a distance.  Pin-free confirmed by tools/shimcount.py.  Its piece holds
 * exactly ONE function.
 *
 * WHAT IS NEW IS THE DIRECTION IN REAL INSTRUCTIONS: fifty-nine against our
 * fifty-eight.  The equal encoding counts are an ALIGNMENT PAD -- the
 * reference's odd instruction count leaves a halfword that the assembler pads
 * before the pool, and `objdump -dz` counts that pad as an encoding.  So we are
 * ONE REAL INSTRUCTION SHORT, not level, and the correction-2 analysis above is
 * confirmed quantitatively:
 *
 *   the or inside the loop     costs two moves plus the or against our one move
 *   the pointer in the loop     costs a pool load, and saves the setup move
 *   the shifted base on the     costs a load, an add and a store against our
 *     stack                     move and add, and saves building the constant
 *   our spilled counter         costs a store and a load the reference lacks
 *
 * Net one, and every term follows from the single hoist named above.
 *
 * ========== THE SIZE ROUTE IS CLOSED, WITH THE ARITHMETIC =================
 *
 * `move_movables` hoists when `threshold * savings * lifetime >= insn_count`
 * (loop.c:1803), where `threshold = (has_call ? 1 : 2) * (1 + n_non_fixed_regs)`
 * (loop.c:651).  This loop HAS a call, so the multiplier is one -- but
 * n_non_fixed_regs is of order fourteen on this target, so the product clears
 * the loop's twenty-odd insns with a lifetime of two.  Defeating the hoist by
 * making the loop bigger or the value shorter-lived would need a lifetime of
 * one, i.e. the or's result consumed by the very next insn, and it already
 * nearly is.  DO NOT SPEND ANOTHER ROUND ON THE THRESHOLD.
 *
 * ========== AND THE HARD-REGISTER ROUTE REPRODUCES AS INERT ===============
 *
 * `scan_loop` refuses a movable whose destination is in `may_not_optimize`
 * (loop.c:766), and loop.c:713-717 sets that flag for EVERY hard register
 * below FIRST_PSEUDO_REGISTER.  So a set whose destination is a hard register
 * is STRUCTURALLY un-hoistable, which looked like the answer.  Measured as an
 * instrument -- a file-local helper identical to this file's DMA3 helper except
 * that the or happens inside its own pinned r2 initialiser -- the or is STILL
 * gone from the loop and the figure is unchanged, in both operand orders.  So
 * the hoisted object is NOT that set: it is a pseudo that precedes it, exactly
 * as the correction above says, and the header's own warning that this is not a
 * spelling problem is now backed by the compiler source.
 *
 * Also measured: routing the control word through this file's existing
 * size-taking DMA3 helper, so the or is built from a byte count inside the
 * helper rather than at the call site.  THREE instructions LONG and the same
 * forty-seven differing -- the unsigned divide by four does not fold against
 * the multiply by four that feeds it.
 *
 * NEXT, narrowed: the question is no longer how to stop loop.c, because the
 * hoist is of a pseudo that integrate.c's parameter copy creates and the
 * threshold cannot be reached.  It is how the ROM's source avoids HAVING a
 * loop-invariant or at all -- which means one of its two operands is not
 * invariant there.  The count is a constant difference and the control word is
 * a literal, so the candidate worth testing next is that the ROM's loop does
 * not carry a count at all: that the transferred length is derived from a value
 * the loop already changes, and the apparent invariant is an artifact of our
 * reconstruction.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

extern void Func_80008d8(void *p, int n, int v);

void Func_80167e0(int lines)
{
    char *src;
    char *dst;
    char *cur;
    int n3, n6;
    int stride, cnt, ctl, off;
    int i;
    void (*fp)(void *, int, int);

    n3 = lines * 3;
    dst = (char *)0x6002520;
    n6 = n3 * 2;
    stride = n3 * 8;
    src = (char *)0x6002520 + stride;
    cur = (char *)0x6002500;
    cnt = 0x18 - n6;
    ctl = 0x84000000;
    off = (0x20 - n6) * 4;
    i = 0x1d;
    do {
        DMA3_SET(src, dst, ctl | cnt);
        fp = Func_80008d8;
        fp(cur + off, stride, 0);
        i--;
        cur += 0x80;
        dst += 0x80;
        src += 0x80;
    } while (i >= 0);
}
