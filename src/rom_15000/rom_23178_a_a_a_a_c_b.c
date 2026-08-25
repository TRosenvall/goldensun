/* Cluster Func_80284dc..Func_80284dc extracted from goldensun/asm/rom_15000/rom_23178_a_a_a_a_c.s.
 *
 * Slotted between rom_23178_a_a_a_a_c_a.o and the rest of stage1.ld.
 *
 * Allocates a 0x98-byte EWRAM block, zeroes it, starts a task on it and returns
 * it. `pop {r1}` is the return-value tell.
 *
 * StartTask IS DELIBERATELY LEFT UNDECLARED, exactly as in
 * src/rom_8a000/rom_944ec_a_c_a_a_a_b.c. Declared, gcc emits its
 * `ldr r0, =Func_8028194` one instruction early, ahead of the `ldr r1, =0xc76`
 * rather than after it. 2 of 22 with the prototype, exact without it.
 *
 * That is now twice in one batch for the same callee, which makes it a property
 * of StartTask's call shape rather than a coincidence: its two arguments are
 * both pooled, and declaring it moves r0 ahead of r1.
 */
#include "dma.h"
extern void *galloc_ewram(int tag, int size);
extern void Func_8028194(void);

void *Func_80284dc(void)
{
    void *p;

    p = galloc_ewram(0x3a, 0x98);
    DMA3_CLEAR(p, 0x98);
    StartTask(Func_8028194, 0xc76);
    return p;
}
