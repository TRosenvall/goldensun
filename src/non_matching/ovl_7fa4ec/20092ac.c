/* OvlFunc_970_20092ac (0x020092ac) -- NON-MATCHING.
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
 * NEXT: nothing. The class has no known source-level lever.
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
