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
 *     asm/rom_15000/rom_15e8c_c_a_a_a_a.s --func Func_80167e0
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
