/* Cluster Func_801d980..Func_801d980 extracted from goldensun/asm/rom_15000/rom_1ca1c_c_c_a.s.
 *
 * Slotted between rom_1ca1c_c_c_a_a.o and the rest of stage1.ld.
 *
 * Allocates a 0x628-byte EWRAM block, zeroes it with DMA3_CLEAR, and starts a
 * task on it. Third instance of this exact shape in two batches.
 *
 * StartTask IS DELIBERATELY LEFT UNDECLARED -- see
 * src/rom_8a000/rom_944ec_a_c_a_a_a_b.c. That is now three functions in two
 * batches, so it is a property of StartTask's call shape rather than a
 * coincidence: both its arguments are pooled or shifted, and declaring it moves
 * r0 ahead of r1.
 *
 * The task it starts is Func_801d94c, which is itself parked at 5 of 17 --
 * see src/non_matching/rom_15000/rom_1d94c.c.
 */
#include "dma.h"
extern void *galloc_ewram(int tag, int size);
extern void Func_801d94c(void);

void Func_801d980(void)
{
    void *p;

    p = galloc_ewram(0x14, 0xc5 << 3);
    DMA3_CLEAR(p, 0x628);
    StartTask(Func_801d94c, 0xc8 << 4);
}
