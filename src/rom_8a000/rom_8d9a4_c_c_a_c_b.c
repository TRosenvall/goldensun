/* Cluster AllocGlobal1F..AllocGlobal1F extracted from goldensun/asm/rom_8a000/rom_8d9a4_c_c_a_c.s.
 *
 * Slotted between rom_8d9a4_c_c_a_c_a.o and the rest of stage1.ld.
 *
 * Allocates a 0x540-byte block in EWRAM, zeroes it with DMA3_CLEAR, and returns
 * it. `pop {r1}` is the return-value tell.
 */
#include "dma.h"
extern void *galloc_ewram(int tag, int size);

void *AllocGlobal1F(void)
{
    void *p;

    p = galloc_ewram(0x1f, 0xa8 << 3);
    DMA3_CLEAR(p, 0x540);
    return p;
}
