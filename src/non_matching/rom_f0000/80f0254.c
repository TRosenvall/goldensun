/* Func_80f0254 (ClearBackgroundPage) -- NON-MATCHING.
 * Blocker class: PLACEMENT OF A POOL LOAD among cheap register setup.
 * 33 lines against the ROM's 34, 13 differing.
 *
 * Two DMA3 fixed-source fills, the fill word supplied from a single stack
 * slot. Both `stmia` blocks, both control words and the whole second transfer
 * are exact.
 *
 *     rom    mov r1, #0xc0 / mov r5, #0xa0 / ldr r3, =0x1010101
 *            / lsl r1, #19 / lsl r5, #19
 *     ours   mov r1, #0xc0 / mov r5, #0xa0 / lsl r1, #19 / lsl r5, #19
 *            / ldr r3, =0x1010101
 *
 * The ROM puts the pool load between the two `mov`s and the two `lsl`s; gcc
 * emits it after both shifts. This is the same shape as the argument-precompute
 * class HANDOFF.md diagnoses, except these are not call arguments, so that
 * section's mechanism does not obviously apply and no claim is made here that
 * it does.
 *
 * SOLVED, and worth reusing: SEPARATE THE REGISTER TEMP FROM THE STACK SLOT.
 * Written with one `int v` whose address is taken, gcc stores into the stack
 * slot INSIDE each switch arm -- 20 differing. The ROM computes the fill word
 * into a register across the branch and stores ONCE after the join. Two
 * variables, `value` for the register and `slot` for the address-taken word,
 * reproduces that and is worth 20 -> 14.
 *
 * Tried after that:
 *   - assigning `value` between the two base assignments in arm 0, which is
 *     where the ROM's pool load sits: 13, the best seen
 *   - a named `int *sp = &slot` so the store goes through a pointer the way
 *     the ROM's `mov r4, sp / str r3, [r4]` suggests: 14, no change. gcc
 *     addresses the slot off sp regardless.
 *   - zeroing through the register temp before the second transfer: 14
 *
 * The stream stays one line short in every form, which by the length rule
 * means something the ROM does is still missing rather than merely reordered.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

void Func_80f0254(int page)
{
    int slot;
    int value;
    void *dst;
    void *pal;

    if (page == 0) {
        dst = (void *)(0xc0 << 19);
        value = 0x1010101;
        pal = (void *)(0xa0 << 19);
    } else {
        value = 0x81818181;
        dst = (void *)0x6008000;
        pal = (void *)0x5000100;
    }
    slot = value;
    DMA3_SET(&slot, dst, 0x85001e00);
    slot = 0;
    DMA3_SET(&slot, pal, 0x85000040);
}
