/* Func_8011a84 -- 0x08011a84  (asm/rom_9000/rom_11568_c_c_a_c_c.s)
 *
 * BLOCKER: prologue instruction scheduling. 7 of 36, exact length, and all
 * seven are the SAME instructions in a different order:
 *
 *   rom   sub sp,#4 / mov r4,r3 / mov r5,r0 / mov r6,#0 / mov r0,sp
 *         / add r4,#0xd8 / str r6,[r0,#0]
 *   ours  mov r4,r3 / sub sp,#4 / add r4,#0xd8 / mov r6,#0 / mov r5,r0
 *         / str r6,[sp,#0] / ... / mov r0,sp
 *
 * The ROM materialises the stack address into r0 once and both stores through
 * it and passes it to the DMA; we store through sp and materialise r0
 * separately. The body -- the DMA3_SET, the 0xffff guard, the four field
 * stores and the StartTask -- is exact.
 *
 * MEASURED, and both of the obvious edits fail:
 *   a named `int *q = &buf;` used for the store and the DMA source   7 (inert)
 *   splitting `t` so `t += 0xd8` comes after the zero, matching the
 *     ROM's `mov r4,r3 ... add r4,#0xd8` interleave                 33, one
 *     line short -- gcc folds the offset back into the base load
 *   both together                                                   33
 *
 * The second result is the useful one: the split-statement interleave needs the
 * value to survive as a LOCAL, and a pointer built from a global address is
 * folded even when it is mutated in two steps. That is the same foldability
 * boundary recorded for the naming lever -- a link-time address is foldable, and
 * splitting the arithmetic does not make it less so.
 *
 * DMA3_SET from include/dma.h reproduces its block exactly, which is the second
 * function in the corpus to use that header successfully.
 */
#include "dma.h"

extern int iwram_3001e70;
extern void Func_80119cc(void);
extern void StartTask(void *f, int pri);

void Func_8011a84(unsigned short *p)
{
    char *t;
    int buf;
    int go;

    t = (char *)iwram_3001e70 + 0xd8;
    go = 0;
    buf = go;
    DMA3_SET(&buf, t, 0x85000003);
    if (*p != 0xffff) {
        *(unsigned short **)t = p;
        *(unsigned short **)(t + 4) = p;
        *(unsigned short *)(t + 8) = go;
        *(unsigned short *)(t + 0xa) = go;
        go = 1;
    }
    if (go != 0)
        StartTask(Func_80119cc, 0xc8 << 4);
}
