/* Func_808fe38 -- 0x0808fe38 -- asm/rom_8a000/rom_8d9a4_c_c_a_a.s
 *
 * BLOCKER: ARGUMENT FILL ORDER at two calls. 9 of 43.
 *
 * Allocates a 0x540-byte block, DMA-clears it, writes four fields, and starts
 * two tasks. Found via tools/shapesib.py, which scored Func_8090824
 * (src/rom_8a000/rom_8d9a4_c_c_c_a_a_a_b.c, already matching, same directory)
 * at 0.771. That sibling supplied the allocation, the DMA3_CLEAR and the
 * `(0xa5 << 3)` field write unchanged, and the first 27 instructions were
 * exact on the first screen.
 *
 * ONE LEVER LANDED, and its PLACEMENT is the finding:
 *
 *   The stored `1` at +0x536 is a bare literal into a halfword store, so it
 *   pools -- `ldr r3, =0x1` against the ROM's `mov r3, #0x1` -- and the pool
 *   word makes the function two lines long. Routing it through an `int` local
 *   fixes it, but only when the assignment sits IMMEDIATELY BEFORE THE STORE:
 *
 *     `one = 1;` immediately before the store   45 lines,  9 differ  <- kept
 *     `one = 1;` as the first statement         51 lines, 49 differ
 *
 * That is a sharper version of the precondition recorded on
 * OvlFunc_common1_148 and Func_808f498. It is not only that the fix needs a
 * spare register -- it needs the constant's live range to stay SHORT. Hoisting
 * the assignment to the top of the function keeps the value live across the
 * allocation, the DMA and three other stores, and costs six lines.
 *
 * Note the contrast with OvlFunc_939_2009240, elevated in the same round,
 * where the sibling's `v = 0x5b;` had to be the FIRST statement. Both are the
 * same rule -- put the assignment where the ROM materialises the constant --
 * and the ROM shows which: there the `mov` precedes the whole body, here it
 * sits between the address load and the store.
 *
 * WHAT REMAINS is the order of two independent argument setups at both
 * StartTask calls:
 *
 *     rom    lsl r1, #0x4 / ldr r0, =Task_ScreenWindowTransition
 *     ours   ldr r0, =Task_ScreenWindowTransition / lsl r1, #0x4
 *
 * Both values die at the call, so this is the argument-temporary boundary
 * recorded in docs/elevation.md: gcc rematerialises argument temporaries
 * during fill and discards any statement structure the source imposes. Four
 * spellings were measured against that boundary on ovl_780898/2008fec and came
 * back byte-identical; it is not re-tested here.
 *
 * The remaining tail lines are the double-label artifact -- this reference
 * keeps its pool inside the function.
 */
#include "dma.h"

extern void *galloc_ewram(int index, unsigned int size);
extern void StartTask(void (*task)(void), unsigned int priority);
extern void Task_ScreenWindowTransition(void);
extern void Func_808f498(void);

void Func_808fe38(unsigned int arg0)
{
    void *p;
    int one;

    p = galloc_ewram(0x1f, 0xa8 << 3);
    DMA3_CLEAR(p, 0xa8 << 3);
    *(unsigned short *)((unsigned int)p + (0xa5 << 3)) = arg0;
    *(unsigned short *)((unsigned int)p + 0x52a) = 0;
    *(unsigned short *)((unsigned int)p + 0x534) = 0x3f3f;
    one = 1;
    *(unsigned short *)((unsigned int)p + 0x536) = one;
    StartTask(Task_ScreenWindowTransition, 0xc8 << 4);
    StartTask(Func_808f498, 0x90 << 3);
}
